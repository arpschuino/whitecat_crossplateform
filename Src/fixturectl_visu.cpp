/*-------------------------------------------------------------------------------------------------------------
 White Cat - Control Fixtures window (devices)
 Copyright (C) 2009-2016 Christoph Guillermet - Maintenance 2026 Jacques Bouault (arpschuino.fr)
 GNU General Public License v2 or later. See <http://www.gnu.org/licenses/>.
---------------------------------------------------------------------------------------------------------------*/

/**
* \file fixturectl_visu.cpp
* \brief Fenetre "Control Fixtures" : editeur d'attributs des devices (Intensite + Pan/Tilt/RGBW/Zoom/Shutter).
*
* Tranche 3 = faders d'attributs, style master (course 255, gris). Le circuit selectionne
* (last_ch_selected) designe le device. Fader 0 = INTENSITE (ecrit bufferSaisie[circuit], comme un
* circuit normal) ; faders suivants = attributs NON-Dimmer -> ecrivent output_devval[output] (16 bit).
* Pilotables au drag ET a la molette (Ctrl+molette = pas fin 1/65535), via la cible fixturectl_wheel_hover
* publiee ici et consommee dans DoMouseLevel (channels_core.cpp). Palettes (T4) et cues (T5) a venir.
 **/

#include "wc_tus.h"
#include "gui_boutons_rebuild1.h"

// --- Geometrie des faders (partagee rendu <-> logique). Modele = grand master (40 x 255, roundness 15). ---
static const int FXC_FAD_X0 = 30;    // x du 1er fader (intensite), relatif a xf
static const int FXC_FAD_Y  = 80;    // haut de la course, relatif a yf
static const int FXC_FAD_W  = 40;    // largeur d'un fader
static const int FXC_FAD_H  = 255;   // hauteur de la course (comme les masters)
static const int FXC_FAD_DX = 70;    // pas horizontal entre faders
static const int FXC_MAXATT = 12;    // nb max d'attributs non-Dimmer

static inline int fxc_slot_x(int xf, int slot){ return xf + FXC_FAD_X0 + slot*FXC_FAD_DX; }

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

// Rassemble les channels NON-Dimmer du device dont un channel a circuit == <circuit>.
// Remplit coarse[]/attr[] dans l'ordre du patch, retourne le nombre (<= maxn).
// Si fixname != NULL, y depose le nom de la 1re fixture trouvee.
static int fxc_collect(int circuit, int* coarse, uint8_t* attr, int maxn, const char** fixname)
{
    int n=0;
    if(circuit<=0) return 0;
    for(size_t f=0; f<wc_patch.size() && n<maxn; f++)
    {
        bool belongs=false;
        for(size_t c=0;c<wc_patch[f].channels.size();c++)
            if((int)wc_patch[f].channels[c].circuit==circuit){ belongs=true; break; }
        if(!belongs) continue;
        if(fixname && *fixname==NULL) *fixname = wc_patch[f].name.c_str();
        for(size_t c=0; c<wc_patch[f].channels.size() && n<maxn; c++)
        {
            const wc::Channel& ch = wc_patch[f].channels[c];
            if(ch.attribute==wc::ATTR_DIMMER || ch.attribute==wc::ATTR_NONE) continue; // intensite = circuit
            if(ch.coarse_addr==0) continue;
            coarse[n]=(int)ch.coarse_addr;
            attr[n]=ch.attribute;
            n++;
        }
    }
    return n;
}

