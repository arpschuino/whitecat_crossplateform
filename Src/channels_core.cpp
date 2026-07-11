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

* \file channels_9_core.cpp
* \brief {file for the core of dmx channels}
* \author Christoph Guillermet
* \version {0.8.6.3}
* \date {12/02/2015}

 White Cat {- categorie} {- sous categorie {- sous categorie}}

*   Gère les fonction core des channels dmx
*
*   Core fonctions for the dmx channels
*
 **/

#include "wc_tus.h"
#include "gui_boutons_rebuild1.h"
int key_up();
int key_down();
int add_channel_selection_to_layers_plot();
int substract_channel_selection_to_layers_plot();

int snap_channels_selection_array()//pour saisie continue des circuits, au click button
{
for (int i=0;i<513;i++)
{
 Temp_Selected_Channel[i]=Selected_Channel[i];
}
return(0);
}


int channel_copy()
{
    for(int ck=1;ck<513;ck++)
    {
    bufferCopyPaste[ck]=0;
    SelectedForCopyPaste[ck]=0;
    if(index_blind==0)
    {
    if(Selected_Channel[ck]==1)
    {
    SelectedForCopyPaste[ck]=1;
    bufferCopyPaste[ck]=Tmax (bufferSequenciel[ck],bufferFaders[ck]);
    }
    }
    else
    {
    if(Selected_Channel[ck]==1)
    {
    SelectedForCopyPaste[ck]=1;
    bufferCopyPaste[ck]=bufferBlind[ck];
    }
    }
    }
    sprintf(string_Last_Order,">> Pasted selected channels in memory");
return(0);
}

int channel_paste()
{
    for(int ck=1;ck<513;ck++)
    {
    if(index_blind==0)
    {
    if(SelectedForCopyPaste[ck]==1){bufferSaisie[ck]=bufferCopyPaste[ck];}
        sprintf(string_Last_Order,">> Pasted channels from memory to Stage");
    }
    else{
    if(SelectedForCopyPaste[ck]==1){bufferBlind[ck]=bufferCopyPaste[ck];}
        sprintf(string_Last_Order,">> Pasted channels from memory to Blind");
    }
    }

return(0);
}



int reset_blind()
{
 for (int ci=1;ci<514;ci++)
 {
 bufferBlind[ci]=0;
 }
 return(0);
}


int Channel_select_all()
{
 if(index_blind==0)
 {
 for (int ci=1;ci<514;ci++)
 {
 if(bufferSaisie[ci]==0)    {Selected_Channel[ci]=0;}
 else if (bufferSaisie[ci]>0)    {Selected_Channel[ci]=1;}
 }
 }
 else if(index_blind==1)
 {
 for (int ci=1;ci<514;ci++)
 {
 if(bufferBlind[ci]==0)    {Selected_Channel[ci]=0;}
 else if (bufferBlind[ci]>0)    {Selected_Channel[ci]=1;}
 }
 }
 sprintf(string_Last_Order,">> Selected All lighting channels ( ALL )");
return(0);
}

int Channel_select_inv()
{
 if(index_blind==0)
 {
 for (int ci=1;ci<514;ci++)
 {
 if(bufferSaisie[ci]>0 && Selected_Channel[ci]==1)    {Selected_Channel[ci]=0;}
 else if (bufferSaisie[ci]>0 && Selected_Channel[ci]==0)    {Selected_Channel[ci]=1;}
 }
 }
 else if(index_blind==1)
 {
 for (int ci=1;ci<514;ci++)
 {
 if(bufferBlind[ci]>0 && Selected_Channel[ci]==1)    {Selected_Channel[ci]=0;}
 else if (bufferBlind[ci]>0 && Selected_Channel[ci]==0)    {Selected_Channel[ci]=1;}
 }
 }
 sprintf(string_Last_Order,">> Selected All lighting channels non selected ( INV )");
return(0);
}


int Channel_select_thruth(int fromch, int toch)
{
 if(fromch<toch)
 {
  for (int cit=fromch;cit<=toch;cit++)
 {Selected_Channel[cit]=1;}
 }
 if (fromch>toch)
 {
  for (int cit=toch;cit<=fromch;cit++)  {Selected_Channel[cit]=1;}
 }
  sprintf(string_Last_Order,">> Selected from Ch %d to Ch %d", fromch, toch);
 return(0);
}


