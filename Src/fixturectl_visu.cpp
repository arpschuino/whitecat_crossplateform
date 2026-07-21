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
*
* Rouleaux REGROUPES par CATEGORIE (Intensity/Position/Color/Gobo/Beam/Framing/Control/Other) avec
* en-tetes + separateurs ; colonne de FILTRES a gauche (afficher/masquer par categorie) ; defilement
* horizontal FLUIDE (offset au pixel + clipping) ; fenetre redimensionnable (poignee coin bas-droit).
 **/

#include "wc_tus.h"
#include "gui_boutons_rebuild1.h"
#include "fixturectl_visu.h"
#include <cmath>
#include <cstring>
#include <cctype>
#include <string>
#include <algorithm>

// --- Geometrie des rouleaux (thumbwheels verticaux) : partagee rendu <-> logique ---
static const int FXC_ENC_X0 = 125;   // x du centre du 1er rouleau (scroll=0), relatif a xf (colonne de filtres a gauche)
static const int FXC_ENC_CY = 175;   // y du centre des rouleaux, relatif a yf (place les en-tetes de categorie au-dessus)
static const int FXC_ENC_DX = 68;    // pas horizontal de BASE (module par +/-3 selon la categorie)
static const int FXC_ROL_W  = 30;    // largeur d'un rouleau
static const int FXC_ROL_H  = 130;   // hauteur d'un rouleau
static const int FXC_MAXENC = 64;    // nb max de rouleaux (gros devices : Robin T.5 = ~39)
static const int FXC_GAP_SAME = 3;   // dans une meme categorie : rouleaux plus SERRES (pas = FXC_ENC_DX - FXC_GAP_SAME)
static const int FXC_GAP_CAT  = 18;  // entre deux categories : ecart NETTEMENT plus large (pas = FXC_ENC_DX + FXC_GAP_CAT)

// Colonne de boutons "filtres de categorie" a gauche (afficher/masquer par categorie).
static const int FXC_LEFT_X  = 8;    // x des boutons (relatif a xf)
static const int FXC_LEFT_W  = 74;   // largeur d'un bouton (le 1er rouleau demarre a FXC_ENC_X0 -> ~28px de marge)
static const int FXC_BTN_Y0  = 96;   // y du 1er bouton (relatif a yf)
static const int FXC_BTN_H   = 18;   // hauteur d'un bouton
static const int FXC_BTN_GAP = 4;    // espace vertical entre boutons

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

