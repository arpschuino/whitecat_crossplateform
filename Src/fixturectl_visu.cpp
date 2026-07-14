/*-------------------------------------------------------------------------------------------------------------
 White Cat - Control Fixtures window (devices)
 Copyright (C) 2009-2016 Christoph Guillermet - Maintenance 2026 Jacques Bouault (arpschuino.fr)
 GNU General Public License v2 or later. See <http://www.gnu.org/licenses/>.
---------------------------------------------------------------------------------------------------------------*/

/**
* \file fixturectl_visu.cpp
* \brief Fenetre "Control Fixtures" : contrôle des attributs des devices par ENCODEURS RELATIFS.
*
* Modele console : la commande agit sur TOUTE la selection (Selected_Channel[]) et par ATTRIBUT.
* Un rouleau (encodeur infini) par attribut present dans la selection ; molette OU drag vertical =
* DELTA applique a chaque device selectionne (Pan de tous, Tilt de tous...) -> les ecarts entre
* lyres (fan) sont preserves. Ctrl = pas fin (1/65535), sinon pas grossier (1 DMX = 257).
* L'intensite (Int) applique le delta a bufferSaisie[circuit] de chaque device selectionne ;
* les autres attributs a output_devval[output] (endpoint live du crossfade LTP, cf. dmx_functions).
* La valeur affichee sur chaque rouleau = celle du DERNIER device selectionne (reference).
 **/

#include "wc_tus.h"
#include "gui_boutons_rebuild1.h"
#include "fixturectl_visu.h"
#include <cmath>

// --- Geometrie des rouleaux (thumbwheels verticaux) : partagee rendu <-> logique ---
static const int FXC_ENC_X0 = 45;    // x du centre du 1er rouleau, relatif a xf
static const int FXC_ENC_CY = 150;   // y du centre des rouleaux, relatif a yf
static const int FXC_ENC_DX = 68;    // pas horizontal
static const int FXC_ROL_W  = 30;    // largeur d'un rouleau
static const int FXC_ROL_H  = 130;   // hauteur d'un rouleau
static const int FXC_MAXENC = 12;    // nb max de rouleaux affiches

// Ordre d'affichage canonique des attributs.
static const uint8_t FXC_ATTR_ORDER[] = {
    wc::ATTR_DIMMER, wc::ATTR_PAN, wc::ATTR_TILT, wc::ATTR_ZOOM, wc::ATTR_SHUTTER1,
    wc::ATTR_COLORADD_R, wc::ATTR_COLORADD_G, wc::ATTR_COLORADD_B, wc::ATTR_COLORADD_W
};
static const int FXC_ATTR_ORDER_N = (int)(sizeof(FXC_ATTR_ORDER)/sizeof(FXC_ATTR_ORDER[0]));

// Libelle court d'un attribut (wc::AttrId).
static const char* fxc_attr_label(uint8_t a)
{
    switch(a){
        case wc::ATTR_DIMMER:     return "Int";
        case wc::ATTR_COLORADD_R: return "R";
        case wc::ATTR_COLORADD_G: return "G";
        case wc::ATTR_COLORADD_B: return "B";
        case wc::ATTR_COLORADD_W: return "W";
        case wc::ATTR_PAN:        return "Pan";
        case wc::ATTR_TILT:       return "Tilt";
        case wc::ATTR_SHUTTER1:   return "Shut";
        case wc::ATTR_ZOOM:       return "Zoom";
    }
    return "?";
}

// Un circuit est-il un device selectionne ? (selectionne ET porte au moins un channel)
static inline bool fxc_circuit_selected_device(int circuit)
{
    return circuit>0 && circuit<514 && Selected_Channel[circuit]==1;
}

// L'attribut <attr> est-il present dans la selection ?
//   - Int (Dimmer) : des qu'UN circuit est selectionne (circuit classique OU device) -> encodeur de level.
//   - autres attributs : uniquement si un DEVICE selectionne les porte.
static bool fxc_attr_present(uint8_t attr)
{
    if(attr==wc::ATTR_DIMMER)
    {
        for(int c=1;c<514;c++) if(Selected_Channel[c]==1) return true;
        return false;
    }
    for(size_t f=0; f<wc_patch.size(); f++)
    {
        int circ = wc_patch[f].channels.empty() ? 0 : (int)wc_patch[f].channels[0].circuit;
        if(!fxc_circuit_selected_device(circ)) continue;
        for(size_t c=0;c<wc_patch[f].channels.size();c++)
            if(wc_patch[f].channels[c].attribute==attr) return true;
    }
    return false;
}

