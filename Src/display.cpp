#include "graphics_backend.h"
#include "display.h"

/*-------------------------------------------------------------------------------------------------------------
                                 |
          CWWWWWWWW              | Copyright (C) 2009-2013  Christoph Guillermet
       WWWWWWWWWWWWWWW           |
       WWWWWWWWWWWWWWW           |               2026       Jacques Bouault - arpschuino.fr
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
volatile int ticks_dixieme_for_icat_and_draw = 0;
////////////////////AFFICHAGE///////////////////////////////////////////////////
int largeur_ecran = 1368;
int hauteur_ecran = 800;
int visu_performances[32];
Bitmap logo;
float myalpha = 0.0;
float myalphachan = 0.0;
float angle_souris = 0.0, transparence_souris = 1.0;
int config_color_style_is = 0;
float size_faders = 0.8;
// screens
int index_fullscreen = 0;
int posX_mainwindow = 0; // placement fenetre sur bureau
int posY_mainwindow = 0;
int index_report_customs[128]; // report des fentres on off, du view dmx, theme couleur etc etc
// BOXES
float epaisseur_ligne_fader = 2.0;
float demi_epaisseur_ligne_fader = 1.0;
float tiers_epaisseur_ligne_fader = 2.0 / 3;
float sizefader_epaisseur_ligne_fader = 2.0 * 0.8;
float double_epaisseur_ligne_fader = 2.0 * 2;
float epaisseur_bordure_fenetre = 3.0;
// Affichage
bool index_specify_size = 0;
bool index_set_pos = 0;
int windows_position[24][2];
int index_color_user_to_affect = 0;
int index_color_user_selected = 0;
bool receive_from_trichro = 0;
float couleurs_user[12][3];
char descriptif_colors_user_conf[32];
char tmp_time[24];
// variables globales d'affichage
char string_Last_Order[256];
char string_xy_mouse[12];
char string_display_fps[5];
char string_display_dmx_params[64];
char string_niveau[4];
char string_channel[3];
char string_function[8];
/////////////////////////
bool recall_windows_onoff[24];
int recall_windows_focus_id = 0;
/////////////////////////
float facteur_scroll_fader_space = 10.0;
int XGConfig = 670;
int YGConfig = 250;
// Visuels move window
bool index_click_move_trichro = 0;
bool index_click_move_visualpad = 0;
bool index_click_move_video = 0;
bool index_click_move_patch = 0;
bool index_click_move_time = 0;
bool index_click_move_sequentiel = 0;
bool index_click_move_artpollreply = 0;
bool index_click_move_faderspace = 0;
bool index_click_move_confirmwindow = 0;
bool index_click_move_configbox = 0;
bool index_clik_move_listproj = 0;
bool index_click_move_save_menu = 0;
bool index_click_move_savereportwindow = 0; // report
bool index_click_move_banger_window = 0;
bool index_click_move_cfg_window = 0;