// Libelle ajuste a la largeur d'un rouleau : tronque avec "…" si trop long. true si tronque.
static bool fxc_fit_label(const std::string& key, std::string& out)
{
    out = fxc_label(key);
    if(petitchiffre.TextWidth(out.c_str()) <= FXC_ENC_DX-4) return false;
    const char* ell = "\xE2\x80\xA6";
    while(!out.empty()){
        out.pop_back();
        std::string cand = out + ell;
        if(petitchiffre.TextWidth(cand.c_str()) <= FXC_ENC_DX-4){ out = cand; return true; }
    }
    out = ell; return true;
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

// --- Categories d'attributs (regroupement + filtres) ---
enum { FXC_CAT_INT=0, FXC_CAT_POS, FXC_CAT_COLOR, FXC_CAT_GOBO, FXC_CAT_BEAM, FXC_CAT_FRAMING, FXC_CAT_CONTROL, FXC_CAT_OTHER, FXC_CAT_N };
static bool g_fxc_cat_hidden[FXC_CAT_N] = {false};   // filtres afficher/masquer

static bool ci_contains(const std::string& hay, const char* needle)
{
    std::string h=hay, n=needle;
    for(size_t i=0;i<h.size();++i) h[i]=(char)tolower((unsigned char)h[i]);
    for(size_t i=0;i<n.size();++i) n[i]=(char)tolower((unsigned char)n[i]);
    return h.find(n)!=std::string::npos;
}

// Categorie d'un attribut d'apres son nom GDTF (ordre des tests = priorite).
static int fxc_category(const std::string& key)
{
    if(fxc_is_int(key)) return FXC_CAT_INT;
    if(ci_contains(key,"pan")||ci_contains(key,"tilt")||ci_contains(key,"position")) return FXC_CAT_POS;
    if(ci_contains(key,"color")||ci_contains(key,"cto")||ci_contains(key,"ctc")||ci_contains(key,"ctb")
       ||ci_contains(key,"tint")||ci_contains(key,"cri")||ci_contains(key,"hue")||ci_contains(key,"saturation")) return FXC_CAT_COLOR;
    if(ci_contains(key,"gobo")||ci_contains(key,"animation")) return FXC_CAT_GOBO;
    if(ci_contains(key,"blade")||ci_contains(key,"shaper")||ci_contains(key,"fram")||ci_contains(key,"knife")) return FXC_CAT_FRAMING;
    if(ci_contains(key,"zoom")||ci_contains(key,"focus")||ci_contains(key,"iris")||ci_contains(key,"frost")
       ||ci_contains(key,"prism")||ci_contains(key,"shutter")||ci_contains(key,"strobe")||ci_contains(key,"beam")) return FXC_CAT_BEAM;
    if(ci_contains(key,"control")||ci_contains(key,"function")||ci_contains(key,"led")||ci_contains(key,"macro")
       ||ci_contains(key,"reset")||ci_contains(key,"lamp")||ci_contains(key,"fan")||ci_contains(key,"speed")) return FXC_CAT_CONTROL;
    return FXC_CAT_OTHER;
}
static const char* fxc_category_name(int c)
{
    switch(c){
        case FXC_CAT_INT:     return "Intensity";
        case FXC_CAT_POS:     return "Position";
        case FXC_CAT_COLOR:   return "Color";
        case FXC_CAT_GOBO:    return "Gobo";
        case FXC_CAT_BEAM:    return "Beam";
        case FXC_CAT_FRAMING: return "Framing";
        case FXC_CAT_CONTROL: return "Control";
    }
    return "Other";
}

// Liste ordonnee des categories PRESENTES dans la selection (filtres ignores). Retourne le nombre.
static int fxc_present_cats(int* out)
{
    bool pres[FXC_CAT_N]; for(int c=0;c<FXC_CAT_N;c++) pres[c]=false;
    for(int c=1;c<514;c++) if(Selected_Channel[c]==1){ pres[FXC_CAT_INT]=true; break; }
    for(size_t f=0;f<wc_patch.size();f++){
        if(wc_patch[f].channels.empty()) continue;
        int circ=(int)wc_patch[f].channels[0].circuit;
        if(!fxc_circuit_selected_device(circ)) continue;
        for(size_t c=0;c<wc_patch[f].channels.size();c++){
            const wc::Channel& ch=wc_patch[f].channels[c];
            if(ch.attribute==wc::ATTR_DIMMER) continue;
            pres[fxc_category(fxc_chan_key(ch))]=true;
        }
    }
    int n=0; for(int c=0;c<FXC_CAT_N;c++) if(pres[c]) out[n++]=c;
    return n;
}

// Liste des attributs (clefs) presents dans la selection : Int d'abord (si un circuit selectionne),
// puis les noms d'attributs NON-Dimmer des devices selectionnes, dedupliques, puis REGROUPES par
// categorie (stable : ordre DMX conserve dans chaque groupe). Retourne le nombre (<= maxn).
static int fxc_build_display(std::string* out, int maxn)
{
    int n=0;
    if(!g_fxc_cat_hidden[FXC_CAT_INT])
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
            if(g_fxc_cat_hidden[fxc_category(key)]) continue;   // categorie masquee (filtre gauche)
            bool seen=false; for(int k=0;k<n;k++) if(out[k]==key){ seen=true; break; }
            if(!seen) out[n++]=key;
        }
    }
    std::stable_sort(out, out+n, [](const std::string& a, const std::string& b){ return fxc_category(a) < fxc_category(b); });
    return n;
}

