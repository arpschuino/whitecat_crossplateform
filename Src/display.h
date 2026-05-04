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

#pragma once

extern volatile int mouse_button;
extern volatile int mouse_released;
extern volatile int ticks_dixieme_for_icat_and_draw;
////////////////////AFFICHAGE///////////////////////////////////////////////////
extern int largeur_ecran;
extern int hauteur_ecran;
extern int visu_performances[32];
extern Bitmap logo;
extern float myalpha;
extern float myalphachan;
extern float angle_souris, transparence_souris;
extern int config_color_style_is;
extern float size_faders;
// screens
extern int index_fullscreen;
extern int posX_mainwindow;
extern int posY_mainwindow;
extern int index_report_customs[128];
// BOXES
extern float epaisseur_ligne_fader;
extern float demi_epaisseur_ligne_fader;
extern float tiers_epaisseur_ligne_fader;
extern float sizefader_epaisseur_ligne_fader;
extern float double_epaisseur_ligne_fader;
extern float epaisseur_bordure_fenetre;
// Affichage
extern bool index_specify_size;
extern bool index_set_pos;
extern int windows_position[24][2];
extern int index_color_user_to_affect;
extern int index_color_user_selected;
extern bool receive_from_trichro;
extern float couleurs_user[12][3];
extern char descriptif_colors_user_conf[32];
extern char tmp_time[24];
// variables globales d'affichage
extern char string_Last_Order[256];
extern char string_xy_mouse[12];
extern char string_display_fps[5];
extern char string_display_dmx_params[64];
extern char string_niveau[4];
extern char string_channel[3];
extern char string_function[8];
/////////////////////////
extern bool recall_windows_onoff[24];
extern int recall_windows_focus_id;
/////////////////////////
extern float facteur_scroll_fader_space;
extern int XGConfig;
extern int YGConfig;

// Visuels move window

extern bool index_click_move_trichro;
extern bool index_click_move_visualpad;

extern bool index_click_move_video;
extern bool index_click_move_patch;
extern bool index_click_move_time;
extern bool index_click_move_sequentiel;
extern bool index_click_move_artpollreply;
extern bool index_click_move_faderspace;

extern bool index_click_move_confirmwindow;
extern bool index_click_move_configbox;
extern bool index_clik_move_listproj;
extern bool index_click_move_save_menu;
extern bool index_click_move_savereportwindow;

extern bool index_click_move_banger_window;
extern bool index_click_move_cfg_window;
