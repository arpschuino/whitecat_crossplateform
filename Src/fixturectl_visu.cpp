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
* Un rouleau (encodeur infini) par ATTRIBUT present dans la selection ; molette OU drag vertical =
* DELTA applique a chaque device selectionne (Pan de tous, Tilt de tous...) -> les ecarts entre
* lyres (fan) sont preserves. Ctrl = pas fin (1/65535), sinon pas grossier (1 DMX = 257).
*
* [devices] Les attributs sont identifies par leur NOM GDTF (ch.name : "Pan", "Gobo1", "Prism1"...),
* pas par un AttrId fige -> TOUT canal non-Dimmer est pilotable (gobo, prisme, roue, CTC, iris...).
* Le sentinelle "\x01" designe l'intensite (Int), traitee a part : delta sur bufferSaisie[circuit].
* Les autres attributs vivent dans output_devval[output] (endpoint live du crossfade LTP, cf. dmx_functions).
* La valeur affichee sur chaque rouleau = celle du DERNIER device selectionne (reference).
 **/

#include "wc_tus.h"
#include "gui_boutons_rebuild1.h"
#include "fixturectl_visu.h"
#include <cmath>
#include <cstring>
#include <string>

// --- Geometrie des rouleaux (thumbwheels verticaux) : partagee rendu <-> logique ---
static const int FXC_ENC_X0 = 45;    // x du centre du 1er rouleau, relatif a xf
static const int FXC_ENC_CY = 150;   // y du centre des rouleaux, relatif a yf
static const int FXC_ENC_DX = 68;    // pas horizontal
static const int FXC_ROL_W  = 30;    // largeur d'un rouleau
static const int FXC_ROL_H  = 130;   // hauteur d'un rouleau
static const int FXC_MAXENC = 24;    // nb max de rouleaux enumeres (ascenseur horizontal a venir)

// Sentinelle "intensite" (Int) : un octet 0x01, jamais un nom d'attribut GDTF.
static const char FXC_INT[] = "\x01";
static inline bool fxc_is_int(const std::string& key){ return key.size()==1 && (unsigned char)key[0]==0x01; }

// Libelle court d'un attribut (mappe les noms GDTF connus vers un libelle compact, sinon nom brut).
static std::string fxc_label(const std::string& key)
{
    if(fxc_is_int(key))     return "Int";
    if(key=="ColorAdd_R")   return "R";
    if(key=="ColorAdd_G")   return "G";
    if(key=="ColorAdd_B")   return "B";
    if(key=="ColorAdd_W")   return "W";
    if(key=="ColorSub_C")   return "C";
    if(key=="ColorSub_M")   return "M";
    if(key=="ColorSub_Y")   return "Y";
    if(key=="Shutter1")     return "Shut";
    return key;   // nom GDTF tel quel (tronque au dessin s'il est trop long)
}

// Un circuit est-il un device selectionne ? (selectionne ET porte au moins un channel)
static inline bool fxc_circuit_selected_device(int circuit)
{
    return circuit>0 && circuit<514 && Selected_Channel[circuit]==1;
}

// Nom canonique GDTF d'un AttrId mappe ("" si non mappe) : fallback pour les devices charges d'un
// ancien show (fichier <= v3, sans ch.name) mais dont l'AttrId est connu.
static const char* fxc_attr_canonical(uint8_t a)
{
    switch(a){
        case wc::ATTR_PAN:        return "Pan";
        case wc::ATTR_TILT:       return "Tilt";
        case wc::ATTR_ZOOM:       return "Zoom";
        case wc::ATTR_SHUTTER1:   return "Shutter1";
        case wc::ATTR_COLORADD_R: return "ColorAdd_R";
        case wc::ATTR_COLORADD_G: return "ColorAdd_G";
        case wc::ATTR_COLORADD_B: return "ColorAdd_B";
        case wc::ATTR_COLORADD_W: return "ColorAdd_W";
    }
    return "";
}