// Valeur affichee d'un attribut + drapeau "mixte" (plusieurs devices selectionnes aux valeurs
// differentes -> le nombre n'a plus de sens, on affichera "..."). Pour Int : bufferSaisie[circuit].
static void fxc_value_and_mixed(const std::string& key, int& val, bool& mixed)
{
    mixed=false; int first=-1;
    if(fxc_is_int(key)){
        for(int c=1;c<514;c++) if(Selected_Channel[c]==1){ int v=(int)bufferSaisie[c]; if(first<0)first=v; else if(v!=first) mixed=true; }
        val = first<0?0:first; return;
    }
    for(size_t f=0; f<wc_patch.size(); f++){
        if(wc_patch[f].channels.empty()) continue;
        int circ=(int)wc_patch[f].channels[0].circuit;
        if(!fxc_circuit_selected_device(circ)) continue;
        for(size_t c=0;c<wc_patch[f].channels.size();c++){
            const wc::Channel& ch=wc_patch[f].channels[c];
            if(ch.attribute==wc::ATTR_DIMMER) continue;
            if(fxc_chan_key(ch)!=key) continue;
            int v=(int)output_devval[ch.coarse_addr];
            if(first<0)first=v; else if(v!=first) mixed=true;
            break;
        }
    }
    val = first<0?0:first;
}

// --- Slots nommes (parametres a crans : gobo, roue de couleur, prisme, modes...) ---

// Canal de reference pour <key> (dernier device selectionne en priorite, sinon le 1er). NULL si aucun.
static const wc::Channel* fxc_ref_channel(const std::string& key)
{
    const wc::Channel* fallback=nullptr;
    for(size_t f=0; f<wc_patch.size(); f++){
        if(wc_patch[f].channels.empty()) continue;
        int circ=(int)wc_patch[f].channels[0].circuit;
        if(!fxc_circuit_selected_device(circ)) continue;
        for(size_t c=0;c<wc_patch[f].channels.size();c++){
            const wc::Channel& ch=wc_patch[f].channels[c];
            if(ch.attribute==wc::ATTR_DIMMER) continue;
            if(fxc_chan_key(ch)!=key) continue;
            if(circ==last_ch_selected) return &ch;   // priorite au dernier selectionne
            if(!fallback) fallback=&ch;
            break;
        }
    }
    return fallback;
}
static inline bool fxc_is_slotted(const wc::Channel* ch){ return ch && ch->slots.size()>=2; }

// Index du slot contenant la valeur 16 bit <val> (le plus grand from16 <= val), -1 si aucun.
static int fxc_slot_index(const wc::Channel* ch, int val)
{
    if(!ch) return -1;
    int idx=-1;
    for(size_t i=0;i<ch->slots.size();i++){ if((int)ch->slots[i].from16 <= val) idx=(int)i; else break; }
    return idx;
}

