/*-------------------------------------------------------------------------------------------------------------
                                 |
          CWWWWWWWW              | Copyright (C) 2009-2013  Christoph Guillermet
       WWWWWWWWWWWWWWW           |
     WWWWWWWWWWWWWWWWWWW         | This file is part of White Cat.
    WWWWWWWWWWWWWWWWWCWWWW       |
   WWWWWWWWWWWWWWWWW tWWWWW      | White Cat is free software: you can redistribute it and/or modify
  WWWW   WWWWWWWWWW  tWWWWWW     | it under the terms of the GNU General Public License as published by
 WWWWWt              tWWWWWWa    | the Free Software Foundation, either version 2 of the License, or
 WWWWWW               WWWWWWW    | (at your option) any later version.
WWWWWWWW              WWWWWWW    |
WWWWWWWW               WWWWWWW   | White Cat is distributed in the hope that it will be useful,
WWWWWWW               WWWWWWWW   | but WITHOUT ANY WARRANTY; without even the implied warranty of
WWWWWWW      CWWW    W WWWWWWW   | MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
WWWWWWW            aW  WWWWWWW   | GNU General Public License for more details.
WWWWWWWW           C  WWWWWWWW   |
 WWWWWWWW            CWWWWWWW    | You should have received a copy of the GNU General Public License
 WWWWWWWWW          WWWWWWWWW    | along with White Cat.  If not, see <http://www.gnu.org/licenses/>.
  WWWWWWWWWWC    CWWWWWWWWWW     |
   WWWWWWWWWWWWWWWWWWWWWWWW      |
    WWWWWWWWWWWWWWWWWWWWWW       |
      WWWWWWWWWWWWWWWWWWa        |
        WWWWWWWWWWWWWWW          |
           WWWWWWWWt             |
                                 |
---------------------------------------------------------------------------------------------------------------*/

/**

* \file patch_core.cpp
* \brief {DMX patch core}
* \author Christoph Guillermet
* \version {0.8.6.3}
* \date {12/02/2015}

 White Cat {- categorie} {- sous categorie {- sous categorie}}

*   Fonction Core pour le patch dmx
*
*   Core fonction for the dmx patch
*
**/

#include "wc_tus.h"
#include "patch_splines.h"
#include "gdtf_import.h"   // [devices] import GDTF -> wc::Fixture
#include "gestionaire_fenetres2.h"   // [devices] add_a_window / substract_a_window (bouton Patch device)

// ============================================================================
// [Fixtures] Pont entre le modele wc_patch (source de verite) et les tableaux
// legacy que le rendu balaie (cohabitation). Voir wc_fixture.h.
// ============================================================================

// Regenere les tableaux plats depuis wc_patch.
int rebuild_patch_from_fixtures()
{
    for(int o=0;o<514;o++)
    {
        Patch[o]=0;
        output_fine[o]=0;
        is_fine[o]=0;
        curves[o]=0;
        dimmer_type[o]=0;
        output_attribute[o]=wc::ATTR_DIMMER;   // [devices] defaut = gradateur (rendu legacy inverse+courbe)
        output_devdefault[o]=0;                // [devices] valeur home, remplie depuis ch.home ci-dessous
    }
    for(size_t f=0; f<wc_patch.size(); f++)
    {
        for(size_t c=0; c<wc_patch[f].channels.size(); c++)
        {
            const wc::Channel& ch = wc_patch[f].channels[c];
            int co = (int)ch.coarse_addr;
            if(co<=0 || co>=514) continue;
            Patch[co]            = (int)ch.circuit;
            curves[co]           = (int)ch.curve;
            dimmer_type[co]      = (ch.combine==wc::COMBINE_LTP) ? 1 : 0;
            output_attribute[co] = ch.attribute;   // [devices] attribut GDTF -> rendu (Dimmer inverse+courbe, autre lineaire)
            output_devdefault[co]= ch.home;        // [devices] valeur home (bouton home), persistee via ch.home
            if(ch.resolution==wc::RES_16BIT && ch.fine_addr!=0 && (int)ch.fine_addr<514)
            {
                output_fine[co]              = (int)ch.fine_addr;
                is_fine[ch.fine_addr]        = 1;
                Patch[ch.fine_addr]          = (int)ch.circuit;   // l'output fine pointe le meme circuit
                curves[ch.fine_addr]         = (int)ch.curve;
                output_attribute[ch.fine_addr] = ch.attribute;
            }
        }
    }
    return 0;
}