int Channel_at_level()
{
 // [2c-2B] saisie grossiere (%/DMX 0-255) convertie en 16 bit. Le fin viendra de molette/fleches+Ctrl.
 int chlevelis=0;
 if(dmx_view==0)
 {int p=(int)(atof(numeric)+0.5); if(p>100){p=100;} if(p<0){p=0;} chlevelis=wc::pct_to_lvl(p); }
 else if (dmx_view==1)
 {int d=atol(numeric); if(d>255){d=255;} if(d<0){d=0;} chlevelis=wc::dmx8_to_lvl((unsigned char)d);}
 if(chlevelis>65535){chlevelis=65535;}
 if(chlevelis<0){chlevelis=0;}

 for (int ci=1;ci<514;ci++)
 {
 if(Selected_Channel[ci]==1 && index_blind==0)    {bufferSaisie[ci]=chlevelis;}
 else if(Selected_Channel[ci]==1  && index_blind==1)    {bufferBlind[ci]=chlevelis;}
 }

 if(dmx_view==0){sprintf(string_Last_Order,">> Selection AT %d%%", wc::lvl_to_pct((unsigned short)chlevelis));}
 else {sprintf(string_Last_Order,">> Selection AT %d", wc::lvl_to_dmx8((unsigned short)chlevelis));}
 return(0);
}

int wc_window_under_mouse();           // detection fenetre sous le curseur (procs_visuels_rebuild1.cpp)
int fader_set_level(int cmptfader, int val); // setter master fader (core.cpp)