// Clef d'un channel non-Dimmer = son nom GDTF ; a defaut, le nom canonique de son AttrId ; sinon "?".
static inline std::string fxc_chan_key(const wc::Channel& ch)
{
    if(ch.name[0]) return std::string(ch.name);
    const char* cn = fxc_attr_canonical(ch.attribute);
    return cn[0] ? std::string(cn) : std::string("?");
}

// Liste des attributs (clefs) presents dans la selection : Int d'abord (si un circuit selectionne),
// puis les noms d'attributs NON-Dimmer des devices selectionnes, dedupliques, en ordre de rencontre
// (ordre DMX du device). Retourne le nombre (<= maxn).
static int fxc_build_display(std::string* out, int maxn)
{
    int n=0;
    for(int c=1;c<514 && n<maxn;c++) if(Selected_Channel[c]==1){ out[n++]=FXC_INT; break; }
    for(size_t f=0; f<wc_patch.size() && n<maxn; f++)
    {
        if(wc_patch[f].channels.empty()) continue;
        int circ = (int)wc_patch[f].channels[0].circuit;
        if(!fxc_circuit_selected_device(circ)) continue;
        for(size_t c=0;c<wc_patch[f].channels.size() && n<maxn;c++)
        {
            const wc::Channel& ch = wc_patch[f].channels[c];
            if(ch.attribute==wc::ATTR_DIMMER) continue;      // l'intensite est portee par Int
            std::string key = fxc_chan_key(ch);
            bool seen=false; for(int k=0;k<n;k++) if(out[k]==key){ seen=true; break; }
            if(!seen) out[n++]=key;
        }
    }
    return n;
}

// Valeur de reference d'un attribut = valeur du dernier device selectionne (last_ch_selected) s'il
// porte l'attribut, sinon du 1er device selectionne. Pour Int : bufferSaisie[circuit].
static int fxc_reference_value(const std::string& key)
{
    if(fxc_is_int(key))   // level : tout circuit selectionne (classique ou device)
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
            const wc::Channel& ch = wc_patch[f].channels[c];
            if(ch.attribute==wc::ATTR_DIMMER) continue;
            if(fxc_chan_key(ch)!=key) continue;
            int val = (int)output_devval[ch.coarse_addr];
            if(circ==last_ch_selected) return val;   // priorite au dernier selectionne
            if(fallback<0) fallback=val;
        }
    }
    return fallback<0 ? 0 : fallback;
}

// Applique un DELTA (16 bit) a l'attribut <key> de TOUS les devices selectionnes.
// Preserve les ecarts entre lyres (chaque valeur += meme delta, clampee).
void fxc_apply_delta(const char* name, int delta)
{
    if(delta==0 || !name) return;
    std::string key(name);
    if(fxc_is_int(key))   // level : applique a TOUS les circuits selectionnes (classiques + devices)
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
            const wc::Channel& ch = wc_patch[f].channels[c];
            if(ch.attribute==wc::ATTR_DIMMER) continue;
            if(fxc_chan_key(ch)!=key) continue;
            int o=(int)ch.coarse_addr;
            if(o>0 && o<514){ int v=(int)output_devval[o]+delta; if(v<0)v=0; if(v>65535)v=65535; output_devval[o]=(unsigned short)v; }
            break; // 1 channel de cet attribut par device
        }
    }
}

// Remet l'attribut <key> a sa valeur "home" (defaut GDTF) sur tous les devices selectionnes.
// Int (Dimmer) : home = 0 (intensite eteinte).
static void fxc_apply_home(const std::string& key)
{
    if(fxc_is_int(key)){
        for(int c=1;c<514;c++) if(Selected_Channel[c]==1) bufferSaisie[c]=0;
        return;
    }
    for(size_t f=0; f<wc_patch.size(); f++){
        if(wc_patch[f].channels.empty()) continue;
        int circ=(int)wc_patch[f].channels[0].circuit;
        if(!fxc_circuit_selected_device(circ)) continue;
        for(size_t c=0;c<wc_patch[f].channels.size();c++){
            const wc::Channel& ch = wc_patch[f].channels[c];
            if(ch.attribute==wc::ATTR_DIMMER) continue;
            if(fxc_chan_key(ch)!=key) continue;
            int o=(int)ch.coarse_addr;
            if(o>0&&o<514) output_devval[o]=output_devdefault[o];
            break;
        }
    }
}