// Resynchronise wc_patch avec l'etat courant.
// [devices] wc_patch est la SOURCE DE VERITE pour les DEVICES (fixtures multi-canaux ou portant un
// attribut non-Dimmer) : on les PRESERVE tels quels. Seuls les DIMMERS SIMPLES (1 canal Dimmer) sont
// re-synthetises depuis les tableaux plats (encore edites par l'UI patch), pour les outputs NON deja
// possedes par un device. Ainsi un device patche survit au save/reload (sans quoi il etait aplati).
static inline bool fixture_is_device(const wc::Fixture& fx)
{
    if(fx.channels.size()>1) return true;
    for(size_t c=0;c<fx.channels.size();c++)
        if(fx.channels[c].attribute!=wc::ATTR_DIMMER) return true;
    return false;
}

int synthesize_fixtures_from_legacy()
{
    // 1) extraire les devices existants (a preserver) et marquer les outputs qu'ils occupent
    std::vector<wc::Fixture> devices;
    bool owned[514]; for(int i=0;i<514;i++) owned[i]=false;
    for(size_t f=0; f<wc_patch.size(); f++)
    {
        if(!fixture_is_device(wc_patch[f])) continue;
        devices.push_back(wc_patch[f]);
        for(size_t c=0;c<wc_patch[f].channels.size();c++)
        {
            int co=(int)wc_patch[f].channels[c].coarse_addr;
            int fi=(int)wc_patch[f].channels[c].fine_addr;
            if(co>0 && co<514) owned[co]=true;
            if(fi>0 && fi<514) owned[fi]=true;
        }
    }

    // 2) reconstruire : devices preserves + dimmers simples depuis le legacy (hors outputs des devices)
    wc_patch.clear();
    for(size_t d=0; d<devices.size(); d++) wc_patch.push_back(devices[d]);
    for(int o=1;o<514;o++)
    {
        if(Patch[o]>0 && is_fine[o]==0 && !owned[o])
        {
            wc::Channel ch;
            ch.attribute   = wc::ATTR_DIMMER;
            ch.combine     = dimmer_type[o] ? wc::COMBINE_LTP : wc::COMBINE_HTP;
            ch.resolution  = (output_fine[o]!=0) ? wc::RES_16BIT : wc::RES_8BIT;
            ch.curve       = (uint8_t)curves[o];
            ch.universe    = 0;
            ch.coarse_addr = (uint16_t)o;
            ch.fine_addr   = (uint16_t)output_fine[o];
            ch.circuit     = (uint16_t)Patch[o];
            wc::Fixture fx;
            fx.channels.push_back(ch);
            wc_patch.push_back(fx);
        }
    }
    return 0;
}

// [devices] ECHAFAUDAGE (test) : cree un device RGB (3 channels ColorAdd R/G/B) sur les outputs
// base..base+2, controle par <circuit>, dans le modele wc_patch, puis regenere les tableaux legacy.
// Appele quand on est en mode "patch device" et qu'on clique un output. A remplacer par la vraie
// creation de device (choix du type/personality) plus tard.
int create_rgb_device_at(int base, int circuit)
{
    if(base<1 || base>510) return -1;
    if(circuit<1 || circuit>512) circuit=1;

    // capturer l'etat legacy dans le modele (wc_patch etait peut-etre perime)
    synthesize_fixtures_from_legacy();

    // retirer les fixtures existantes qui occupent base..base+2 (le device les remplace)
    for(int k=0;k<3;k++)
    {
        int out=base+k;
        for(size_t f=0; f<wc_patch.size(); )
        {
            bool hit=false;
            for(size_t c=0;c<wc_patch[f].channels.size();c++)
                if((int)wc_patch[f].channels[c].coarse_addr==out || (int)wc_patch[f].channels[c].fine_addr==out){ hit=true; break; }
            if(hit) wc_patch.erase(wc_patch.begin()+f); else f++;
        }
    }

    // la Fixture RGB (3 channels ColorAdd R/G/B sur base..base+2, LTP 8 bit)
    wc::Fixture fx;
    fx.name = "RGB test";
    wc::AttrId rgb[3] = { wc::ATTR_COLORADD_R, wc::ATTR_COLORADD_G, wc::ATTR_COLORADD_B };
    for(int k=0;k<3;k++)
    {
        wc::Channel ch;
        ch.attribute   = (uint8_t)rgb[k];
        ch.combine     = wc::COMBINE_LTP;
        ch.resolution  = wc::RES_8BIT;
        ch.coarse_addr = (uint16_t)(base+k);
        ch.circuit     = (uint16_t)circuit;
        fx.channels.push_back(ch);
    }
    wc_patch.push_back(fx);

    rebuild_patch_from_fixtures();   // modele -> tableaux legacy (le rendu balaie ceux-ci)
    return 0;
}