int DoMouseLevel()
{
 // fenetre actuellement SOUS le curseur (detection FRAICHE, sans effet de bord) : sert aux gardes
 // molette du grand master (== aucune fenetre) et des faders (== W_FADERS). index_over_A_window est
 // fige au dernier clic et ne convient pas pour une garde au survol.
 int win_under = wc_window_under_mouse();

 // [crossfade 16 bit] molette sur les faders X1/X2 du sequenciel :
 //   molette = pas COARSE (1 DMX = 257 en 16 bit) ; Ctrl+molette = pas FIN (1/65535).
 //   Reutilise la meme courbe veloce dynamique que la molette circuits (1,1,5,20,45 max).
 {
   static int last_scroll_mouse_for_xfade = 0;
   bool overX1 = (mouse_x > xseq_window+480 && mouse_x < xseq_window+525);
   bool overX2 = (mouse_x > xseq_window+580 && mouse_x < xseq_window+625);
   if (window_focus_id == W_SEQUENCIEL &&
       mouse_y > yseq_window+35 && mouse_y < yseq_window+330 && (overX1 || overX2)) {
       int _delta = mouse_z - last_scroll_mouse_for_xfade;
       if (_delta != 0) {
           int _absd  = _delta > 0 ? _delta : -_delta;
           int _d     = _absd > 2 ? _absd - 2 : 0;
           int _steps = _d > 0 ? _d * _d * 5 : 1;   // courbe veloce dynamique (sans inertie)
           if (_steps > 45) _steps = 45;            // plafond
           // etat Ctrl LIVE : key_shifts n'est rafraichi qu'aux events clavier, pas a la molette
           // -> il resterait colle a "fine" apres relachement du Ctrl. SDL_GetModState() est a jour.
           bool fine   = (SDL_GetModState() & KMOD_CTRL) || index_false_control == 1;
           int  unit   = fine ? 1 : 257;            // fin = 1/65535 ; coarse = 1 DMX (x257)
           int  change = (_delta > 0 ? 1 : -1) * _steps * unit;
           index_go = 0; index_go_back = 0; index_pause = 0;
           if (overX1) {
               niveauX1 += change;
               if (niveauX1 < 0)     niveauX1 = 0;
               if (niveauX1 > 65535) niveauX1 = 65535;
               if (index_x1_x2_together == 1) {
                   if (((255.0 - ratio_X1X2_together) / 255) == 1.0) niveauX2 = 65535 - niveauX1;
                   else niveauX2 = remapX2[(255 - (niveauX1 >> 8))];
                   if (niveauX2 < 0)     niveauX2 = 0;
                   if (niveauX2 > 65535) niveauX2 = 65535;
               }
           } else {
               niveauX2 += change;
               if (niveauX2 < 0)     niveauX2 = 0;
               if (niveauX2 > 65535) niveauX2 = 65535;
               if (index_x1_x2_together == 1) {
                   if (((255.0 - ratio_X1X2_together) / 255) == 1.0) niveauX1 = 65535 - niveauX2;
                   else niveauX1 = 65535 - remapX1[(niveauX2 >> 8)];
                   if (niveauX1 < 0)     niveauX1 = 0;
                   if (niveauX1 > 65535) niveauX1 = 65535;
                   if (niveauX2 == 65535) niveauX1 = 0;
               }
           }
           if (midi_send_out[491] == 1) index_send_midi_out[491] = 1;
           if (midi_send_out[492] == 1) index_send_midi_out[492] = 1;
           last_scroll_mouse_for_xfade = mouse_z;
           last_scroll_mouse_for_chan  = mouse_z; // empeche le bloc niveau-circuit de refirer sur ce scroll
       }
       return (0); // molette consommee par le crossfade manuel : pas de modif niveau circuit
   }
   last_scroll_mouse_for_xfade = mouse_z; // hors survol : garde la baseline fraiche (evite un saut au survol)
 }

 // [GM 16 bit] molette sur le grand master : coarse (1 DMX = 257) / Ctrl+molette = fin (1/65535),
 //   meme courbe veloce dynamique. GM dessine en 1050,55 larg 40 (cf graphics_rebuild1 / procs_visuels).
 {
   static int last_scroll_mouse_for_gm = 0;
   const int GMX = 1050, GMY = 55, GMlarg = 40;
   // [GM 16 bit] molette active quel que soit le focus (le GM est sur le bureau), mais PAS si le
   // curseur est au-dessus d'une fenetre (win_under, frais).
   if (index_allow_grand_master == 1 && win_under == 0 &&
       mouse_x > GMX && mouse_x < GMX + GMlarg && mouse_y >= GMY - 20 && mouse_y <= GMY + 275) {
       int _delta = mouse_z - last_scroll_mouse_for_gm;
       if (_delta != 0) {
           int _absd  = _delta > 0 ? _delta : -_delta;
           int _d     = _absd > 2 ? _absd - 2 : 0;
           int _steps = _d > 0 ? _d * _d * 5 : 1;   // courbe veloce dynamique
           if (_steps > 45) _steps = 45;
           bool fine   = (SDL_GetModState() & KMOD_CTRL) || index_false_control == 1;
           int  unit   = fine ? 1 : 257;            // fin = 1/65535 ; coarse = 1 DMX (x257)
           niveauGMaster += (_delta > 0 ? 1 : -1) * _steps * unit;
           if (niveauGMaster < 0)     niveauGMaster = 0;
           if (niveauGMaster > 65535) niveauGMaster = 65535;
           midi_levels[615] = wc::lvl_to_dmx8(niveauGMaster) / 2;
           if (midi_send_out[615] == 1) index_send_midi_out[615] = 1;
           last_scroll_mouse_for_gm   = mouse_z;
           last_scroll_mouse_for_chan = mouse_z; // empeche le bloc niveau-circuit de refirer sur ce scroll
       }
       return (0); // molette consommee par le grand master
   }
   last_scroll_mouse_for_gm = mouse_z; // hors survol : garde la baseline fraiche
 }

 // [fader 16 bit] molette sur la fenetre faders = master survole.
 //   molette = coarse (1 DMX = 257, courbe veloce) ; Ctrl+molette = fin (1/65535).
 {
   static int last_scroll_mouse_for_fader = 0;
   if (win_under == W_FADERS && mouse_y >= YFader && mouse_y <= YFader + 255) {
       int _delta = mouse_z - last_scroll_mouse_for_fader;
       if (_delta != 0) {
           int x_base     = XFader - (int)(scroll_faderspace * facteur_scroll_fader_space);
           int largeur    = (int)(50 * size_faders);
           int espacement = (int)(150 + (190 - 150) * size_faders);
           int cmptfader  = -1;
           for (int f = 0; f < 48; f++) {
               int fx = x_base + f * espacement;
               if (mouse_x >= fx && mouse_x <= fx + largeur) { cmptfader = f; break; }
           }
           if (cmptfader >= 0) {
               bool fine = (SDL_GetModState() & KMOD_CTRL) || index_false_control == 1;
               int _absd  = _delta > 0 ? _delta : -_delta;
               int _d     = _absd > 2 ? _absd - 2 : 0;
               int _steps = _d > 0 ? _d * _d * 5 : 1;       // courbe veloce dynamique
               if (_steps > 45) _steps = 45;
               int unit = fine ? 1 : 257;                   // [fader 16 bit] coarse = 1 DMX (257), fin = 1/65535
               int step = (_delta > 0 ? 1 : -1) * _steps * unit;
               int val = (int)Fader[cmptfader] + step;
               if (val < 0)     val = 0;
               if (val > 65535) val = 65535;
               // sortie de LFO si actif (comme le drag souris du fader)
               if (lfo_mode_is[cmptfader] == 1 || lfo_mode_is[cmptfader] == 2 || lfo_cycle_is_on[cmptfader] == 1) {
                   lfo_mode_is[cmptfader] = 0; lfo_cycle_is_on[cmptfader] = 0;
               }
               fader_set_level(cmptfader, val);
           }
           last_scroll_mouse_for_fader = mouse_z;
           last_scroll_mouse_for_chan  = mouse_z; // empeche le bloc niveau-circuit de refirer
       }
       return (0); // molette consommee par les faders
   }
   last_scroll_mouse_for_fader = mouse_z; // hors survol : garde la baseline fraiche
 }

 // [grid 16 bit / stage B] Ctrl+molette sur la case survolee d'une grille en edition = retouche
 // FINE 16 bit (+/-1 par cran, meme acceleration que la molette-circuit). Mirroir du bloc fader :
 // consomme la molette (return) pour ne PAS aussi piloter les circuits selectionnes.
 {
   static int last_scroll_mouse_for_grid = 0;
   if (window_focus_id == W_GRID && index_enable_edit_Grider == 1 && grid_wheel_hover_player >= 0
       && (SDL_GetModState() & KMOD_CTRL)) {
       int _delta = mouse_z - last_scroll_mouse_for_grid;
       if (_delta != 0) {
           int _gsel = index_grider_selected[grid_wheel_hover_player];
           int _step = index_grider_step_is[grid_wheel_hover_player];
           int _absd  = _delta > 0 ? _delta : -_delta;
           int _d     = _absd > 2 ? _absd - 2 : 0;
           int _steps = _d > 0 ? _d * _d * 5 : 1;   // 1 cran = 1/65535 ; coup vif jusqu'a 45
           if (_steps > 45) _steps = 45;
           int _val = (int)grid_levels[_gsel][_step][position_grid_editing] + (_delta > 0 ? _steps : -_steps);
           if (_val < 0)     _val = 0;
           if (_val > 65535) _val = 65535;
           grid_levels[_gsel][_step][position_grid_editing] = _val;
           last_scroll_mouse_for_grid = mouse_z;
           last_scroll_mouse_for_chan = mouse_z; // empeche le bloc circuit de refirer sur ce scroll
       }
       return (0); // molette consommee par la grille
   }
   last_scroll_mouse_for_grid = mouse_z; // hors edition/Ctrl : garde la baseline fraiche
 }

 // [devices] molette sur les faders de la fenetre Control Fixtures :
 //   coarse = 1 DMX (257) ; Ctrl+molette = fin (1/65535), meme courbe veloce que les autres faders.
 //   Cible publiee par fixturectl_window : -1 aucun / 0 intensite (bufferSaisie[circuit]) / >=1 output (output_devval).
 {
   static int last_scroll_mouse_for_fxc = 0;
   if (win_under == W_FIXTURECTL && fixturectl_wheel_hover >= 0) {
       int _delta = mouse_z - last_scroll_mouse_for_fxc;
       if (_delta != 0) {
           int _absd  = _delta > 0 ? _delta : -_delta;
           int _d     = _absd > 2 ? _absd - 2 : 0;
           int _steps = _d > 0 ? _d * _d * 5 : 1;   // courbe veloce dynamique
           if (_steps > 45) _steps = 45;
           bool fine   = (SDL_GetModState() & KMOD_CTRL) || index_false_control == 1;
           int  unit   = fine ? 1 : 257;            // fin = 1/65535 ; coarse = 1 DMX (x257)
           int  change = (_delta > 0 ? 1 : -1) * _steps * unit;
           if (fixturectl_wheel_hover == 0) {       // intensite -> circuit selectionne
               if (last_ch_selected > 0 && last_ch_selected < 514) {
                   int v = (int)bufferSaisie[last_ch_selected] + change;
                   if (v < 0)     v = 0;
                   if (v > 65535) v = 65535;
                   bufferSaisie[last_ch_selected] = (unsigned short)v;
               }
           } else if (fixturectl_wheel_hover >= 1 && fixturectl_wheel_hover < 514) {   // attribut -> output_devval
               int v = (int)output_devval[fixturectl_wheel_hover] + change;
               if (v < 0)     v = 0;
               if (v > 65535) v = 65535;
               output_devval[fixturectl_wheel_hover] = (unsigned short)v;
           }
           last_scroll_mouse_for_fxc  = mouse_z;
           last_scroll_mouse_for_chan = mouse_z; // empeche le bloc circuit de refirer sur ce scroll
       }
       return (0); // molette consommee par la fenetre Control Fixtures
   }
   last_scroll_mouse_for_fxc = mouse_z; // hors survol : garde la baseline fraiche
 }

 {
 int _delta = mouse_z - last_scroll_mouse_for_chan;
 if (_delta != 0) {
     int _absd = _delta > 0 ? _delta : -_delta;
     int _d    = _absd > 2 ? _absd - 2 : 0;
     int _steps = _d > 0 ? _d * _d * 5 : 1;  // 1→1, 2→1, 3→5, 4→20, 5→45
     if (_steps > 45) _steps = 45;            // plafond
     for (int _i = 0; _i < _steps; _i++) {
         if (_delta > 0) key_up();
         else            key_down();
     }
     last_scroll_mouse_for_chan = mouse_z;
 }
 }

 // molette sur la fenêtre audio
 if(window_focus_id == W_AUDIO && mouse_x > XAudio+350 && mouse_x < XAudio+590)
 {
  int delta = mouse_z - last_scroll_mouse_for_audio;
  if(delta != 0)
  {
   if(index_show_audio_folder_list && mouse_y > YAudio+40 && nbre_audio_folders > 8)
   {
    int max_s = nbre_audio_folders - 8;
    audio_folder_list_scroll -= delta;
    if(audio_folder_list_scroll < 0) audio_folder_list_scroll = 0;
    if(audio_folder_list_scroll > max_s) audio_folder_list_scroll = max_s;
   }
   else if(mouse_y > YAudio+45 && mouse_y < YAudio+45+index_nbre_players_visibles*120)
   {
    int vc = index_nbre_players_visibles*6 - 1;
    int max_s = audio_number_total_in_folder > vc ? audio_number_total_in_folder - vc : 0;
    line_audio -= delta;
    if(line_audio < 0) line_audio = 0;
    if(line_audio > max_s) line_audio = max_s;
   }
   last_scroll_mouse_for_audio = mouse_z;
  }
 }

 if (window_focus_id==W_BANGER &&  mouse_x>=X_banger && mouse_x<=X_banger+480 && mouse_y>=Y_banger && mouse_y<=Y_banger+300  )//banger
 {
    if(over_family==0)
    {
        if(editing_banger_family ==1 &&  over_banger_event>0)
        {
            if(mouse_z>mouse_level_for_event)
            {
                bangers_type[index_banger_selected][ over_banger_event-1]++;
                reset_banger_event(index_banger_selected, over_banger_event-1);
                constrain_banger_type( over_banger_event-1);
                position_mouse_z(0);  //reset
                mouse_level_for_event=mouse_z;
                mouse_level_for_banger=mouse_z;
                editing_banger_family=0;editing_banger_action=0;
            }
            if(mouse_z<mouse_level_for_event)
            {
                bangers_type[index_banger_selected][ over_banger_event-1]--;
                if(bangers_type[index_banger_selected][ over_banger_event-1]<0){
                    bangers_type[index_banger_selected][ over_banger_event-1]=0;
                }
                // [iCat retiré] categorie 9 supprimee : au decrement on la saute vers le bas (9 -> 8)
                if(bangers_type[index_banger_selected][ over_banger_event-1]==9){
                    bangers_type[index_banger_selected][ over_banger_event-1]=8;
                }
                reset_banger_event(index_banger_selected, over_banger_event-1);
                position_mouse_z(0);  //reset
                mouse_level_for_event=mouse_z;
                mouse_level_for_banger=mouse_z;
                editing_banger_family=0;editing_banger_action=0;
            }
        }
    }
    else{
        if(editing_banger_action==1 && over_banger_action>0)
        {
            if(mouse_z>mouse_level_for_banger)
            {
                position_mouse_z(0);//reset
                mouse_level_for_banger=mouse_z;
                mouse_level_for_event=mouse_z;
                editing_banger_family=0;editing_banger_action=0;
                bangers_action[index_banger_selected][over_banger_action-1]++;
                reset_banger_params(index_banger_selected,over_banger_action-1);
                constrain_banger_param(over_banger_action-1);
            }
            if(mouse_z<mouse_level_for_banger)
            {
                position_mouse_z(0);//reset
                mouse_level_for_banger=mouse_z;
                mouse_level_for_event=mouse_z;
                bangers_action[index_banger_selected][over_banger_action-1]--;
                if(bangers_action[index_banger_selected][over_banger_action]<0){
                    bangers_action[index_banger_selected][over_banger_action-1]=0;
                }
                reset_banger_params(index_banger_selected,over_banger_action-1);
            }
        }
    }






    
 }
 else
 {
    mouse_level_for_event=mouse_z;
    mouse_level_for_banger=mouse_z;
    editing_banger_family=0;editing_banger_action=0;
 }
 return(0);
}