// --- ascenseur horizontal (device a plus de canaux que la fenetre n'affiche) ---
static int g_fxc_hscroll = 0;   // index du 1er rouleau visible

static int fxc_visible_count(){ int v=(fixturectl_window_w - FXC_ENC_X0 - 20)/FXC_ENC_DX; return v<1?1:v; }
static void fxc_clamp_hscroll(int n){ int m=n-fxc_visible_count(); if(m<0)m=0; if(g_fxc_hscroll>m)g_fxc_hscroll=m; if(g_fxc_hscroll<0)g_fxc_hscroll=0; }

// Geometrie de la barre ; renvoie true si un ascenseur est necessaire (n > rouleaux visibles).
static bool fxc_scrollbar_geom(int xf, int yf, int n, int& x0, int& sbY, int& trackw, int& thumbx, int& thumbw)
{
    x0     = xf + FXC_ENC_X0 - 15;
    sbY    = yf + fixturectl_window_h - 28;
    trackw = (xf + fixturectl_window_w - 25) - x0;
    int vis = fxc_visible_count();
    if(n<=vis){ thumbw=trackw; thumbx=x0; return false; }
    thumbw = trackw*vis/n; if(thumbw<20) thumbw=20;
    thumbx = x0 + (trackw-thumbw)*g_fxc_hscroll/(n-vis);
    return true;
}

// Attribut (clef) de l'encodeur VISIBLE situe sous le point (px,py), ou "" si aucun.
static std::string fxc_attr_at_point(int xf, int yf, int px, int py)
{
    std::string disp[FXC_MAXENC];
    int n = fxc_build_display(disp, FXC_MAXENC);
    fxc_clamp_hscroll(n);
    int vis = fxc_visible_count();
    int cy = yf + FXC_ENC_CY;
    for(int p=0; p<vis && p+g_fxc_hscroll<n; p++)
    {
        int cx = xf + FXC_ENC_X0 + p*FXC_ENC_DX;
        if(px>cx-FXC_ROL_W/2 && px<cx+FXC_ROL_W/2 && py>cy-FXC_ROL_H/2 && py<cy+FXC_ROL_H/2)
            return disp[p+g_fxc_hscroll];
    }
    return std::string();
}

