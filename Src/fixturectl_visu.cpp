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
#include <cstdio>
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
    if(ci_contains(key,"color")||ci_contains(key,"colour")||ci_contains(key,"cto")||ci_contains(key,"ctc")||ci_contains(key,"ctb")
       ||ci_contains(key,"tint")||ci_contains(key,"cri")||ci_contains(key,"hue")||ci_contains(key,"saturation")) return FXC_CAT_COLOR;  // "colour" = repli orthographe britannique
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
// [devices] Affichage "physique" facon EOS = aussi le signal "PROPORTIONNEL". true si l'attribut a une
// plage physique parlante (Angle, Hz, K°, ou None NON normalise comme CRI 80..90). Percent/ColorComponent
// et plage normalisee 0..1 (0..100) -> level. Donnee dispo a l'import ET au chargement (patch v7).
static bool fxc_show_physical(const wc::Channel* ch)
{
    if(!ch) return false;
    if(ch->phys_from == ch->phys_to) return false;                // pas de plage -> level
    switch(ch->phys_unit){
        case wc::PU_PERCENT:
        case wc::PU_COLORCOMPONENT: return false;                 // = level (suit dmx_view)
        case wc::PU_NONE: {                                        // None : physique si plage non normalisee
            float lo=ch->phys_from, hi=ch->phys_to; if(lo>hi){ float t=lo; lo=hi; hi=t; }
            bool n01 =(lo>-0.001f&&lo<0.001f&&hi>0.999f&&hi<1.001f);
            bool n100=(lo>-0.1f  &&lo<0.1f  &&hi>99.9f &&hi<100.1f);
            return !(n01 || n100);
        }
        default: return true;                                     // unite dimensionnelle -> physique
    }
}

// Nb de tranches STEP (non proportionnelles) d'un canal (modele "par tranches", patch v8+).
static int fxc_step_ranges(const wc::Channel* ch){
    int n=0; if(ch) for(size_t i=0;i<ch->ranges.size();++i) if(!ch->ranges[i].proportional) n++;
    return n;
}
// "slotted" = on AJOUTE un menu de modes sous la molette (modele EOS "molette + menu").
// Modele PAR TRANCHES (v8+) : slotted <=> le canal a au moins une tranche STEP (et des reperes a lister).
//   -> que du proportionnel (Pan, CRI, CTO, vitesse gobo, RGB...) = molette seule ;
//   -> tranches step (gobos, couleurs, macros, pulses...) = menu ; mixte (Iris, Shutter, Gobo) = molette + menu.
// Repli (patch < v8, sans tranches) : ancienne heuristique phys_hint / nombre de crans.
static const size_t FXC_SLOT_MIN = 6;
static inline bool fxc_is_slotted(const wc::Channel* ch){
    if(!ch) return false;
    if(!ch->ranges.empty()) return fxc_step_ranges(ch) > 0 && !ch->slots.empty();
    if(fxc_show_physical(ch))                return false;   // (legacy) plage physique reelle = molette
    if(ch->phys_hint == wc::PHYS_CONTINUOUS) return false;
    if(ch->phys_hint == wc::PHYS_MODE)       return ch->slots.size() >= 2;
    return ch->slots.size() >= FXC_SLOT_MIN;
}

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

// Va DIRECTEMENT au slot d'index <idx> (clic sur un bouton de mode) sur chaque device selectionne.
static void fxc_set_slot(const std::string& key, int idx)
{
    for(size_t f=0; f<wc_patch.size(); f++){
        if(wc_patch[f].channels.empty()) continue;
        int circ=(int)wc_patch[f].channels[0].circuit;
        if(!fxc_circuit_selected_device(circ)) continue;
        for(size_t c=0;c<wc_patch[f].channels.size();c++){
            const wc::Channel& ch=wc_patch[f].channels[c];
            if(ch.attribute==wc::ATTR_DIMMER) continue;
            if(fxc_chan_key(ch)!=key) continue;
            if(!ch.slots.empty()){
                int o=(int)ch.coarse_addr;
                if(o>0 && o<514){
                    int i2=idx; if(i2<0)i2=0; if(i2>=(int)ch.slots.size())i2=(int)ch.slots.size()-1;
                    output_devval[o]=ch.slots[i2].from16;
                }
            }
            break;
        }
    }
}