int do_logical_ChannelScroller( int ScrollX, int ScrollY)
{

if(mouse_x>=ScrollX-10 && mouse_x<ScrollX-10+100 && mouse_y>ScrollY-5 && mouse_y<ScrollY+270
&& index_over_A_window==0 &&  index_over_faderspace==0 && mouse_released==0)
{
if( Midi_Faders_Affectation_Type!=0 )
{
attribute_midi_solo_affectation(1624,Midi_Faders_Affectation_Mode);
mouse_released=1;
}
else
{
scroll_channelspace=mouse_y-ScrollY;
index_moving_channel_scroller=1;
}
}
return(0);
}

int do_logical_Draw_Channel_Preset_View( int xchan, int ychan,   int prst_v)
{

int maxchan_per_ligne=12;
int nbre_lignes=channel_number_of_lines[prst_v];
int num_circ=0;
int ypos_ch=0;



for (int l=0;l<nbre_lignes+1;l++)
{
//if (l==nbre_lignes) {maxchan_per_ligne=(channel_number_in_View[prst_v]%13)+1;} //derniere ligne à 512
for (int c=1; c<=maxchan_per_ligne;c++)
{
ypos_ch=ychan+(l*70);
num_circ=Channel_View_ROUTING[prst_v][c+(l*12)-1];
//Selection circuit
if (index_over_A_window==0 && index_over_faderspace==0 && mouse_x>   ((xchan-10) + (45*c)) && mouse_x< ((xchan+30) + (45*c))
&& mouse_y> ypos_ch && mouse_y< ypos_ch+65
&&  num_circ!=0 && num_circ<513)
{

if(index_level_attribue==1)//pour déselection lors prochain circuit piqué dominique guesdon 10aout 2010
{
for(int y=1;y<512;y++)
{
Selected_Channel[y]=0;
last_ch_selected=0;
if(index_plot_window==1){substract_channel_selection_to_layers_plot();}
}
index_level_attribue=0;
}

if (index_ch_thruth==0)//selection normale
 {
 Selected_Channel[num_circ]=1-(Temp_Selected_Channel[num_circ]); //chainage mouse released sorti
 last_ch_selected=num_circ;
 if(index_plot_window==1){add_channel_selection_to_layers_plot();substract_channel_selection_to_layers_plot();}
 }
 else if (index_ch_thruth==1)//selection thruth
 {
 Channel_select_thruth(last_ch_selected,num_circ);
 index_ch_thruth=0;
 mouse_released=1;
 if(index_plot_window==1){add_channel_selection_to_layers_plot();substract_channel_selection_to_layers_plot();}
 }
 //
}
}

}

return(0);
}