// Liste (ordre canonique) des attributs presents dans la selection. Retourne le nombre (<= maxn).
static int fxc_build_display(uint8_t* out, int maxn)
{
    int n=0;
    for(int k=0;k<FXC_ATTR_ORDER_N && n<maxn;k++)
        if(fxc_attr_present(FXC_ATTR_ORDER[k])) out[n++]=FXC_ATTR_ORDER[k];
    return n;
}

// Valeur de reference d'un attribut = valeur du dernier device selectionne (last_ch_selected) s'il
// porte l'attribut, sinon du 1er device selectionne. Pour Int : bufferSaisie[circuit].
static int fxc_reference_value(uint8_t attr)
{
    if(attr==wc::ATTR_DIMMER)   // level : tout circuit selectionne (classique ou device)
    {
        if(last_ch_selected>0 && last_ch_selected<514 && Selected_Channel[last_ch_selected]==1)
            return (int)bufferSaisie[last_ch_selected];
        for(int c=1;c<514;c++) if(Selected_Channel[c]==1) return (int)bufferSaisie[c];
        return 0;
    }
    int fallback = -1;
    for(size_t f=0; f<wc_patch.size(); f++)
    {
        if(wc_patch[f].channels.empty()) continue;
        int circ = (int)wc_patch[f].channels[0].circuit;
        if(!fxc_circuit_selected_device(circ)) continue;
        for(size_t c=0;c<wc_patch[f].channels.size();c++)
        {
            if(wc_patch[f].channels[c].attribute!=attr) continue;
            int val = (attr==wc::ATTR_DIMMER)
                        ? (int)bufferSaisie[circ]
                        : (int)output_devval[wc_patch[f].channels[c].coarse_addr];
            if(circ==last_ch_selected) return val;   // priorite au dernier selectionne
            if(fallback<0) fallback=val;
        }
    }
    return fallback<0 ? 0 : fallback;
}

// Applique un DELTA (16 bit) a l'attribut <attr> de TOUS les devices selectionnes.
// Preserve les ecarts entre lyres (chaque valeur += meme delta, clampee).
void fxc_apply_delta(uint8_t attr, int delta)
{
    if(delta==0) return;
    if(attr==wc::ATTR_DIMMER)   // level : applique a TOUS les circuits selectionnes (classiques + devices)
    {
        for(int c=1;c<514;c++)
            if(Selected_Channel[c]==1){ int v=(int)bufferSaisie[c]+delta; if(v<0)v=0; if(v>65535)v=65535; bufferSaisie[c]=(unsigned short)v; }
        return;
    }
    for(size_t f=0; f<wc_patch.size(); f++)
    {
        if(wc_patch[f].channels.empty()) continue;
        int circ = (int)wc_patch[f].channels[0].circuit;
        if(!fxc_circuit_selected_device(circ)) continue;
        for(size_t c=0;c<wc_patch[f].channels.size();c++)
        {
            if(wc_patch[f].channels[c].attribute!=attr) continue;
            if(attr==wc::ATTR_DIMMER)
            {
                int v=(int)bufferSaisie[circ]+delta;
                if(v<0)v=0; if(v>65535)v=65535;
                bufferSaisie[circ]=(unsigned short)v;
            }
            else
            {
                int o=(int)wc_patch[f].channels[c].coarse_addr;
                if(o>0 && o<514){ int v=(int)output_devval[o]+delta; if(v<0)v=0; if(v>65535)v=65535; output_devval[o]=(unsigned short)v; }
            }
            break; // 1 channel de cet attribut par device
        }
    }
}

// Remet l'attribut <attr> a sa valeur "home" (defaut GDTF) sur tous les devices selectionnes.
// Int (Dimmer) : home = 0 (intensite eteinte).
static void fxc_apply_home(uint8_t attr)
{
    if(attr==wc::ATTR_DIMMER){
        for(int c=1;c<514;c++) if(Selected_Channel[c]==1) bufferSaisie[c]=0;
        return;
    }
    for(size_t f=0; f<wc_patch.size(); f++){
        if(wc_patch[f].channels.empty()) continue;
        int circ=(int)wc_patch[f].channels[0].circuit;
        if(!fxc_circuit_selected_device(circ)) continue;
        for(size_t c=0;c<wc_patch[f].channels.size();c++){
            if(wc_patch[f].channels[c].attribute!=attr) continue;
            int o=(int)wc_patch[f].channels[c].coarse_addr;
            if(o>0&&o<514) output_devval[o]=output_devdefault[o];
            break;
        }
    }
}