// Affichage "boutons empiles" (tous les modes visibles) plutot que molette+bouton :
// reserve aux canaux a PEU de modes (les grosses roues -> menu deroulant, chantier suivant).
static const size_t FXC_BTNLIST_MAX = 6;
static inline bool fxc_is_buttonlist(const wc::Channel* ch){
    // boutons-SEULS uniquement pour un canal PUR step (aucune tranche proportionnelle) et peu de modes.
    // Un hybride (au moins une tranche proportionnelle) garde toujours sa molette -> molette + menu.
    if(!fxc_is_slotted(ch)) return false;
    if(!ch->ranges.empty()){ for(size_t i=0;i<ch->ranges.size();++i) if(ch->ranges[i].proportional) return false; }
    return ch->slots.size() <= FXC_BTNLIST_MAX;
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
static std::string g_fxc_drop_key;    // [devices] canal dont le menu de modes (dropdown) est ouvert ("" = aucun)
static int         g_fxc_drop_scroll = 0;   // 1er mode visible dans le menu
static int         g_fxc_drop_saved_h = 0;  // hauteur fenetre avant agrandissement auto (0 = pas agrandie)
static const int   FXC_DROP_MAXVIS = 14;    // nb max de modes visibles (la fenetre s'agrandit pour les loger)
static void fxc_close_dropdown();           // ferme le menu + restaure la hauteur (defini plus bas)

static void fxc_layout(const std::string* disp, int n, int* cx_rel, int& content_w)
{
    int prev_cat=-1, x=0;
    for(int i=0;i<n;i++){
        int cat = fxc_category(disp[i]);
        if(i>0) x += (cat!=prev_cat) ? (FXC_ENC_DX + FXC_GAP_CAT) : (FXC_ENC_DX - FXC_GAP_SAME);
        cx_rel[i]=x; prev_cat=cat;
    }
    // On reserve FXC_ENC_DX (largeur de l'ETIQUETTE, 68px) et non FXC_ROL_W (rouleau, 30px) : sinon
    // l'etiquette du dernier circuit debordait du clip au defilement max. Laisse ~16px d'air a droite.
    content_w = (n>0) ? (x + FXC_ENC_DX) : 0;
}
static inline int fxc_view_left (int xf){ return xf + FXC_ENC_X0 - 18; }   // -18 : loge le bouton home (32 de large) sans rognage
static inline int fxc_view_right(int xf){ return xf + fixturectl_window_w - 6; }   // -6 (et non -12) : le dernier rouleau debordait de ~3px du clip
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

static const char* fxc_unit_suffix(uint8_t u)
{
    switch(u){
        case wc::PU_ANGLE:        return "\xC2\xB0";              // degre
        case wc::PU_ANGULARSPEED: return "\xC2\xB0/s";
        case wc::PU_FREQUENCY:    return "Hz";
        case wc::PU_TIME:         return "s";
        case wc::PU_LENGTH:       return "m";
        case wc::PU_TEMPERATURE:  return "K";
        case wc::PU_SPEED:        return "m/s";
        default:                  return "";
    }
}
// Texte de la valeur d'un rouleau : physique (avec unite) si "il y a lieu", sinon dmx_view (%/DMX).
static std::string fxc_value_text(const std::string& key, int refval, bool mixed)
{
    if(mixed) return "\xE2\x80\xA6";
    const wc::Channel* ch = fxc_ref_channel(key);
    if(fxc_show_physical(ch)){
        float phys = ch->phys_from + (refval/65535.0f)*(ch->phys_to - ch->phys_from);
        float av = phys<0?-phys:phys;
        char buf[32];
        snprintf(buf,sizeof buf, av>=100.0f?"%.0f%s":"%.1f%s", phys, fxc_unit_suffix(ch->phys_unit));
        return buf;
    }
    int d8 = (int)(refval >> 8);                                  // level : octet fort
    return ol::ToString(dmx_view ? d8 : (int)(d8/2.55));          // dmx_view : 1 = DMX 0..255, 0 = % 0..100
}

// Dessine un rouleau vertical (thumbwheel) : corps + crans defilants + repere + textes (centres).
// Convention "fonction en haut" (coherente avec les rouleaux simples) :
//   HAUT = nom d'attribut (fonction) pour TOUS les rouleaux.
//   BAS  = valeur DMX (continu) OU declencheur du menu de modes " nom v " (slotted).
// slotted (canal a modes) = facon EOS : la molette reste (plage continue) ET un declencheur de menu
// en bas. mixed = plusieurs devices aux valeurs differentes -> "..." au lieu du nombre/mode.
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

    // HAUT : libelle d'attribut (fonction) -- IDENTIQUE pour les deux types (convention "fonction en haut")
    {
        std::string lab; fxc_fit_label(key, lab);
        petitchiffre.Print(lab.c_str(), cx - petitchiffre.TextWidth(lab.c_str())/2, top - 6);
    }

    // BAS : valeur DMX (continu) ou  DECLENCHEUR DU MENU DE MODES  " nom v "  (grosses roues)
    int by = top + h + 16;
    if(slotted){
        int bw = FXC_ENC_DX, bh = 18;                 // declencheur du menu deroulant sous la molette
        Rect Btn(Vec2D(cx - bw/2, by - 13), Vec2D(bw, bh));
        Btn.SetRoundness(4);
        Btn.SetLineWidth(epaisseur_ligne_fader);
        Btn.Draw(CouleurGrisMoyen);
        Btn.DrawOutline(hovered ? Rgba(1,1,1) : CouleurGrisClair);
        petitchiffre.Print("v", cx + FXC_ENC_DX/2 - 9, by);   // indicateur "deroulant"
        std::string sn = mixed ? std::string("\xE2\x80\xA6") : slotName;
        int maxw = FXC_ENC_DX - 20;
        if(petitchiffre.TextWidth(sn.c_str())>maxw){
            const char* ell="\xE2\x80\xA6";
            while(!sn.empty()){ sn.pop_back(); std::string c=sn+ell; if(petitchiffre.TextWidth(c.c_str())<=maxw){ sn=c; break; } }
        }
        petitchiffre.Print(sn.c_str(), cx - 6 - petitchiffre.TextWidth(sn.c_str())/2, by);
    } else {
        std::string vs = fxc_value_text(key, refval, mixed);   // physique (unite) si pertinent, sinon %/DMX
        petitchiffre.Print(vs.c_str(), cx - petitchiffre.TextWidth(vs.c_str())/2, by);
    }
}