// [devices] ECHAFAUDAGE (test) : cree un device en forme de MAC Aura (mode Standard 14 ch) a partir de
// l'adresse <base> (= adresse de patch de la fixture, offset 1). MODELE 1 DEVICE = 1 CIRCUIT :
// TOUS les channels sont rattaches au MEME <circuit>, qui porte l'INTENSITE (Dimmer) et sert de selection.
// Les attributs non-Dimmer ne consomment PAS de circuit : leur valeur vit dans output_devval[output],
// reglee par la fenetre Control Fixtures. Seuls les attributs deja definis sont poses (Control/Color1/CTC differes) :
//   Shutter  addr base+0    (8 bit, LTP, lineaire brut ; ~22 = ouvert)
//   Dimmer   addr base+1    (8 bit, HTP, courbe = rendu legacy)  <- INTENSITE, depuis MergerArray[circuit]
//   Zoom     addr base+2    (8 bit, LTP, lineaire)
//   Pan      addr base+3/+4 (16 bit, LTP, lineaire)
//   Tilt     addr base+5/+6 (16 bit, LTP, lineaire)
//   R/G/B/W  addr base+9/10/11/12 (8 bit, LTP, lineaire)
// Cf. docs/mac_aura_standard.md. A remplacer par l'import d'une personality + UI plus tard.
int create_mac_aura_at(int base, int circuit)
{
    if(base<1 || base+12>513) return -1;             // W = base+12 doit rester dans 1..513
    if(circuit<1)   circuit=1;
    if(circuit>512) circuit=512;                     // circuit unique (indice MergerArray valide)

    synthesize_fixtures_from_legacy();               // capturer l'etat legacy dans le modele

    // retirer les fixtures existantes qui occupent l'empreinte base..base+13
    for(int out=base; out<=base+13 && out<514; out++)
    {
        for(size_t f=0; f<wc_patch.size(); )
        {
            bool hit=false;
            for(size_t c=0;c<wc_patch[f].channels.size();c++)
                if((int)wc_patch[f].channels[c].coarse_addr==out || (int)wc_patch[f].channels[c].fine_addr==out){ hit=true; break; }
            if(hit) wc_patch.erase(wc_patch.begin()+f); else f++;
        }
    }

    wc::Fixture fx;
    fx.name = "MAC Aura (test)";

    // [devices] 1 device = 1 circuit : TOUS les channels sur le meme <circuit>.
    // Shutter (8 bit, LTP, lineaire brut ; ~22 = ouvert sur du vrai materiel) -> output_devval
    { wc::Channel ch; ch.attribute=wc::ATTR_SHUTTER1; ch.combine=wc::COMBINE_LTP; ch.resolution=wc::RES_8BIT;
      ch.coarse_addr=(uint16_t)(base+0);  ch.circuit=(uint16_t)circuit; fx.channels.push_back(ch); }
    // Dimmer (8 bit, HTP, rendu gradateur) -> INTENSITE depuis MergerArray[circuit]
    { wc::Channel ch; ch.attribute=wc::ATTR_DIMMER; ch.combine=wc::COMBINE_HTP; ch.resolution=wc::RES_8BIT;
      ch.coarse_addr=(uint16_t)(base+1);  ch.circuit=(uint16_t)circuit; fx.channels.push_back(ch); }
    // Zoom (8 bit, LTP, lineaire) -> output_devval
    { wc::Channel ch; ch.attribute=wc::ATTR_ZOOM;  ch.combine=wc::COMBINE_LTP; ch.resolution=wc::RES_8BIT;
      ch.coarse_addr=(uint16_t)(base+2);  ch.circuit=(uint16_t)circuit; fx.channels.push_back(ch); }
    // Pan (16 bit, LTP, lineaire) -> output_devval
    { wc::Channel ch; ch.attribute=wc::ATTR_PAN;  ch.combine=wc::COMBINE_LTP; ch.resolution=wc::RES_16BIT;
      ch.coarse_addr=(uint16_t)(base+3); ch.fine_addr=(uint16_t)(base+4); ch.circuit=(uint16_t)circuit; fx.channels.push_back(ch); }
    // Tilt (16 bit, LTP, lineaire) -> output_devval
    { wc::Channel ch; ch.attribute=wc::ATTR_TILT; ch.combine=wc::COMBINE_LTP; ch.resolution=wc::RES_16BIT;
      ch.coarse_addr=(uint16_t)(base+5); ch.fine_addr=(uint16_t)(base+6); ch.circuit=(uint16_t)circuit; fx.channels.push_back(ch); }
    // R / G / B / W (8 bit, LTP, lineaire) -> output_devval
    wc::AttrId rgbw[4] = { wc::ATTR_COLORADD_R, wc::ATTR_COLORADD_G, wc::ATTR_COLORADD_B, wc::ATTR_COLORADD_W };
    for(int k=0;k<4;k++)
    { wc::Channel ch; ch.attribute=(uint8_t)rgbw[k]; ch.combine=wc::COMBINE_LTP; ch.resolution=wc::RES_8BIT;
      ch.coarse_addr=(uint16_t)(base+9+k); ch.circuit=(uint16_t)circuit; fx.channels.push_back(ch); }

    wc_patch.push_back(fx);
    rebuild_patch_from_fixtures();

    // [devices] valeurs vivantes de test (echelle 16 bit x257) : la lyre est visible tout de suite
    // dans le visualiseur (blanc plein, Pan/Tilt centres, shutter/zoom ouverts). L'intensite reste
    // pilotee par le circuit. Ces valeurs seront ecrasees par les faders de la fenetre Control Fixtures.
    output_devval[base+0]  = 0;      // Shutter (Blender : 0 = ouvert)
    output_devval[base+2]  = 0;      // Zoom
    output_devval[base+3]  = 32768;  // Pan  centre (16 bit)
    output_devval[base+5]  = 32768;  // Tilt centre (16 bit)
    output_devval[base+9]  = 65535;  // R plein
    output_devval[base+10] = 65535;  // G plein
    output_devval[base+11] = 65535;  // B plein
    output_devval[base+12] = 65535;  // W plein
    return 0;
}