// Attribut de l'encodeur situe sous le point (px,py), ou 0 (ATTR_NONE) si aucun.
static uint8_t fxc_attr_at_point(int xf, int yf, int px, int py)
{
    uint8_t disp[FXC_MAXENC];
    int n = fxc_build_display(disp, FXC_MAXENC);
    int cy = yf + FXC_ENC_CY;
    for(int s=0;s<n;s++)
    {
        int cx = xf + FXC_ENC_X0 + s*FXC_ENC_DX;
        if(px>cx-FXC_ROL_W/2 && px<cx+FXC_ROL_W/2 && py>cy-FXC_ROL_H/2 && py<cy+FXC_ROL_H/2)
            return disp[s];
    }
    return wc::ATTR_NONE;
}

// Dessine un rouleau vertical (thumbwheel) : corps gris + crans horizontaux qui defilent avec la
// valeur (effet de cylindre qui tourne) + repere de lecture central + libelle + valeur.
static void fxc_draw_encoder(int cx, int cy, uint8_t attr, int refval, bool hovered)
{
    int w = FXC_ROL_W, h = FXC_ROL_H;
    int left = cx - w/2, top = cy - h/2;

    Rect Body(Vec2D(left, top), Vec2D(w, h));
    Body.SetRoundness(3);                                            // bords a peine arrondis
    Body.SetLineWidth(epaisseur_ligne_fader);
    Body.Draw(CouleurGrisAnthracite);                               // base sombre (bords du cylindre)

    // degrade en bandes HORIZONTALES : reflet clair a mi-hauteur -> plus sombre vers le haut/bas
    // (rondeur d'un cylindre a axe horizontal, qu'on fait tourner de haut en bas)
    int halfh = h/2;
    float peak = 0.55f;   // reflet constant ; le survol surbrille le CONTOUR (cf. DrawOutline)
    for(int y=1; y<h-1; y++)
    {
        int dy = y-halfh; if(dy<0) dy=-dy;
        float a = 1.0f - (float)dy/(float)halfh;                    // 1 a mi-hauteur .. 0 en haut/bas
        if(a<0) a=0;
        Line(Vec2D(left+1, top+y), Vec2D(left+w-1, top+y)).Draw(CouleurGrisClair.WithAlpha(a*a*a*peak)); // a^3 : reflet resserre au centre
    }
    Body.DrawOutline(hovered ? Rgba(1,1,1) : CouleurGrisClair);   // survol = contour blanc ; sinon gris clair

    // crans horizontaux (gris clair) qui defilent selon la valeur -> le rouleau "tourne"
    const int spacing = 11;
    int scroll = (int)(((long)(refval >> 8)) % spacing);            // 0..spacing-1
    for(int y = top + spacing - scroll; y < top + h - 2; y += spacing)
        if(y > top + 2) Line(Vec2D(left+3, y), Vec2D(left+w-3, y)).Draw(CouleurGrisClair);

    // repere de lecture au centre
    Line(Vec2D(left, cy), Vec2D(left+w-1, cy)).Draw(CouleurFader);

    petitchiffre.Print(ol::ToString((int)(refval >> 8)), cx - 8, top - 6);   // valeur DMX 8 bit au-dessus
    petitchiffre.Print(fxc_attr_label(attr), cx - 10, top + h + 16);         // libelle en dessous
}