// Dessine un rouleau vertical (thumbwheel) : corps gris + crans horizontaux qui defilent avec la
// valeur (effet de cylindre qui tourne) + repere de lecture central + libelle + valeur.
static void fxc_draw_encoder(int cx, int cy, const std::string& key, int refval, bool hovered)
{
    int w = FXC_ROL_W, h = FXC_ROL_H;
    int left = cx - w/2, top = cy - h/2;

    Rect Body(Vec2D(left, top), Vec2D(w, h));
    Body.SetRoundness(3);                                            // bords a peine arrondis
    Body.SetLineWidth(epaisseur_ligne_fader);
    Body.Draw(CouleurGrisAnthracite);                               // base sombre (bords du cylindre)

    // degrade en bandes HORIZONTALES : reflet clair a mi-hauteur -> plus sombre vers le haut/bas
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

    // libelle (tronque au pixel pour tenir sous le rouleau)
    std::string lab = fxc_label(key);
    while(lab.size()>3 && petitchiffre.TextWidth(lab.c_str()) > FXC_ENC_DX-10) lab.erase(lab.size()-1);
    petitchiffre.Print(lab.c_str(), cx - 10, top + h + 16);                  // libelle en dessous
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

    fixturectl_wheel_hover[0] = 0;   // cible molette republiee chaque frame ("" = aucun encodeur survole)

    // compte des circuits selectionnes (pour l'en-tete)
    int nsel=0;
    for(int c=1;c<514;c++) if(Selected_Channel[c]==1) nsel++;

    std::string disp[FXC_MAXENC];
    int n = fxc_build_display(disp, FXC_MAXENC);

    char hdr[96];
    if(nsel==0) sprintf(hdr, "Select one or more channels");
    else        sprintf(hdr, "%d channel(s) selected  -  wheel / drag = relative (Ctrl = fine)", nsel);
    petitchiffre.Print(hdr, xf + 30, yf + 52);

    if(n<=0) return(0);

    fxc_clamp_hscroll(n);
    int vis = fxc_visible_count();
    int cy = yf + FXC_ENC_CY;
    for(int p=0; p<vis && p+g_fxc_hscroll<n; p++)
    {
        int gi = p+g_fxc_hscroll;
        int cx = xf + FXC_ENC_X0 + p*FXC_ENC_DX;
        bool hov = (window_focus_id==W_FIXTURECTL &&
                    mouse_x>cx-FXC_ROL_W/2 && mouse_x<cx+FXC_ROL_W/2 && mouse_y>cy-FXC_ROL_H/2 && mouse_y<cy+FXC_ROL_H/2);
        fxc_draw_encoder(cx, cy, disp[gi], fxc_reference_value(disp[gi]), hov);
        if(hov){ strncpy(fixturectl_wheel_hover, disp[gi].c_str(), 23); fixturectl_wheel_hover[23]=0; }  // publie l'attribut survole
        // bouton home sous le rouleau
        int hy = cy + FXC_ROL_H/2 + 26;
        Rect Home(Vec2D(cx-16, hy), Vec2D(32,14)); Home.SetRoundness(3);
        Home.Draw(CouleurGrisAnthracite);     // fond gris fonce
        Home.DrawOutline(CouleurGrisClair);   // bordure gris clair
        petitpetitchiffre.Print("home", cx-15, hy+11);
    }

    // ascenseur horizontal (si plus de rouleaux que de place)
    {
        int x0, sbY, trackw, thumbx, thumbw;
        if(fxc_scrollbar_geom(xf, yf, n, x0, sbY, trackw, thumbx, thumbw))
        {
            Rect Track(Vec2D(x0, sbY), Vec2D(trackw, 8)); Track.SetRoundness(3); Track.Draw(CouleurGrisAnthracite);
            Rect Thumb(Vec2D(thumbx, sbY), Vec2D(thumbw, 8)); Thumb.SetRoundness(3); Thumb.Draw(CouleurGrisMoyen);
        }
    }

    // poignee de redimensionnement (coin bas-droit) : 3 petits traits diagonaux
    {
        int gx = xf+fixturectl_window_w, gy = yf+fixturectl_window_h;
        for(int i=4;i<=12;i+=4) Line(Vec2D(gx-i-2, gy-4), Vec2D(gx-4, gy-i-2)).Draw(CouleurGrisClair);
    }

    return(0);
}

// Attribut (clef) du bouton home situe sous (px,py), ou "" si aucun.
static std::string fxc_home_at_point(int xf, int yf, int px, int py)
{
    std::string disp[FXC_MAXENC];
    int n = fxc_build_display(disp, FXC_MAXENC);
    fxc_clamp_hscroll(n);
    int vis = fxc_visible_count();
    int cy = yf + FXC_ENC_CY;
    int hy = cy + FXC_ROL_H/2 + 26;
    for(int p=0; p<vis && p+g_fxc_hscroll<n; p++){
        int cx = xf + FXC_ENC_X0 + p*FXC_ENC_DX;
        if(px>cx-16 && px<cx+16 && py>hy && py<hy+14) return disp[p+g_fxc_hscroll];
    }
    return std::string();
}