int do_logical_Draw_Channel_Preset_Title(int xchan, int ychan, int prst_v)
{


 if(window_focus_id==0 && mouse_button==1  && mouse_released==0 && prst_v!=0 )
 {
 if(mouse_y>ychan && mouse_y<ychan+20  )
 {
 //TITRE VIEW
 if(mouse_x> xchan+40 && mouse_x<xchan+40+210)
 {
 if(prst_v>0 && index_type==1 )
 {
 sprintf( channel_view_Name[prst_v],numeric);
 reset_numeric_entry();
 numeric_postext=0;
 mouse_released=1;
 }
 }
 // Mem sel
 else if(mouse_x>  xchan+435 && mouse_x<xchan+435+20)
 {
 if(channel_view_type_of_behaviour[prst_v]!=1){channel_view_type_of_behaviour[prst_v]=1;}
 else{channel_view_type_of_behaviour[prst_v]=0;}
 mouse_released=1;
 }
 // fader sel
 else if(mouse_x>  xchan+460 && mouse_x<xchan+460+20 )
 {
 if(channel_view_type_of_behaviour[prst_v]!=2){channel_view_type_of_behaviour[prst_v]=2;}
 else{channel_view_type_of_behaviour[prst_v]=0;}
 mouse_released=1;
 }

 //All sel
  else if(mouse_x>  xchan+485 && mouse_x<xchan+485+30  )
 {
 channel_view_mode_builder[prst_v]=toggle(channel_view_mode_builder[prst_v]);
 mouse_released=1;
 }

 //do build logique sortie car plantage grave à la reconstruction. il faut garder ca dans la boucle affichage
/* else if(mouse_x>  xchan+520 && mouse_x<xchan+520+55 )
 {
 channel_view_is=prst_v;
 if(index_do_dock==1)
 {
 if(channel_view_type_of_behaviour[channel_view_is]==0 &&  channel_view_mode_builder[channel_view_is]==0)
 {index_ask_record_selection_of_view=1 ; index_ask_confirm=1; mouse_released=1;}
 else  {index_ask_build_view=1; index_ask_confirm=1; mouse_released=1;  }
 }

 else if(index_do_modify==1)
 {
 index_ask_modify_selection_of_view=1 ;index_ask_confirm=1;   mouse_released=1;
 }
 else if(index_do_report==1)
 {
 index_ask_report_selection_of_view=1;index_ask_confirm=1;      mouse_released=1;
 }
 else if(index_main_clear==1)
 {
 index_ask_clear_selection_of_view=1;   mouse_released=1;
 }

 } */


 }

 }

 return(0);
}