// [devices] Cree un device a partir d'un GDTF (description.xml deja extrait) : mode <mode_index>,
// adresse de depart <base>, circuit <circuit>. Retire les fixtures qui chevauchent l'empreinte,
// pousse la fixture GDTF dans wc_patch, regenere le legacy, et seme des valeurs de test visibles.
// Retour : 0 OK ; 1 fichier absent ; 2 GDTF invalide ; 3 mode hors bornes ; -1 base invalide.
int create_device_from_gdtf_at(const char* xmlpath, int mode_index, int base, int circuit)
{
    if(base<1 || base>512) return -1;
    if(circuit<1)   circuit=1;
    if(circuit>512) circuit=512;

    wc::Fixture fx;
    std::string mode_name; int footprint=0;
    int r = wcgdtf::build_fixture(xmlpath, mode_index, base, circuit, fx, mode_name, footprint);
    if(r!=0) return r;                                // 1 absent / 2 invalide / 3 mode hors bornes
    if(base+footprint-1>512) return 2;               // depasse l'univers

    synthesize_fixtures_from_legacy();               // capturer l'etat courant dans le modele

    // retirer les fixtures existantes qui occupent base..base+footprint-1
    for(int out=base; out<base+footprint && out<514; out++)
    {
        for(size_t f=0; f<wc_patch.size(); )
        {
            bool hit=false;
            for(size_t c=0;c<wc_patch[f].channels.size();c++)
                if((int)wc_patch[f].channels[c].coarse_addr==out || (int)wc_patch[f].channels[c].fine_addr==out){ hit=true; break; }
            if(hit) wc_patch.erase(wc_patch.begin()+f); else f++;
        }
    }

    wc_patch.push_back(fx);
    rebuild_patch_from_fixtures();

    // [devices] valeurs par defaut GDTF (Default de chaque canal) posees dans output_devval :
    // shutter ouvert, control idle, position home, couleur ouverte... telles que definies par le fabricant.
    // Le Dimmer (intensite) reste pilote par le circuit (on n'y pose pas le defaut).
    for(size_t c=0;c<fx.channels.size();c++)
    {
        const wc::Channel& ch = fx.channels[c];
        if(ch.coarse_addr<1 || ch.coarse_addr>512) continue;
        if(ch.attribute==wc::ATTR_DIMMER) continue;
        output_devval[ch.coarse_addr] = ch.home;   // valeur vivante initiale = home (output_devdefault vient de rebuild)
    }

    sprintf(string_Last_Order, ">> GDTF: %.24s [%s] %d ch, %d addr @%d",
            fx.name.c_str(), mode_name.c_str(), (int)fx.channels.size(), footprint, base);
    return 0;
}