// Rendu "boutons de modes empiles" (tous les modes visibles) : occupe la colonne de la molette.
// Chaque mode = un bouton ; l'actif surligne. Reserve aux canaux a PEU de modes (fxc_is_buttonlist).
static void fxc_draw_buttonlist(int cx, int cy, const std::string& key, const wc::Channel* ch,
                                int activeIdx, bool mixed, bool hovered)
{
    int nb = (int)ch->slots.size();
    if(nb<1) return;
    int w = FXC_ENC_DX - 6, left = cx - w/2;
    int top = cy - FXC_ROL_H/2;
    int bh = FXC_ROL_H / nb;

    std::string lab; fxc_fit_label(key, lab);   // libelle d'attribut en haut (aligne avec les molettes)
    petitchiffre.Print(lab.c_str(), cx - petitchiffre.TextWidth(lab.c_str())/2, top - 6);

    for(int i=0;i<nb;i++){
        int y0 = top + i*bh;
        Rect Btn(Vec2D(left, y0+1), Vec2D(w, bh-2));
        Btn.SetRoundness(3);
        Btn.SetLineWidth(epaisseur_ligne_fader);
        bool act = (!mixed && i==activeIdx);
        Btn.Draw(act ? CouleurFader : CouleurGrisAnthracite);       // mode actif surligne
        Btn.DrawOutline(hovered ? Rgba(1,1,1) : CouleurGrisClair);
        std::string sn = ch->slots[i].name;
        int maxw = w - 8;
        if(petitchiffre.TextWidth(sn.c_str())>maxw){
            const char* ell="\xE2\x80\xA6";
            while(!sn.empty()){ sn.pop_back(); std::string c=sn+ell; if(petitchiffre.TextWidth(c.c_str())<=maxw){ sn=c; break; } }
        }
        petitchiffre.Print(sn.c_str(), cx - petitchiffre.TextWidth(sn.c_str())/2, y0 + bh/2 + 4);
    }
}