// Rendu de la fenetre. xf/yf = coin haut-gauche.
int fixturectl_window(int xf, int yf)
{
    Rect FixturePanel(Vec2D(xf, yf), Vec2D(fixturectl_window_w, fixturectl_window_h));
    FixturePanel.SetRoundness(15);
    FixturePanel.SetLineWidth(epaisseur_bordure_fenetre);
    FixturePanel.Draw(CouleurFond);
    if (window_focus_id == W_FIXTURECTL) { FixturePanel.DrawOutline(CouleurFader); }
    else                                 { FixturePanel.DrawOutline(CouleurLigne); }

    neuro.Print("CONTROL FIXTURES", xf + 100, yf + 30);

    fixturectl_wheel_hover = wc::ATTR_NONE;   // cible molette republiee chaque frame (0 = aucun encodeur survole)

    // compte des circuits selectionnes (pour l'en-tete)
    int nsel=0;
    for(int c=1;c<514;c++) if(Selected_Channel[c]==1) nsel++;

    uint8_t disp[FXC_MAXENC];
    int n = fxc_build_display(disp, FXC_MAXENC);

    char hdr[96];
    if(nsel==0) sprintf(hdr, "Select one or more channels");
    else        sprintf(hdr, "%d channel(s) selected  -  wheel / drag = relative (Ctrl = fine)", nsel);
    petitchiffre.Print(hdr, xf + 30, yf + 52);

    if(n<=0) return(0);

    int cy = yf + FXC_ENC_CY;
    for(int s=0;s<n;s++)
    {
        int cx = xf + FXC_ENC_X0 + s*FXC_ENC_DX;
        bool hov = (window_focus_id==W_FIXTURECTL &&
                    mouse_x>cx-FXC_ROL_W/2 && mouse_x<cx+FXC_ROL_W/2 && mouse_y>cy-FXC_ROL_H/2 && mouse_y<cy+FXC_ROL_H/2);
        fxc_draw_encoder(cx, cy, disp[s], fxc_reference_value(disp[s]), hov);
        if(hov) fixturectl_wheel_hover = disp[s];   // publie l'attribut survole (pour la molette)
        // bouton home sous le rouleau
        int hy = cy + FXC_ROL_H/2 + 26;
        Rect Home(Vec2D(cx-16, hy), Vec2D(32,14)); Home.SetRoundness(3);
        Home.Draw(CouleurGrisAnthracite);     // fond gris fonce
        Home.DrawOutline(CouleurGrisClair);   // bordure gris clair
        petitpetitchiffre.Print("home", cx-15, hy+11);
    }

    return(0);
}

// Attribut du bouton home situe sous (px,py), ou ATTR_NONE.
static uint8_t fxc_home_at_point(int xf, int yf, int px, int py)
{
    uint8_t disp[FXC_MAXENC];
    int n = fxc_build_display(disp, FXC_MAXENC);
    int cy = yf + FXC_ENC_CY;
    int hy = cy + FXC_ROL_H/2 + 26;
    for(int s=0;s<n;s++){
        int cx = xf + FXC_ENC_X0 + s*FXC_ENC_DX;
        if(px>cx-16 && px<cx+16 && py>hy && py<hy+14) return disp[s];
    }
    return wc::ATTR_NONE;
}

// Logique : drag vertical d'un encodeur = delta relatif (spin). Appelee bouton maintenu.
int do_logical_fixturectl(int xf, int yf)
{
    // detection d'un NOUVEAU grab via l'identite du clic (mouse_click_x/y fige a l'appui)
    static int     last_click_x = -999999, last_click_y = -999999;
    static uint8_t drag_attr = wc::ATTR_NONE;
    static int     drag_prev_y = 0;

    if(mouse_click_x != last_click_x || mouse_click_y != last_click_y)
    {
        last_click_x = mouse_click_x; last_click_y = mouse_click_y;
        // bouton home ? -> remet le defaut une fois, pas de drag
        uint8_t home_attr = fxc_home_at_point(xf, yf, mouse_click_x, mouse_click_y);
        if(home_attr != wc::ATTR_NONE){ fxc_apply_home(home_attr); drag_attr = wc::ATTR_NONE; }
        else { drag_attr = fxc_attr_at_point(xf, yf, mouse_click_x, mouse_click_y); }
        drag_prev_y  = mouse_y;
    }

    if(drag_attr != wc::ATTR_NONE)
    {
        int dy = drag_prev_y - mouse_y;   // haut = +, bas = - (frame a frame, relatif)
        if(dy != 0)
        {
            bool fine = (SDL_GetModState() & KMOD_CTRL) || index_false_control == 1;
            int unit  = fine ? 1 : 257;   // fin = 1/65535 ; grossier = 1 DMX
            fxc_apply_delta(drag_attr, dy * unit);
            drag_prev_y = mouse_y;
        }
    }
    return(0);
}