// ---------------------------------------------------------------------------
// Persistance TEXTE du patch fixtures (format maison versionne, lisible).
// 1 fichier patch_fixtures.whc dans le dossier du show. Petit -> non compresse.
//   WCPATCH <version>
//   <nb fixtures>
//   pour chaque fixture : <nb channels> puis 1 ligne / channel :
//     <attr> <combine> <resolution> <curve> <univers> <coarse> <fine> <circuit>
// ---------------------------------------------------------------------------
int save_patch_fixtures_text(const char* file)
{
    synthesize_fixtures_from_legacy();   // capture l'etat patch courant (tableaux legacy -> modele)
    FILE* fp = fopen(file, "wt");
    if(!fp) return 1;
    fprintf(fp, "WCPATCH 3\n");   // v3 : nom de la fixture (longueur-prefixe) avant les canaux ; v2 : champ home
    fprintf(fp, "%u\n", (unsigned)wc_patch.size());
    for(size_t f=0; f<wc_patch.size(); f++)
    {
        const wc::Fixture& fx = wc_patch[f];
        // nom (bandeau device) : longueur puis texte -> gere espaces et nom vide sans ambiguite de parsing
        fprintf(fp, "%u %s\n", (unsigned)fx.name.size(), fx.name.c_str());
        fprintf(fp, "%u\n", (unsigned)fx.channels.size());
        for(size_t c=0; c<fx.channels.size(); c++)
        {
            const wc::Channel& ch = fx.channels[c];
            fprintf(fp, "%d %d %d %d %d %d %d %d %d\n",
                    (int)ch.attribute, (int)ch.combine, (int)ch.resolution, (int)ch.curve,
                    (int)ch.universe, (int)ch.coarse_addr, (int)ch.fine_addr, (int)ch.circuit, (int)ch.home);
        }
    }
    fclose(fp);
    return 0;
}