// Geometrie du menu de modes ouvert (partagee dessin/clic). false si aucun / introuvable.
static bool fxc_dropdown_geom(int xf, int yf, const wc::Channel*& ch,
                              int& left, int& top, int& w, int& lw, int& row_h,
                              int& vis, int& nb, bool& has_sc, int& maxsc, int& active)
{
    if(g_fxc_drop_key.empty()) return false;
    ch = fxc_ref_channel(g_fxc_drop_key);
    if(!ch || ch->slots.empty()) return false;
    std::string disp[FXC_MAXENC]; int n=fxc_build_display(disp,FXC_MAXENC);
    int cx_rel[FXC_MAXENC], content_w; fxc_layout(disp,n,cx_rel,content_w); fxc_clamp_scroll(content_w,xf);
    int idxCol=-1; for(int i=0;i<n;i++){ if(disp[i]==g_fxc_drop_key){ idxCol=i; break; } }
    if(idxCol<0) return false;
    int scx=fxc_scx(xf,idxCol,cx_rel), cy=yf+FXC_ENC_CY;
    nb=(int)ch->slots.size();
    row_h=16; const int max_vis=FXC_DROP_MAXVIS; w=130;
    left = scx - w/2;                          // borne FENETRE (la fenetre s'est agrandie pour loger le menu)
    if(left < xf+4) left=xf+4;
    if(left+w > xf+fixturectl_window_w-4) left=xf+fixturectl_window_w-4-w;

    vis = nb<max_vis ? nb : max_vis;
    int winBot = yf + fixturectl_window_h - 6;
    int winTop = yf + FXC_ENC_CY - FXC_ROL_H/2 - 28;   // sous les en-tetes de categorie
    int below  = cy + FXC_ROL_H/2 + 24;        // ancrage sous le declencheur
    if(below + vis*row_h + 4 <= winBot){
        top = below;
    } else {                                    // pas la place en bas : au-dessus, sinon clampe
        int aboveBot = cy + FXC_ROL_H/2 - 2;
        int fitUp = (aboveBot - winTop - 4)/row_h;
        int fitDn = (winBot - below - 4)/row_h;
        if(fitUp >= fitDn){ if(vis>fitUp)vis=fitUp; if(vis<1)vis=1; top = aboveBot - (vis*row_h+4); }
        else             { if(vis>fitDn)vis=fitDn; if(vis<1)vis=1; top = below; }
    }
    has_sc = nb > vis;
    maxsc = nb - vis; if(maxsc<0) maxsc=0;
    if(g_fxc_drop_scroll>maxsc)g_fxc_drop_scroll=maxsc; if(g_fxc_drop_scroll<0)g_fxc_drop_scroll=0;
    lw = has_sc ? w-16 : w-4;
    int o=(ch->coarse_addr>0 && ch->coarse_addr<514) ? (int)output_devval[ch->coarse_addr] : 0;
    active = fxc_slot_index(ch, o);
    return true;
}