int do_logical_ClassicalChannelSpace( int xchan, int ychan, int scroll)//les 512 circuits
{

int maxchan_per_ligne=13;
for (int l=0;l<43;l++)
{
if (l==42) {maxchan_per_ligne=9;} //derniere ligne à 512

for (int c=1; c<maxchan_per_ligne;c++)
{

//Selection circuit
if ( mouse_x>   ((xchan-10) + (45*c)) && mouse_x< ((xchan+30) + (45*c))
&& mouse_y> (((ychan*l)+ 40) - (int)((float)(scroll)* Ch_Scroll_Factor) )
&& mouse_y< (((ychan*l)+ 100 - (int)((float)(scroll)* Ch_Scroll_Factor)) )
&& index_over_A_window==0 && index_over_faderspace==0
)
{

if(index_level_attribue==1)//pour déselection lors prochain circuit piqué dominique guesdon 10aout 2010
{
for(int y=1;y<512;y++)
{
Selected_Channel[y]=0;
last_ch_selected=0;
if(index_plot_window==1){substract_channel_selection_to_layers_plot();}
}
index_level_attribue=0;
}

 if (index_ch_thruth==0)//selection normale
 {
 Selected_Channel[c+(l*12)]=1-(Temp_Selected_Channel[c+(l*12)]); //chainage mouse released sorti
 if(Selected_Channel[c+(l*12)]==1){ last_ch_selected=c+(l*12);}
 if(index_plot_window==1){add_channel_selection_to_layers_plot();substract_channel_selection_to_layers_plot();}
 }
 else if (index_ch_thruth==1)//selection thruth
 {
 Channel_select_thruth(last_ch_selected,c+(l*12));
 last_ch_selected=c+(l*12);
 index_ch_thruth=0;
 mouse_released=1;
 if(index_plot_window==1){add_channel_selection_to_layers_plot();substract_channel_selection_to_layers_plot();}
 }
 //
}
}

}

return(0);
}