// Retour : 0 = chargé OK ; 1 = fichier absent (vieux show) ; 2 = format invalide.
int load_patch_fixtures_text(const char* file)
{
    FILE* fp = fopen(file, "rt");
    if(!fp) return 1;
    int ver=0;
    if(fscanf(fp, " WCPATCH %d", &ver)!=1 || ver<1) { fclose(fp); return 2; }
    unsigned nfix=0;
    if(fscanf(fp, " %u", &nfix)!=1) { fclose(fp); return 2; }
    wc_patch.clear();
    for(unsigned f=0; f<nfix; f++)
    {
        wc::Fixture fx;
        if(ver>=3){   // v3 : nom de la fixture (longueur-prefixe -> gere espaces et nom vide)
            int nlen=0;
            if(fscanf(fp, " %d", &nlen)!=1){ fclose(fp); return 2; }
            if(nlen<0) nlen=0; if(nlen>200) nlen=200;
            fgetc(fp);   // consomme l'unique espace separateur avant le texte du nom
            if(nlen>0){ std::string nm; nm.resize(nlen); if(fread(&nm[0],1,(size_t)nlen,fp)==(size_t)nlen) fx.name.swap(nm); }
        }
        unsigned nch=0;
        if(fscanf(fp, " %u", &nch)!=1) { fclose(fp); return 2; }
        for(unsigned c=0; c<nch; c++)
        {
            int attr=0,comb=0,res=8,curve=0,uni=0,coarse=0,fine=0,circ=0,home=0;
            if(fscanf(fp, " %d %d %d %d %d %d %d %d",
                      &attr,&comb,&res,&curve,&uni,&coarse,&fine,&circ)!=8)
            { fclose(fp); return 2; }
            if(ver>=2){ if(fscanf(fp, " %d", &home)!=1) home=0; }   // v2 : home (compat v1 : 0)
            wc::Channel ch;
            ch.attribute   = (uint8_t)attr;
            ch.combine     = (uint8_t)comb;
            ch.resolution  = (uint8_t)res;
            ch.curve       = (uint8_t)curve;
            ch.universe    = (uint16_t)uni;
            ch.coarse_addr = (uint16_t)coarse;
            ch.fine_addr   = (uint16_t)fine;
            ch.circuit     = (uint16_t)circ;
            ch.home        = (uint16_t)home;
            fx.channels.push_back(ch);
        }
        wc_patch.push_back(fx);
    }
    fclose(fp);
    rebuild_patch_from_fixtures();   // modele -> tableaux legacy (que le rendu balaie)
    // [devices] etat vivant initial des attributs = home (sinon rouleaux a 0 apres reload) ;
    // une cue rappelee ensuite (refresh_mem_onstage) l'ecrasera avec ses valeurs.
    for(int o=1;o<514;o++) output_devval[o]=output_devdefault[o];
    return 0;
}

int do_curve_affectation()
{
for(int k=0;k<513;k++)
{
if(Dimmers_selected[k]==1)
{curves[k]=curve_selected;}
}
return(0);
}

int load_curve_in_temp_curve()
{
index_curve_spline_level=the_curve_spline_level[curve_selected];   //pixels de l editeur    le float est recalculé dans la boucle
return(0);
}

int check_dimmer_minus_override()
{
if(index_patch_overide==1)
{
for(int ck=1;ck<513;ck++)
{
if(Dimmers_selected[ck]==1)
        {
        if (ck>1)
        {
        Dimmers_selected[ck]=0;
        Dimmers_selected[ck-1]=1;
        patch_overide[ck]=0;
        patch_overide[ck-1]=1;
        }
        break;

}
}
}
 return(0);
}

int check_dimmer_plus_override()
{
if(index_patch_overide==1)
{
for(int ck=1;ck<513;ck++)
{
if(Dimmers_selected[ck]==1)
        {
        if (ck<513)
        {
        Dimmers_selected[ck]=0;
        Dimmers_selected[ck+1]=1;
        patch_overide[ck]=0;
        patch_overide[ck+1]=1;
        }
        break;
}
}
}

 return(0);
}