// Dessine le menu de modes ouvert (appele en DERNIER, par-dessus les colonnes).
static void fxc_draw_dropdown(int xf, int yf)
{
    const wc::Channel* ch; int left,top,w,lw,row_h,vis,nb,maxsc,active; bool has_sc;
    if(!fxc_dropdown_geom(xf,yf,ch,left,top,w,lw,row_h,vis,nb,has_sc,maxsc,active)){ fxc_close_dropdown(); return; }
    Rect Box(Vec2D(left,top),Vec2D(w,vis*row_h+4)); Box.SetRoundness(4);
    Box.Draw(CouleurFond); Box.DrawOutline(CouleurFader);
    for(int v=0;v<vis;v++){
        int fi=v+g_fxc_drop_scroll; if(fi>=nb) break;
        int ry=top+2+v*row_h;
        bool hov=(window_focus_id==W_FIXTURECTL && mouse_x>left+2 && mouse_x<left+2+lw && mouse_y>ry && mouse_y<ry+row_h);
        Rect Row(Vec2D(left+2,ry),Vec2D(lw,row_h)); Row.SetRoundness(2);
        if(fi==active) Row.Draw(CouleurFader.WithAlpha(0.5f));
        else if(hov)   Row.Draw(CouleurGrisMoyen.WithAlpha(0.5f));
        Canvas::SetClipping(left+4,ry,lw-4,row_h);
        petitpetitchiffre.Print(ch->slots[fi].name.c_str(), left+6, ry+11);
        Canvas::DisableClipping();
    }
    if(has_sc){
        int bx=left+w-14, boxH=vis*row_h;
        Rect Up(Vec2D(bx,top+2),Vec2D(12,12)); Up.SetRoundness(2);
        Up.Draw(g_fxc_drop_scroll>0 ? CouleurGrisAnthracite : CouleurGrisMoyen.WithAlpha(0.3f));
        petitpetitchiffre.Print("^", bx+4, top+11);
        int dyb=top+boxH-10;
        Rect Dn(Vec2D(bx,dyb),Vec2D(12,12)); Dn.SetRoundness(2);
        Dn.Draw(g_fxc_drop_scroll<maxsc ? CouleurGrisAnthracite : CouleurGrisMoyen.WithAlpha(0.3f));
        petitpetitchiffre.Print("v", bx+4, dyb+9);
        // piste + pouce (ascenseur)
        int trTop=top+16, trH=boxH-32;
        if(trH>6){
            Rect Track(Vec2D(bx+3,trTop),Vec2D(6,trH)); Track.SetRoundness(2); Track.Draw(CouleurGrisAnthracite.WithAlpha(0.5f));
            int thumbH = trH*vis/nb; if(thumbH<8)thumbH=8; if(thumbH>trH)thumbH=trH;
            int thumbY = trTop + (maxsc>0 ? g_fxc_drop_scroll*(trH-thumbH)/maxsc : 0);
            Rect Thumb(Vec2D(bx+3,thumbY),Vec2D(6,thumbH)); Thumb.SetRoundness(2); Thumb.Draw(CouleurGrisMoyen);
        }
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
    std::string hov_full; int hov_scx=0; bool hov_trunc=false, hov_top=false;   // label/slot survole (info-bulle) ; hov_top = libelle du haut
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
            bool btnlist = fxc_is_buttonlist(refCh);
            std::string slotName;
            if(slotted){ int si=fxc_slot_index(refCh, val); if(si>=0) slotName=refCh->slots[si].name; }
            if(btnlist) fxc_draw_buttonlist(scx, cy, disp[i], refCh, fxc_slot_index(refCh, val), mixed, hov);
            else        fxc_draw_encoder(scx, cy, disp[i], val, mixed, hov, slotted, slotName);
            if(hov){ strncpy(fixturectl_wheel_hover, disp[i].c_str(), 23); fixturectl_wheel_hover[23]=0; }
            // survol du LIBELLE D'ATTRIBUT (au-DESSUS du rouleau, "fonction en haut") -> info-bulle du nom complet
            // (les deux types : molette et canaux a modes -- ce sont ces libelles qui sont tronques a l'ecran)
            if(window_focus_id==W_FIXTURECTL && mouse_x>scx-FXC_ENC_DX/2 && mouse_x<scx+FXC_ENC_DX/2
               && mouse_y>cy-FXC_ROL_H/2-16 && mouse_y<=cy-FXC_ROL_H/2){
                std::string tmp; hov_trunc=fxc_fit_label(disp[i],tmp); hov_full=fxc_label(disp[i]); hov_scx=scx; hov_top=true;
            }
            // survol du DECLENCHEUR de mode (SOUS le rouleau) -> info-bulle du nom de mode complet
            if(slotted && window_focus_id==W_FIXTURECTL && mouse_x>scx-FXC_ENC_DX/2 && mouse_x<scx+FXC_ENC_DX/2
               && mouse_y>cy+FXC_ROL_H/2+6 && mouse_y<cy+FXC_ROL_H/2+22){
                hov_full = mixed?std::string():slotName; hov_trunc = petitchiffre.TextWidth(hov_full.c_str())>FXC_ENC_DX-24; hov_scx=scx; hov_top=false;
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

    // info-bulle FIXE : texte complet survole s'il est tronque. Ancree sur le rouleau, AU-DESSUS pour un
    // libelle du haut (s'il y a la place), sinon SOUS le bouton home.
    if(!hov_full.empty() && hov_trunc){
        int tw = petitchiffre.TextWidth(hov_full.c_str())+10;
        int tx = hov_scx - tw/2;
        if(tx < xf+4) tx = xf+4;
        if(tx+tw > xf+fixturectl_window_w-4) tx = xf+fixturectl_window_w-4-tw;
        int ty = cy + FXC_ROL_H/2 + 52;                          // defaut : sous le bouton home
        if(hov_top && cy - FXC_ROL_H/2 - 41 >= yf + 18) ty = cy - FXC_ROL_H/2 - 30;   // libelle du haut : au-dessus
        Rect Tip(Vec2D(tx, ty-11), Vec2D(tw,14)); Tip.SetRoundness(3);
        Tip.Draw(CouleurGrisAnthracite); Tip.DrawOutline(CouleurFader);
        petitchiffre.Print(hov_full.c_str(), tx+5, ty);
    }

    // poignee de redimensionnement (coin bas-droit) : 3 petits traits diagonaux
    {
        int gx = xf+fixturectl_window_w, gy = yf+fixturectl_window_h;
        for(int i=4;i<=12;i+=4) Line(Vec2D(gx-i-2, gy-4), Vec2D(gx-4, gy-i-2)).Draw(CouleurGrisClair);
    }

    fxc_draw_dropdown(xf, yf);   // menu de modes ouvert : par-dessus les colonnes
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
    int by = cy + FXC_ROL_H/2 + 16;   // ligne  < nom_du_mode >  sous le rouleau (cf. fxc_draw_encoder)
    if(py<by-10 || py>by+4) return std::string();
    for(int i=0;i<n;i++){
        int scx=fxc_scx(xf,i,cx_rel);
        if(scx+FXC_ROL_W/2<VL || scx-FXC_ROL_W/2>VR) continue;
        const wc::Channel* ch=fxc_ref_channel(disp[i]);
        if(!fxc_is_slotted(ch) || fxc_is_buttonlist(ch)) continue;   // button-list = boutons empiles, pas de fleches
        if(px>scx-FXC_ENC_DX/2 && px<scx-FXC_ENC_DX/2+14){ dir=-1; return disp[i]; }
        if(px>scx+FXC_ENC_DX/2-14 && px<scx+FXC_ENC_DX/2){ dir=+1; return disp[i]; }
    }
    return std::string();
}

// Bouton de mode empile sous (px,py) : renvoie la clef du canal + l'index du mode clique, ou "".
static std::string fxc_modebtn_at_point(int xf, int yf, int px, int py, int& outIdx)
{
    outIdx=-1;
    std::string disp[FXC_MAXENC]; int n=fxc_build_display(disp,FXC_MAXENC);
    int cx_rel[FXC_MAXENC], content_w; fxc_layout(disp,n,cx_rel,content_w); fxc_clamp_scroll(content_w,xf);
    int cy=yf+FXC_ENC_CY, VL=fxc_view_left(xf), VR=fxc_view_right(xf);
    int top = cy - FXC_ROL_H/2;
    if(py<top || py>top+FXC_ROL_H) return std::string();
    for(int i=0;i<n;i++){
        int scx=fxc_scx(xf,i,cx_rel);
        if(scx+FXC_ROL_W/2<VL || scx-FXC_ROL_W/2>VR) continue;
        const wc::Channel* ch=fxc_ref_channel(disp[i]);
        if(!fxc_is_buttonlist(ch)) continue;
        int w=FXC_ENC_DX-6, left=scx-w/2;
        if(px<left || px>left+w) continue;
        int nb=(int)ch->slots.size(); if(nb<1) continue;
        int bh=FXC_ROL_H/nb;
        int idx=(py-top)/bh; if(idx<0)idx=0; if(idx>=nb)idx=nb-1;
        outIdx=idx; return disp[i];
    }
    return std::string();
}

// Declencheur du menu de modes ("nom v") sous (px,py) pour une grosse roue VISIBLE : renvoie la clef, ou "".
static std::string fxc_trigger_at_point(int xf, int yf, int px, int py)
{
    std::string disp[FXC_MAXENC]; int n=fxc_build_display(disp,FXC_MAXENC);
    int cx_rel[FXC_MAXENC], content_w; fxc_layout(disp,n,cx_rel,content_w); fxc_clamp_scroll(content_w,xf);
    int cy=yf+FXC_ENC_CY, VL=fxc_view_left(xf), VR=fxc_view_right(xf);
    int by = cy + FXC_ROL_H/2 + 16;
    if(py<by-13 || py>by+5) return std::string();
    for(int i=0;i<n;i++){
        int scx=fxc_scx(xf,i,cx_rel);
        if(scx+FXC_ROL_W/2<VL || scx-FXC_ROL_W/2>VR) continue;
        const wc::Channel* ch=fxc_ref_channel(disp[i]);
        if(!fxc_is_slotted(ch) || fxc_is_buttonlist(ch)) continue;   // seules les grosses roues ont un menu
        if(px>scx-FXC_ENC_DX/2 && px<scx+FXC_ENC_DX/2) return disp[i];
    }
    return std::string();
}

// Ouvre le menu de modes pour <key> et AGRANDIT la fenetre (si besoin) pour loger la liste :
// le menu reste ainsi DANS la fenetre -> les clics sont traites normalement (pas de bagarre de focus).
static void fxc_open_dropdown(const std::string& key, int yf)
{
    g_fxc_drop_key = key; g_fxc_drop_scroll = 0;
    int needH = FXC_ENC_CY + FXC_ROL_H/2 + 24 + FXC_DROP_MAXVIS*16 + 10;
    int maxH  = SCREEN_H - 8 - yf; if(needH > maxH) needH = maxH;
    if(fixturectl_window_h < needH){
        if(g_fxc_drop_saved_h==0) g_fxc_drop_saved_h = fixturectl_window_h;   // memorise la taille d'origine
        fixturectl_window_h = needH;
    }
}
// Ferme le menu et RESTAURE la hauteur de la fenetre.
static void fxc_close_dropdown()
{
    g_fxc_drop_key.clear();
    if(g_fxc_drop_saved_h>0){ fixturectl_window_h = g_fxc_drop_saved_h; g_fxc_drop_saved_h = 0; }
}

// Clic quand le menu de modes est ouvert : ligne -> selection+ferme ; fleches -> defilement.
// Renvoie 1 si le clic est DANS le cadre (consomme), 0 sinon (laisse la fermeture aux autres handlers).
static int fxc_dropdown_click(int xf, int yf, int px, int py)
{
    const wc::Channel* ch; int left,top,w,lw,row_h,vis,nb,maxsc,active; bool has_sc;
    if(!fxc_dropdown_geom(xf,yf,ch,left,top,w,lw,row_h,vis,nb,has_sc,maxsc,active)) return 0;
    if(has_sc){
        int bx=left+w-14;
        if(px>bx && px<bx+12 && py>top+2 && py<top+14){ if(g_fxc_drop_scroll>0)g_fxc_drop_scroll--; return 1; }
        int dyb=top+vis*row_h-10;
        if(px>bx && px<bx+12 && py>dyb && py<dyb+12){ if(g_fxc_drop_scroll<maxsc)g_fxc_drop_scroll++; return 1; }
    }
    for(int v=0;v<vis;v++){
        int fi=v+g_fxc_drop_scroll; if(fi>=nb) break;
        int ry=top+2+v*row_h;
        if(px>left+2 && px<left+2+lw && py>ry && py<ry+row_h){ fxc_set_slot(g_fxc_drop_key, fi); fxc_close_dropdown(); return 1; }
    }
    if(px>=left && px<=left+w && py>=top && py<=top+vis*row_h+4) return 1;   // marge du cadre : consomme, reste ouvert
    fxc_close_dropdown();   // clic hors du menu -> fermer
    return 1;
}

// Ascenseur du menu : true si (px,py) est dans la PISTE. Si setScroll, positionne g_fxc_drop_scroll d'apres py.
static bool fxc_dropdown_scrollbar(int xf, int yf, int px, int py, bool setScroll)
{
    const wc::Channel* ch; int left,top,w,lw,row_h,vis,nb,maxsc,active; bool has_sc;
    if(!fxc_dropdown_geom(xf,yf,ch,left,top,w,lw,row_h,vis,nb,has_sc,maxsc,active)) return false;
    if(!has_sc) return false;
    int bx=left+w-14, boxH=vis*row_h, trTop=top+16, trH=boxH-32;
    if(trH<=6) return false;
    if(px<bx || px>bx+12 || py<trTop || py>trTop+trH) return false;   // hors piste (les fleches sont gerees a part)
    if(setScroll){
        int thumbH=trH*vis/nb; if(thumbH<8)thumbH=8; if(thumbH>trH)thumbH=trH;
        int sc = (trH-thumbH>0) ? (py - trTop - thumbH/2)*maxsc/(trH-thumbH) : 0;
        if(sc<0)sc=0; if(sc>maxsc)sc=maxsc;
        g_fxc_drop_scroll=sc;
    }
    return true;
}

// Etat du menu de modes (utilise par MAIN pour verrouiller le focus, et channels_core pour la molette).
bool fxc_dropdown_open(){ return !g_fxc_drop_key.empty(); }
bool fxc_dropdown_wheel(int delta){   // delta>0 = molette vers le haut ; renvoie true si consomme
    if(g_fxc_drop_key.empty()) return false;
    g_fxc_drop_scroll -= delta; if(g_fxc_drop_scroll<0) g_fxc_drop_scroll=0;   // borne haute clampee par fxc_dropdown_geom
    return true;
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

    // menu de modes ouvert. On teste CHAQUE FRAME si le CLIC D'ORIGINE (mouse_click, fige a l'appui et
    // stable pendant tout le drag) etait sur la PISTE de l'ascenseur -> drag du pouce ; sinon clic normal
    // (ligne / fleche / dehors). Pas de drapeau persistant (qui restait colle a true apres un drag).
    if(!g_fxc_drop_key.empty()){
        if(mouse_button==1 && fxc_dropdown_scrollbar(xf, yf, mouse_click_x, mouse_click_y, false)){
            fxc_dropdown_scrollbar(xf, yf, mouse_x, mouse_y, true);   // suit la souris (pas de mouse_released : rappel chaque frame)
            return(0);
        }
        if(fxc_dropdown_click(xf, yf, mouse_x, mouse_y)){ mouse_released=1; return(0); }
    }
    // filtres de categorie : clic SIMPLE fiable -> on consomme le clic (mouse_released=1) pour ne pas
    // dependre du deplacement de la souris (sinon re-cliquer au meme pixel ne re-declenchait pas).
    {
        int bc = fxc_button_at_point(xf, yf, mouse_x, mouse_y);
        if(bc>=0){ g_fxc_cat_hidden[bc] = !g_fxc_cat_hidden[bc]; mouse_released=1; return(0); }
    }
    // declencheur du menu de modes (grosses roues) : clic -> ouvrir / fermer (consomme)
    {
        std::string tk = fxc_trigger_at_point(xf, yf, mouse_x, mouse_y);
        if(!tk.empty()){
            if(g_fxc_drop_key==tk) fxc_close_dropdown();
            else                   fxc_open_dropdown(tk, yf);
            mouse_released=1; return(0);
        }
    }
    // boutons de modes empiles : clic simple -> aller directement au mode (consomme)
    {
        int si=-1; std::string bk = fxc_modebtn_at_point(xf, yf, mouse_x, mouse_y, si);
        if(!bk.empty()){ fxc_set_slot(bk, si); mouse_released=1; return(0); }
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
