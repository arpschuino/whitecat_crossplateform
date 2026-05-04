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

/////////////////////////////CHANNELS///////////////////////////////////////////
extern int scroll_channelspace;
extern int dmx_view;
extern unsigned char bufferSaisie[514];
extern unsigned char bufferBlind[514];
extern unsigned char bufferFaders[514];
extern unsigned char bufferSequenciel[514];
extern unsigned char bufferCopyPaste[514];
extern unsigned char buffer_affichage_valeurs_sequenciel[514];
extern unsigned char bufferPourcentStepdefaultlevel[514];

extern bool SelectedForCopyPaste[514];
extern bool Selected_Channel[514];
extern bool Temp_Selected_Channel[514];
extern int last_ch_selected;
extern int check_channel_level;
extern int previous_ch_selected;
extern bool index_blind;
extern char string_last_over_dock[36];
extern char string_secondary_feeback[64];
extern bool index_ch_thruth;
extern int default_step_level;
extern bool index_level_attribue;

///////////////////CHANNELS////////////////////////////////////////////////////

extern int XChannels, YChannels;
extern int ChScrollX, ChScrollY;
extern float Ch_Scroll_Factor;
extern int last_scroll_mouse_for_chan;
extern bool index_moving_channel_scroller;
extern char string_last_ch[36];

///////////AFFICHAGES DIVERS CHANNEL SPACE/////////////////////////////////////

extern bool ClassicalChannelView;
extern int ChannelXMenu, ChannelYMenu;
extern int hauteur_ChannelMenu;
extern int largeur_ChannelMenu;
const int nbre_de_vues_circuits = 16;
extern int hauteur_preset_titre;

extern bool Channel_View_MODE[nbre_de_vues_circuits];
extern int Channel_View_ROUTING[nbre_de_vues_circuits][514];
extern int channel_view_is;

extern char channel_view_Name[nbre_de_vues_circuits][25];
extern int channel_view_link_type[nbre_de_vues_circuits];
extern int channel_view_link_ref[nbre_de_vues_circuits];

extern int channel_view_type_of_behaviour[nbre_de_vues_circuits];
extern bool channel_view_mode_builder[nbre_de_vues_circuits];

extern int channel_number_in_View[nbre_de_vues_circuits];
extern int channel_number_of_lines[nbre_de_vues_circuits];
extern bool index_ask_record_selection_of_view;
extern bool index_ask_modify_selection_of_view;
extern bool index_ask_clear_selection_of_view;
extern bool index_ask_report_selection_of_view;

extern bool index_ask_build_view;
extern bool index_do_build_view;

extern int total_pixel_vision;
extern float ratioview;

extern int scroll_pos_preset[nbre_de_vues_circuits];

extern bool index_show_main_menu;
extern int x_mainmenu, y_mainmenu;
extern int size_x_mainmenu;
extern int size_y_mainmenu;

extern int expert_mode;
extern bool index_show_first_dim;
extern int show_first_dim_array[514][4];
extern bool show_more_than_one_dim[514];