int do_logical_menu_curve(int XCurv, int YCurv)
{

if(mouse_x>(XCurv+30) && mouse_x<(XCurv+30+50) && mouse_y>(YCurv+10) && mouse_y<(YCurv+10+20))
{
index_enable_curve_editing=toggle(index_enable_curve_editing);
mouse_released=1;
}


for(int l=0;l<4;l++)
{
for (int curvenumber=0;curvenumber<4;curvenumber++)
{

if(mouse_x>(XCurv+20+(curvenumber*70)) && mouse_x<(XCurv+20+(curvenumber*70)+60) && mouse_y>(YCurv+370+(30*l)) && mouse_y<(YCurv+370+(30*l))+20)
{

  curve_selected=curvenumber+(l*4);

 //courbe droite
 if(index_reset_curve==1 && index_enable_curve_editing==1 && index_square_curve==0 && index_fluo_curve==0 && index_preheat_curve==0)
 {
 build_default_curve(curve_selected);
 index_reset_curve=0;
 mouse_released=1;
 }
 //courbe TV
 else if (index_square_curve==1 &&  index_enable_curve_editing==1 && index_reset_curve==0 && index_fluo_curve==0 && index_preheat_curve==0)
 {
 build_square_curve(curve_selected);
 index_square_curve=0;
  mouse_released=1;
 }
 //courbe preheat
 else if (index_preheat_curve==1 && index_enable_curve_editing==1 && index_square_curve==0 && index_reset_curve==0 && index_fluo_curve==0  )
 {
 build_preheat_curve(curve_selected);
 index_preheat_curve=0;
  mouse_released=1;
 }
 //courbe fluo
 else if (index_fluo_curve==1 && index_enable_curve_editing==1 && index_square_curve==0 && index_reset_curve==0 && index_preheat_curve==0)
 {
 build_fluo_curve(curve_selected);
 index_fluo_curve=0;
  mouse_released=1;
 }

else if(index_reset_curve==0 && index_square_curve==0 && index_fluo_curve==0 && index_preheat_curve==0 && mouse_released==0)//selection du curve
 {
 load_curve_in_temp_curve();
 mouse_released=1;
 }
}

}
}

// CHOIX PRESET DE CURVE///////////////////////////////////////////////////////////////////

for (int act=0;act<4;act++)
{
///ACTIONS
if(mouse_x>(XCurv+20+(act*70)) && mouse_x<(XCurv+20+(act*70)+60) && mouse_y>(YCurv+510) && mouse_y<(YCurv+510)+20)
{
switch(act)
{
case 0:
if(index_reset_curve==0){dessafect_index_curves();index_reset_curve=1; }
else if (index_reset_curve==1){index_reset_curve=0; }
break;
case 1:
if(index_square_curve==0){dessafect_index_curves();index_square_curve=1;}
else if (index_square_curve==1){index_square_curve=0; }
break;
case 2:
if(index_preheat_curve==0){dessafect_index_curves();index_preheat_curve=1; }
else if (index_preheat_curve==1){index_preheat_curve=0; }
break;
case 3:
if(index_fluo_curve==0){dessafect_index_curves();index_fluo_curve=1; }
else if (index_fluo_curve==1){index_fluo_curve=0; }
break;
}
mouse_released=1;
}

}
/////////////////////AFFECTATION AUX DIMERS SELECTIONNES///////////////////////////

if(mouse_x>XCurv+65 && mouse_x<XCurv+65+200 && mouse_y>YCurv+560 && mouse_y<YCurv+560+30)
{
do_curve_affectation();
patch_unselect_all_dimmers();
sprintf(string_Last_Order,">>Affected Curve  %d to Dimmer", curve_selected);
mouse_released=1;
}

return(0);
}