int do_logical_ChannelsMenuSelection(int chx, int chy)
{

//LOGIQUE
//if(window_focus_id==0 && index_over_A_window==0 && index_over_faderspace==0 && mouse_button==1 && mouse_released==0)
if(window_focus_id==0 && index_over_faderspace==0 && mouse_button==1 && mouse_released==0)
{
// bouton Classical
if( mouse_y>chy+31 && mouse_y<chy+49 && mouse_x>chx+70 && mouse_x<chx+130 )
{
if(Midi_Faders_Affectation_Type!=0)
{ attribute_midi_solo_affectation(1644,Midi_Faders_Affectation_Mode); }
else
{ ClassicalChannelView=toggle(ClassicalChannelView); }
mouse_released=1;
}
// bouton Patched Ch. (vue 0)
if( mouse_y>chy+31 && mouse_y<chy+49 && mouse_x>chx+140 && mouse_x<chx+200 )
{
if(Midi_Faders_Affectation_Type!=0)
{ attribute_midi_to_control(1628,Midi_Faders_Affectation_Type,Midi_Faders_Affectation_Mode); }
else
{
if(ClassicalChannelView==1){ClassicalChannelView=0;if(Channel_View_MODE[0]==0){Channel_View_MODE[0]=1;}}
else{Channel_View_MODE[0]=toggle(Channel_View_MODE[0]);ClassicalChannelView=0;}
}
mouse_released=1;
}
// carrés vues 2-16 (i=1..15)
for(int i=1;i<16;i++)
{
if(mouse_x>chx+210+((i-1)*23) && mouse_x<chx+228+((i-1)*23) && mouse_y>chy+31 && mouse_y<chy+49)
{
if(Midi_Faders_Affectation_Type!=0)
{ attribute_midi_to_control(1628+i,Midi_Faders_Affectation_Type,Midi_Faders_Affectation_Mode); }
else
{
if(index_do_dock==0 && index_do_modify==0 && index_main_clear==0)
{
if(ClassicalChannelView==1){ClassicalChannelView=0;if(Channel_View_MODE[i]==0){Channel_View_MODE[i]=1;}}
else{Channel_View_MODE[i]=toggle(Channel_View_MODE[i]);ClassicalChannelView=0;}
}
else if(index_do_dock==1)
{channel_view_is=i;index_ask_record_selection_of_view=i;index_ask_confirm=1;}
else if(index_do_modify==1)
{channel_view_is=i;index_ask_modify_selection_of_view=1;index_ask_confirm=1;}
else if(index_main_clear==1)
{channel_view_is=i;index_ask_confirm=1;index_ask_clear_selection_of_view=1;}
}
mouse_released=1;
}
}
command_button_logical(chx+0,chy+1,index_do_freeze,"Freeze","",1335,115);
command_button_logical(chx+70,chy+1,index_do_exclude,"Exclude","",1541,116);
command_button_logical(chx+140,chy+1,index_blind,"Blind","Shift-F10",754,110);
command_button_logical(chx+210,chy+1,index_direct_chan,"Direct CH.","",1333,111);
command_button_logical(chx+280,chy+1,index_inspekt,"View","",1334,112);
command_button_logical(chx+350,chy+1,index_do_hipass,"HiPass","",1542,113);
command_button_logical(chx+420,chy+1,index_do_fgroup,"FGroup","",1592,114);

command_button_logical(chx+490,chy+1,Midi_Faders_Affectation_Type,"MidiAffect","",1625,106);
command_button_logical(chx+560,chy+1,index_midi_mute,"MidiMute","",1277,107);
command_button_logical(chx+630,chy+1,index_global_midi_send_on_faders,"M.Out Fad.","",1593,109);

command_button_logical(chx+700,chy+1,index_do_dock,"STORE","F1",743,102);
command_button_logical(chx+770,chy+1,index_do_modify,"MODIFY","F2",744,103);
command_button_logical(chx+840,chy+1,index_do_report,"REPORT","F3",745,104);
command_button_logical(chx+910,chy+1,index_main_clear,"CLEAR","F4",746,105);
command_button_logical(chx+980,chy+1,index_show_main_menu,"MENUS","RIGHT CLICK",1626,108);
command_button_logical(chx+1050,chy+1,index_call_help,"Help","",1342,42);
}
//fin windows focus
return(0);
}