// Logique : drag vertical d'un encodeur = delta relatif (spin). Appelee bouton maintenu.
int do_logical_fixturectl(int xf, int yf)
{
    // detection d'un NOUVEAU grab via l'identite du clic (mouse_click_x/y fige a l'appui)
    static int         last_click_x = -999999, last_click_y = -999999;
    static std::string drag_key;         // "" = pas de drag d'encodeur
    static bool        drag_scroll = false;   // drag de la barre horizontale
    static bool        drag_resize = false;   // drag de la poignee de redimensionnement
    static int         drag_prev_y = 0;

    if(mouse_click_x != last_click_x || mouse_click_y != last_click_y)
    {
        last_click_x = mouse_click_x; last_click_y = mouse_click_y;
        drag_key.clear(); drag_scroll = false; drag_resize = false;
        // poignee de redimensionnement (coin bas-droit) ? -> priorite
        bool in_grip = mouse_click_x>xf+fixturectl_window_w-16 && mouse_click_x<xf+fixturectl_window_w+4
                    && mouse_click_y>yf+fixturectl_window_h-16 && mouse_click_y<yf+fixturectl_window_h+4;
        std::string home_key = in_grip ? std::string() : fxc_home_at_point(xf, yf, mouse_click_x, mouse_click_y);
        if(in_grip){ drag_resize = true; }
        // bouton home ? -> remet le defaut une fois, pas de drag
        else if(!home_key.empty()){ fxc_apply_home(home_key); }
        else
        {
            // ascenseur horizontal ?
            std::string disp[FXC_MAXENC]; int n = fxc_build_display(disp, FXC_MAXENC); fxc_clamp_hscroll(n);
            int x0, sbY, trackw, thumbx, thumbw;
            bool has_sb = fxc_scrollbar_geom(xf, yf, n, x0, sbY, trackw, thumbx, thumbw);
            if(has_sb && mouse_click_x>x0 && mouse_click_x<x0+trackw && mouse_click_y>sbY-5 && mouse_click_y<sbY+13)
                drag_scroll = true;
            else
                drag_key = fxc_attr_at_point(xf, yf, mouse_click_x, mouse_click_y);
        }
        drag_prev_y  = mouse_y;
    }

    if(drag_resize)   // redimensionner : coin bas-droit suit la souris (bornes mini/ecran)
    {
        int nw = mouse_x - xf, nh = mouse_y - yf;
        if(nw<340) nw=340; if(nh<240) nh=240;
        if(nw>SCREEN_W-xf-8) nw=SCREEN_W-xf-8;
        if(nh>SCREEN_H-yf-8) nh=SCREEN_H-yf-8;
        fixturectl_window_w = nw; fixturectl_window_h = nh;
    }
    else if(drag_scroll)   // suivre le pouce sous la souris
    {
        std::string disp[FXC_MAXENC]; int n = fxc_build_display(disp, FXC_MAXENC);
        int vis = fxc_visible_count(); int maxs = n-vis; if(maxs<0) maxs=0;
        int x0, sbY, trackw, thumbx, thumbw; fxc_scrollbar_geom(xf, yf, n, x0, sbY, trackw, thumbx, thumbw);
        if(trackw>thumbw){
            int rel = mouse_x - x0 - thumbw/2;
            int s = rel*maxs/(trackw-thumbw);
            if(s<0) s=0; if(s>maxs) s=maxs;
            g_fxc_hscroll = s;
        }
    }
    else if(!drag_key.empty())
    {
        int dy = drag_prev_y - mouse_y;   // haut = +, bas = - (frame a frame, relatif)
        if(dy != 0)
        {
            bool fine = (SDL_GetModState() & KMOD_CTRL) || index_false_control == 1;
            int unit  = fine ? 1 : 257;   // fin = 1/65535 ; grossier = 1 DMX
            fxc_apply_delta(drag_key.c_str(), dy * unit);
            drag_prev_y = mouse_y;
        }
    }
    return(0);
}