int do_logical_PatchBox(int XChan, int YChan, float scroll_chan)
{

//scroll
if(mouse_x>=XChan+340 && mouse_x<=XChan+430 && mouse_y>YChan+50 && mouse_y<YChan+250)
{
scroller_patch=mouse_y-(YChan+50);
}


for (int o=0;o<10;o++)
{

 if(mouse_x>XChan+345 && mouse_x<XChan+435 && mouse_y>YChan+290+(o*30) &&  mouse_y<YChan+310+(o*30)  )
 {
switch(o)
{
 case 0:
 patch_unselect_all_dimmers();
 break;
 case 1:
 index_affect_patch=toggle(index_affect_patch);
 break;
 case 2:
 patch_select_all_dimmers();
 break;
 case 3:
 index_do_clear_patch=1;
 index_ask_confirm=1;
 break;
 case 4:
 index_do_default_patch=1;
 index_ask_confirm=1;
 break;
 case 5:
 index_menu_curve=toggle(index_menu_curve);
 break;
 case 6:
 index_patch_overide=toggle(index_patch_overide);
 reset_check_grada_overide_dimmers();
 break;
 case 7:
 index_build_patch_from_plot=toggle(index_build_patch_from_plot);
 break;
 case 8:
 index_show_first_dim=toggle(index_show_first_dim);
 break;
 case 9:
 // [devices] "Patch device" : ouvre/ferme la fenetre W_DEVICEPATCH (recherche biblio + patch)
 if(index_window_devicepatch==0){ add_a_window(W_DEVICEPATCH); }
 else { substract_a_window(W_DEVICEPATCH); }
 break;
 default:
 break;
}
 mouse_released=1;
}

}



int maxchan_per_ligne=7;
int grad=0;
for (int l=0;l<86;l++)
{
if(l==85){maxchan_per_ligne=3;} //derniere ligne à 512

for (int c=1; c<maxchan_per_ligne;c++)
{
//over dimmer
if(mouse_x>(XChan + (45*c)-10) && mouse_x<(XChan + (45*c)+35) && mouse_y>YChan+50 && mouse_y>(YChan+90+(l*60) - (int)(scroller_patch* scroll_chan)) && mouse_y<(YChan+90+(l*60)+40 - (int)(scroller_patch* scroll_chan)))
{
grad=c+(l*6);
if(Dimmers_selected[grad]==0)
{
Dimmers_selected[grad]=1;
}
else if(Dimmers_selected[grad]==1)
{
Dimmers_selected[grad]=0;
}
if(index_affect_patch==1)//affectation manuelle à la souris
{
Patch[grad]=last_ch_selected;
output_fine[grad]=0; is_fine[grad]=0;   // [Fixtures] patch 8 bit normal : annule un eventuel appariement 16 bit
sprintf(string_Last_Order,">> Dimmer %d affected to Channel %d",grad, last_ch_selected);
sprintf(string_monitor_patch,">> Dimmer %d affected to Channel %d",grad, last_ch_selected);
patch_unselect_all_dimmers();
for (int ci=1;ci<514;ci++)
{Selected_Channel[ci]=0;}
index_type=0;index_level_attribue=0;
index_affect_patch=0;
}
if(index_affect_patch_16bit==1)//[2b] affectation 16 bit a la souris : coarse=grad, fine=grad+1
{
Patch[grad]=last_ch_selected;
Patch[grad+1]=last_ch_selected;
output_fine[grad]=grad+1;
is_fine[grad+1]=1;
sprintf(string_Last_Order,">> 16-bit channel (Dimmer %d + %d) patched to Channel %d",grad, grad+1, last_ch_selected);
sprintf(string_monitor_patch,">> 16-bit channel (Dimmer %d + %d) patched to Channel %d",grad, grad+1, last_ch_selected);
patch_unselect_all_dimmers();
for (int ci=1;ci<514;ci++)
{Selected_Channel[ci]=0;}
index_type=0;index_level_attribue=0;
index_affect_patch_16bit=0;
}
if(index_affect_patch_device==1)//[devices] clic sur output -> device (grad = adresse de depart) sur last_ch_selected
{
// [devices] tenter d'abord un GDTF depose dans user/gdtf.xml (1er mode) ; sinon fallback echafaudage MAC Aura.
int gr = create_device_from_gdtf_at("user/gdtf.xml", 0, grad, last_ch_selected);
if(gr==0)
{
    sprintf(string_monitor_patch,"%.1000s", string_Last_Order);   // message rempli par create_device_from_gdtf_at
}
else
{
    create_mac_aura_at(grad, last_ch_selected);
    const char* why = (gr==1)?"user/gdtf.xml absent":(gr==2)?"GDTF invalide":(gr==3)?"mode absent":"adresse invalide";
    sprintf(string_Last_Order,">> MAC Aura test @%d (fallback GDTF: %s)",grad, why);
    sprintf(string_monitor_patch,">> MAC Aura test @%d (fallback GDTF: %s)",grad, why);
}
patch_unselect_all_dimmers();
for (int ci=1;ci<514;ci++)
{Selected_Channel[ci]=0;}
index_type=0;index_level_attribue=0;
index_affect_patch_device=0;
}
generate_channel_view_list_from_patched_circuits();
mouse_released=1;
}


}
}

return(0);
}