// Saute au slot precedent/suivant (dir = -1/+1) sur CHAQUE device selectionne, chacun dans SA table.
static void fxc_step_slot(const std::string& key, int dir)
{
    for(size_t f=0; f<wc_patch.size(); f++){
        if(wc_patch[f].channels.empty()) continue;
        int circ=(int)wc_patch[f].channels[0].circuit;
        if(!fxc_circuit_selected_device(circ)) continue;
        for(size_t c=0;c<wc_patch[f].channels.size();c++){
            const wc::Channel& ch=wc_patch[f].channels[c];
            if(ch.attribute==wc::ATTR_DIMMER) continue;
            if(fxc_chan_key(ch)!=key) continue;
            if(ch.slots.size()>=2){
                int o=(int)ch.coarse_addr;
                if(o>0 && o<514){
                    int idx=fxc_slot_index(&ch,(int)output_devval[o]); if(idx<0) idx=0;
                    idx+=dir; if(idx<0)idx=0; if(idx>=(int)ch.slots.size())idx=(int)ch.slots.size()-1;
                    output_devval[o]=ch.slots[idx].from16;
                }
            }
            break;
        }
    }
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

// ============================================================================
// Layout horizontal + defilement FLUIDE au pixel.
//   cx_rel[i] = decalage du centre du rouleau i par rapport au centre du 1er (cx_rel[0]=0).
//   ecart = FXC_ENC_DX +FXC_GAP entre deux categories, -FXC_GAP dans une meme categorie.
//   screen center = xf + FXC_ENC_X0 + cx_rel[i] - g_fxc_scroll_px.
// ============================================================================
static int g_fxc_scroll_px = 0;   // offset de defilement, en pixels

static void fxc_layout(const std::string* disp, int n, int* cx_rel, int& content_w)
{
    int prev_cat=-1, x=0;
    for(int i=0;i<n;i++){
        int cat = fxc_category(disp[i]);
        if(i>0) x += (cat!=prev_cat) ? (FXC_ENC_DX + FXC_GAP_CAT) : (FXC_ENC_DX - FXC_GAP_SAME);
        cx_rel[i]=x; prev_cat=cat;
    }
    content_w = (n>0) ? (x + FXC_ROL_W) : 0;   // du bord gauche du 1er au bord droit du dernier
}
static inline int fxc_view_left (int xf){ return xf + FXC_ENC_X0 - 18; }   // -18 : loge le bouton home (32 de large) sans rognage
static inline int fxc_view_right(int xf){ return xf + fixturectl_window_w - 12; }
static inline int fxc_view_w    (int xf){ return fxc_view_right(xf) - fxc_view_left(xf); }
static inline int fxc_scx(int xf, int i, const int* cx_rel){ return xf + FXC_ENC_X0 + cx_rel[i] - g_fxc_scroll_px; }
static void fxc_clamp_scroll(int content_w, int xf){ int m=content_w - fxc_view_w(xf); if(m<0)m=0; if(g_fxc_scroll_px>m)g_fxc_scroll_px=m; if(g_fxc_scroll_px<0)g_fxc_scroll_px=0; }

// Geometrie de la barre (pixel) ; renvoie true si un ascenseur est necessaire.
static bool fxc_scrollbar_geom(int xf, int yf, int content_w, int& x0, int& sbY, int& trackw, int& thumbx, int& thumbw)
{
    x0     = xf + FXC_ENC_X0 - 15;
    sbY    = yf + fixturectl_window_h - 28;
    trackw = (xf + fixturectl_window_w - 25) - x0;
    int vw = fxc_view_w(xf);
    if(content_w<=vw){ thumbw=trackw; thumbx=x0; return false; }
    thumbw = (int)((long)trackw*vw/content_w); if(thumbw<20) thumbw=20;
    int maxs = content_w - vw;
    thumbx = x0 + (int)((long)(trackw-thumbw)*g_fxc_scroll_px/maxs);
    return true;
}

// Attribut (clef) de l'encodeur VISIBLE situe sous (px,py), ou "" si aucun.
static std::string fxc_attr_at_point(int xf, int yf, int px, int py)
{
    std::string disp[FXC_MAXENC]; int n = fxc_build_display(disp, FXC_MAXENC);
    int cx_rel[FXC_MAXENC], content_w; fxc_layout(disp, n, cx_rel, content_w); fxc_clamp_scroll(content_w, xf);
    int cy = yf + FXC_ENC_CY, VL = fxc_view_left(xf), VR = fxc_view_right(xf);
    for(int i=0;i<n;i++){
        int scx = fxc_scx(xf, i, cx_rel);
        if(scx+FXC_ROL_W/2 < VL || scx-FXC_ROL_W/2 > VR) continue;   // hors viewport
        if(px>scx-FXC_ROL_W/2 && px<scx+FXC_ROL_W/2 && py>cy-FXC_ROL_H/2 && py<cy+FXC_ROL_H/2) return disp[i];
    }
    return std::string();
}

// Dessine un rouleau vertical (thumbwheel) : corps + crans defilants + repere + textes (centres).
// mixed = plusieurs devices aux valeurs differentes -> "..." au lieu du nombre.
// slotted = parametre a crans -> haut = nom d'attribut, bas = "< nom_slot >" (fleches de navigation).
static void fxc_draw_encoder(int cx, int cy, const std::string& key, int refval, bool mixed, bool hovered,
                             bool slotted, const std::string& slotName)
{
    int w = FXC_ROL_W, h = FXC_ROL_H;
    int left = cx - w/2, top = cy - h/2;

    Rect Body(Vec2D(left, top), Vec2D(w, h));
    Body.SetRoundness(3);
    Body.SetLineWidth(epaisseur_ligne_fader);
    Body.Draw(CouleurGrisAnthracite);

    int halfh = h/2;
    float peak = 0.55f;
    for(int y=1; y<h-1; y++)
    {
        int dy = y-halfh; if(dy<0) dy=-dy;
        float a = 1.0f - (float)dy/(float)halfh;
        if(a<0) a=0;
        Line(Vec2D(left+1, top+y), Vec2D(left+w-1, top+y)).Draw(CouleurGrisClair.WithAlpha(a*a*a*peak));
    }
    Body.DrawOutline(hovered ? Rgba(1,1,1) : CouleurGrisClair);

    const int spacing = 11;
    int scroll = (int)(((long)(refval >> 8)) % spacing);
    for(int y = top + spacing - scroll; y < top + h - 2; y += spacing)
        if(y > top + 2) Line(Vec2D(left+3, y), Vec2D(left+w-3, y)).Draw(CouleurGrisClair);

    Line(Vec2D(left, cy), Vec2D(left+w-1, cy)).Draw(CouleurFader);   // repere de lecture central

    // HAUT : valeur DMX (continu) ou libelle d'attribut (a crans)
    if(slotted){
        std::string lab; fxc_fit_label(key, lab);
        petitchiffre.Print(lab.c_str(), cx - petitchiffre.TextWidth(lab.c_str())/2, top - 6);
    } else {
        std::string vs = mixed ? std::string("\xE2\x80\xA6") : ol::ToString((int)(refval >> 8));
        petitchiffre.Print(vs.c_str(), cx - petitchiffre.TextWidth(vs.c_str())/2, top - 6);
    }

    // BAS : libelle (continu) ou  < nom_slot >  (a crans, avec fleches de navigation)
    int by = top + h + 16;
    if(slotted){
        petitchiffre.Print("<", cx - FXC_ENC_DX/2 + 4, by);
        petitchiffre.Print(">", cx + FXC_ENC_DX/2 - 9, by);
        std::string sn = mixed ? std::string("\xE2\x80\xA6") : slotName;
        int maxw = FXC_ENC_DX - 24;
        if(petitchiffre.TextWidth(sn.c_str())>maxw){
            const char* ell="\xE2\x80\xA6";
            while(!sn.empty()){ sn.pop_back(); std::string c=sn+ell; if(petitchiffre.TextWidth(c.c_str())<=maxw){ sn=c; break; } }
        }
        petitchiffre.Print(sn.c_str(), cx - petitchiffre.TextWidth(sn.c_str())/2, by);
    } else {
        std::string lab; fxc_fit_label(key, lab);
        petitchiffre.Print(lab.c_str(), cx - petitchiffre.TextWidth(lab.c_str())/2, by);
    }
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

    int nsel=0;
    for(int c=1;c<514;c++) if(Selected_Channel[c]==1) nsel++;

    std::string disp[FXC_MAXENC];
    int n = fxc_build_display(disp, FXC_MAXENC);

    char hdr[96];
    if(nsel==0) sprintf(hdr, "Select one or more channels");
    else        sprintf(hdr, "%d channel(s) selected", nsel);
    petitchiffre.Print(hdr, xf + 30, yf + 52);

    // colonne de filtres de categorie a gauche (afficher/masquer par categorie)
    {
        int pc[FXC_CAT_N]; int npc = fxc_present_cats(pc);
        for(int i=0;i<npc;i++){
            int cat = pc[i];
            int by = yf + FXC_BTN_Y0 + i*(FXC_BTN_H+FXC_BTN_GAP);
            Rect B(Vec2D(xf+FXC_LEFT_X, by), Vec2D(FXC_LEFT_W, FXC_BTN_H)); B.SetRoundness(4);
            B.Draw(g_fxc_cat_hidden[cat] ? CouleurGrisAnthracite : CouleurGrisMoyen);   // actif = plus clair
            B.DrawOutline(CouleurGrisClair);
            petitchiffre.Print(fxc_category_name(cat), xf+FXC_LEFT_X+8, by+13);
        }
    }

    if(n<=0) return(0);

    int cx_rel[FXC_MAXENC], content_w; fxc_layout(disp, n, cx_rel, content_w); fxc_clamp_scroll(content_w, xf);
    int cy = yf + FXC_ENC_CY, VL = fxc_view_left(xf), VR = fxc_view_right(xf);

    // clip : borne gauche juste APRES les boutons (pas VL) -> le texte (en-tetes) peut s'afficher dans
    // la zone libre entre les boutons et la 1re roue ; borne droite = bord fenetre.
    int clipLeft = xf + FXC_LEFT_X + FXC_LEFT_W + 3;
    int clipTop = cy - FXC_ROL_H/2 - 36, clipBot = cy + FXC_ROL_H/2 + 44;   // au-dessus des en-tetes de categorie
    Canvas::SetClipping(clipLeft, clipTop, VR-clipLeft, clipBot-clipTop);

    int prev_cat_all=-1, prev_scx_all=0, prev_vis_cat=-1;
    std::string hov_full; int hov_scx=0; bool hov_trunc=false;   // label/slot survole (info-bulle du texte complet)
    for(int i=0;i<n;i++)
    {
        int scx = fxc_scx(xf, i, cx_rel);
        int cat = fxc_category(disp[i]);

        // separateur 2px au milieu du gap entre deux categories (clip gere les bords)
        if(prev_cat_all>=0 && cat!=prev_cat_all){
            int sx = (prev_scx_all + scx)/2;
            Rect Sep(Vec2D(sx-1, cy-FXC_ROL_H/2-4), Vec2D(2, FXC_ROL_H+8)); Sep.Draw(CouleurGrisClair);
        }

        bool culled = (scx+FXC_ROL_W/2+2 < VL || scx-FXC_ROL_W/2-2 > VR);
        if(!culled){
            // en-tete de categorie (centre) au 1er rouleau VISIBLE de chaque groupe
            if(cat != prev_vis_cat){
                const char* cn = fxc_category_name(cat); int cnw = petitchiffre.TextWidth(cn);
                int htx = scx - cnw/2;
                if(htx < clipLeft+1) htx = clipLeft+1;         // peut deborder dans la zone entre boutons et 1re roue
                if(htx + cnw > VR-1) htx = VR-1 - cnw;
                petitchiffre.Print(cn, htx, cy - FXC_ROL_H/2 - 24);
            }
            prev_vis_cat = cat;

            bool hov = (window_focus_id==W_FIXTURECTL &&
                        mouse_x>scx-FXC_ROL_W/2 && mouse_x<scx+FXC_ROL_W/2 && mouse_y>cy-FXC_ROL_H/2 && mouse_y<cy+FXC_ROL_H/2);
            int val; bool mixed; fxc_value_and_mixed(disp[i], val, mixed);
            const wc::Channel* refCh = fxc_ref_channel(disp[i]);
            bool slotted = fxc_is_slotted(refCh);
            std::string slotName;
            if(slotted){ int si=fxc_slot_index(refCh, val); if(si>=0) slotName=refCh->slots[si].name; }
            fxc_draw_encoder(scx, cy, disp[i], val, mixed, hov, slotted, slotName);
            if(hov){ strncpy(fixturectl_wheel_hover, disp[i].c_str(), 23); fixturectl_wheel_hover[23]=0; }
            // survol du LABEL/slot (sous le rouleau) -> info-bulle du texte complet
            if(window_focus_id==W_FIXTURECTL && mouse_x>scx-FXC_ENC_DX/2 && mouse_x<scx+FXC_ENC_DX/2
               && mouse_y>cy+FXC_ROL_H/2+6 && mouse_y<cy+FXC_ROL_H/2+22){
                hov_scx=scx;
                if(slotted){ hov_full = mixed?std::string():slotName; hov_trunc = petitchiffre.TextWidth(hov_full.c_str())>FXC_ENC_DX-24; }
                else { std::string tmp; hov_trunc=fxc_fit_label(disp[i],tmp); hov_full=fxc_label(disp[i]); }
            }

            int hy = cy + FXC_ROL_H/2 + 26;
            Rect Home(Vec2D(scx-16, hy), Vec2D(32,14)); Home.SetRoundness(3);
            Home.Draw(CouleurGrisAnthracite);
            Home.DrawOutline(CouleurGrisClair);
            petitpetitchiffre.Print("home", scx - petitpetitchiffre.TextWidth("home")/2, hy+11);
        }
        prev_cat_all = cat; prev_scx_all = scx;
    }

    Canvas::DisableClipping();

    // ascenseur horizontal (hors clip)
    {
        int x0, sbY, trackw, thumbx, thumbw;
        if(fxc_scrollbar_geom(xf, yf, content_w, x0, sbY, trackw, thumbx, thumbw))
        {
            Rect Track(Vec2D(x0, sbY), Vec2D(trackw, 8)); Track.SetRoundness(3); Track.Draw(CouleurGrisAnthracite);
            Rect Thumb(Vec2D(thumbx, sbY), Vec2D(thumbw, 8)); Thumb.SetRoundness(3); Thumb.Draw(CouleurGrisMoyen);
        }
    }

    // info-bulle : texte complet (nom d'attribut ou de slot) survole s'il est tronque
    if(!hov_full.empty() && hov_trunc){
        int tw = petitchiffre.TextWidth(hov_full.c_str())+10;
        int tx = hov_scx - tw/2;
        if(tx < xf+4) tx = xf+4;
        if(tx+tw > xf+fixturectl_window_w-4) tx = xf+fixturectl_window_w-4-tw;
        int ty = cy + FXC_ROL_H/2 + 52;   // sous le bouton home
        Rect Tip(Vec2D(tx, ty-11), Vec2D(tw,14)); Tip.SetRoundness(3);
        Tip.Draw(CouleurGrisAnthracite); Tip.DrawOutline(CouleurFader);
        petitchiffre.Print(hov_full.c_str(), tx+5, ty);
    }

    // poignee de redimensionnement (coin bas-droit) : 3 petits traits diagonaux
    {
        int gx = xf+fixturectl_window_w, gy = yf+fixturectl_window_h;
        for(int i=4;i<=12;i+=4) Line(Vec2D(gx-i-2, gy-4), Vec2D(gx-4, gy-i-2)).Draw(CouleurGrisClair);
    }

    return(0);
}

// Attribut (clef) du bouton home VISIBLE situe sous (px,py), ou "" si aucun.
static std::string fxc_home_at_point(int xf, int yf, int px, int py)
{
    std::string disp[FXC_MAXENC]; int n = fxc_build_display(disp, FXC_MAXENC);
    int cx_rel[FXC_MAXENC], content_w; fxc_layout(disp, n, cx_rel, content_w); fxc_clamp_scroll(content_w, xf);
    int cy = yf + FXC_ENC_CY, hy = cy + FXC_ROL_H/2 + 26, VL = fxc_view_left(xf), VR = fxc_view_right(xf);
    for(int i=0;i<n;i++){
        int scx = fxc_scx(xf, i, cx_rel);
        if(scx+16 < VL || scx-16 > VR) continue;
        if(px>scx-16 && px<scx+16 && py>hy && py<hy+14) return disp[i];
    }
    return std::string();
}

// Bouton de filtre de categorie sous (px,py), ou -1.
static int fxc_button_at_point(int xf, int yf, int px, int py)
{
    int pc[FXC_CAT_N]; int npc = fxc_present_cats(pc);
    for(int i=0;i<npc;i++){
        int by = yf + FXC_BTN_Y0 + i*(FXC_BTN_H+FXC_BTN_GAP);
        if(px>xf+FXC_LEFT_X && px<xf+FXC_LEFT_X+FXC_LEFT_W && py>by && py<by+FXC_BTN_H) return pc[i];
    }
    return -1;
}

// Fleche de slot ("<" ou ">") sous (px,py) pour un rouleau a crans VISIBLE : renvoie la clef + dir (-1/+1), ou "".
static std::string fxc_arrow_at_point(int xf, int yf, int px, int py, int& dir)
{
    dir=0;
    std::string disp[FXC_MAXENC]; int n=fxc_build_display(disp,FXC_MAXENC);
    int cx_rel[FXC_MAXENC], content_w; fxc_layout(disp,n,cx_rel,content_w); fxc_clamp_scroll(content_w,xf);
    int cy=yf+FXC_ENC_CY, VL=fxc_view_left(xf), VR=fxc_view_right(xf);
    int by = cy + FXC_ROL_H/2 + 16;
    if(py<by-10 || py>by+4) return std::string();
    for(int i=0;i<n;i++){
        int scx=fxc_scx(xf,i,cx_rel);
        if(scx+FXC_ROL_W/2<VL || scx-FXC_ROL_W/2>VR) continue;
        if(!fxc_is_slotted(fxc_ref_channel(disp[i]))) continue;
        if(px>scx-FXC_ENC_DX/2 && px<scx-FXC_ENC_DX/2+14){ dir=-1; return disp[i]; }
        if(px>scx+FXC_ENC_DX/2-14 && px<scx+FXC_ENC_DX/2){ dir=+1; return disp[i]; }
    }
    return std::string();
}

// Logique : drag vertical d'un encodeur = delta relatif (spin). Appelee bouton maintenu.
int do_logical_fixturectl(int xf, int yf)
{
    // detection d'un NOUVEAU grab via l'identite du clic (mouse_click_x/y fige a l'appui)
    static int         last_click_x = -999999, last_click_y = -999999;
    static std::string drag_key;              // "" = pas de drag d'encodeur
    static bool        drag_scroll = false;   // drag de la barre horizontale
    static bool        drag_resize = false;   // drag de la poignee de redimensionnement
    static int         drag_prev_y = 0;

    // filtres de categorie : clic SIMPLE fiable -> on consomme le clic (mouse_released=1) pour ne pas
    // dependre du deplacement de la souris (sinon re-cliquer au meme pixel ne re-declenchait pas).
    {
        int bc = fxc_button_at_point(xf, yf, mouse_x, mouse_y);
        if(bc>=0){ g_fxc_cat_hidden[bc] = !g_fxc_cat_hidden[bc]; mouse_released=1; return(0); }
    }
    // fleches "<" ">" des parametres a crans : clic simple -> slot precedent/suivant (consomme)
    {
        int dir=0; std::string ak = fxc_arrow_at_point(xf, yf, mouse_x, mouse_y, dir);
        if(!ak.empty()){ fxc_step_slot(ak, dir); mouse_released=1; return(0); }
    }

    if(mouse_click_x != last_click_x || mouse_click_y != last_click_y)
    {
        last_click_x = mouse_click_x; last_click_y = mouse_click_y;
        drag_key.clear(); drag_scroll = false; drag_resize = false;

        bool in_grip = mouse_click_x>xf+fixturectl_window_w-16 && mouse_click_x<xf+fixturectl_window_w+4
                    && mouse_click_y>yf+fixturectl_window_h-16 && mouse_click_y<yf+fixturectl_window_h+4;
        std::string home_key = in_grip ? std::string() : fxc_home_at_point(xf, yf, mouse_click_x, mouse_click_y);

        if(in_grip){ drag_resize = true; }
        else if(!home_key.empty()){ fxc_apply_home(home_key); }
        else
        {
            // ascenseur horizontal ?
            std::string disp[FXC_MAXENC]; int n = fxc_build_display(disp, FXC_MAXENC);
            int cx_rel[FXC_MAXENC], content_w; fxc_layout(disp, n, cx_rel, content_w); fxc_clamp_scroll(content_w, xf);
            int x0, sbY, trackw, thumbx, thumbw;
            bool has_sb = fxc_scrollbar_geom(xf, yf, content_w, x0, sbY, trackw, thumbx, thumbw);
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
        if(nw<340) nw=340; if(nh<340) nh=340;   // hauteur mini : loge en-tetes + rouleaux + home + tooltip + scrollbar
        if(nw>SCREEN_W-xf-8) nw=SCREEN_W-xf-8;
        if(nh>SCREEN_H-yf-8) nh=SCREEN_H-yf-8;
        fixturectl_window_w = nw; fixturectl_window_h = nh;
    }
    else if(drag_scroll)   // defilement fluide : le pouce (au pixel) suit la souris
    {
        std::string disp[FXC_MAXENC]; int n = fxc_build_display(disp, FXC_MAXENC);
        int cx_rel[FXC_MAXENC], content_w; fxc_layout(disp, n, cx_rel, content_w);
        int x0, sbY, trackw, thumbx, thumbw; fxc_scrollbar_geom(xf, yf, content_w, x0, sbY, trackw, thumbx, thumbw);
        int maxs = content_w - fxc_view_w(xf); if(maxs<0) maxs=0;
        if(trackw>thumbw){
            int rel = mouse_x - x0 - thumbw/2;
            int px  = (int)((long)rel*maxs/(trackw-thumbw));
            if(px<0) px=0; if(px>maxs) px=maxs;
            g_fxc_scroll_px = px;
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