// Dessine un fader style master : course 40x255, remplissage gris, valeur DMX au-dessus, libelle en dessous.
static void fxc_draw_fader(int fx, int fy, unsigned short lvl, const char* label)
{
    int pix = (int)((long)lvl * FXC_FAD_H / 65535);   // hauteur remplie (0..255)
    Rect Box(Vec2D(fx, fy), Vec2D(FXC_FAD_W, FXC_FAD_H));
    Box.SetRoundness(15);
    Box.SetLineWidth(epaisseur_ligne_fader);
    Rect Niv(Vec2D(fx, (fy + FXC_FAD_H) - pix), Vec2D(FXC_FAD_W, pix));
    Niv.SetRoundness(15);
    Niv.Draw(CouleurGrisMoyen);            // gris (style master, distinct du bleu blind/fader)
    Box.DrawOutline(CouleurLigne);
    petitchiffre.Print(ol::ToString((int)(lvl >> 8)), fx + 8, fy - 5);       // valeur DMX 8 bit au-dessus
    petitchiffre.Print(label, fx + 6, fy + FXC_FAD_H + 15);                  // libelle en dessous
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

    fixturectl_wheel_hover = -1;   // cible molette republiee a chaque frame (defaut : aucun fader survole)

    int coarse[FXC_MAXATT]; uint8_t attr[FXC_MAXATT];
    const char* fixname = NULL;
    int n = fxc_collect(last_ch_selected, coarse, attr, FXC_MAXATT, &fixname);

    // en-tete : circuit selectionne + device
    char hdr[96];
    if(n>0) sprintf(hdr, "Circuit %d  -  %s", last_ch_selected, fixname ? fixname : "");
    else    sprintf(hdr, "Circuit %d : selectionne un device (Mac Aura...)", last_ch_selected);
    petitchiffre.Print(hdr, xf + 30, yf + 52);

    if(n<=0) return(0);   // pas un device : rien a editer

    int fy = yf + FXC_FAD_Y;

    // fader 0 = INTENSITE (pilote le circuit via bufferSaisie ; l'intensite reelle sort par MergerArray)
    {
        int fx = fxc_slot_x(xf, 0);
        unsigned short lvl = (last_ch_selected>0 && last_ch_selected<514) ? bufferSaisie[last_ch_selected] : 0;
        fxc_draw_fader(fx, fy, lvl, "Int");
        if(window_focus_id==W_FIXTURECTL &&
           mouse_x>fx && mouse_x<fx+FXC_FAD_W && mouse_y>fy-2 && mouse_y<fy+FXC_FAD_H+2)
            fixturectl_wheel_hover = 0;   // 0 = intensite
    }

    // faders des attributs non-Dimmer
    for(int i=0;i<n;i++)
    {
        int fx = fxc_slot_x(xf, i+1);
        unsigned short lvl = output_devval[coarse[i]];
        fxc_draw_fader(fx, fy, lvl, fxc_attr_label(attr[i]));
        if(window_focus_id==W_FIXTURECTL &&
           mouse_x>fx && mouse_x<fx+FXC_FAD_W && mouse_y>fy-2 && mouse_y<fy+FXC_FAD_H+2)
            fixturectl_wheel_hover = coarse[i];   // >=1 = output d'attribut
    }

    return(0);
}

// Logique (drag des faders). Appelee uniquement bouton maintenu (check_graphics_mouse_handling).
int do_logical_fixturectl(int xf, int yf)
{
    int coarse[FXC_MAXATT]; uint8_t attr[FXC_MAXATT];
    const char* dummy = NULL;
    int n = fxc_collect(last_ch_selected, coarse, attr, FXC_MAXATT, &dummy);
    if(n<=0) return(0);

    int fy = yf + FXC_FAD_Y;

    // helper local : le drag "prend" le fader si le clic a DEMARRE dedans (colonne + course verticale)
    // slot 0 = intensite -> bufferSaisie[circuit] ; slots 1..n = attributs -> output_devval[output].
    for(int slot=0; slot<=n; slot++)
    {
        int fx = fxc_slot_x(xf, slot);
        if(mouse_click_x > fx && mouse_click_x < fx + FXC_FAD_W &&
           mouse_click_y > fy - 2 && mouse_click_y < fy + FXC_FAD_H + 2)
        {
            set_mouse_range(fx, fy, fx + FXC_FAD_W, fy + FXC_FAD_H);   // borne mouse_y a la course
            int lvl = (int)((long)(fy + FXC_FAD_H - mouse_y) * 65535 / FXC_FAD_H);
            if(lvl < 0)     lvl = 0;
            if(lvl > 65535) lvl = 65535;
            if(slot==0){ if(last_ch_selected>0 && last_ch_selected<514) bufferSaisie[last_ch_selected]=(unsigned short)lvl; }
            else       { output_devval[coarse[slot-1]] = (unsigned short)lvl; }
        }
    }
    return(0);
}
