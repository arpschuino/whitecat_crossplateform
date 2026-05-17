#include "channels.h"

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
/////////////////////////////CHANNELS///////////////////////////////////////////
int scroll_channelspace = 0;
int dmx_view = 0;                // 0 pourcentage 1 dmx
unsigned char bufferSaisie[514]; // le 0 n est pas pris, le 512 est en  513
unsigned char bufferBlind[514];
unsigned char bufferFaders[514];
unsigned char bufferSequenciel[514];
unsigned char bufferCopyPaste[514];
unsigned char buffer_affichage_valeurs_sequenciel[514];
unsigned char bufferPourcentStepdefaultlevel[514]; // buffer tampon pour simplifier les steps levels en % ou dmx
bool SelectedForCopyPaste[514];
bool Selected_Channel[514];
bool Temp_Selected_Channel[514]; // pour selection chainee souris ( comme ds schwz)
int last_ch_selected = 0;
int check_channel_level = 255;
int previous_ch_selected = 0;
bool index_blind = 0;
char string_last_over_dock[36];
char string_secondary_feeback[64];
bool index_ch_thruth = 0;
int default_step_level = 1;
bool index_level_attribue = 0; // pour deselection lors d une resaisie nouvelle
///////////////////CHANNELS////////////////////////////////////////////////////
int XChannels = -20, YChannels = 70;
int ChScrollX = 580, ChScrollY = 50; // scroller ascenceur
float Ch_Scroll_Factor = 12.0;
int last_scroll_mouse_for_chan = 0;
bool index_moving_channel_scroller = 0;
char string_last_ch[36];
///////////AFFICHAGES DIVERS CHANNEL SPACE/////////////////////////////////////
bool ClassicalChannelView = 1;
int ChannelXMenu = 0, ChannelYMenu = 0;
int hauteur_ChannelMenu = 53;
int largeur_ChannelMenu = 1180;
int hauteur_preset_titre = 30;
bool Channel_View_MODE[nbre_de_vues_circuits]; // les 16 indexes de vision
int Channel_View_ROUTING[nbre_de_vues_circuits][514];
int channel_view_is = 0; // 0 pas touché car sortie du patch
char channel_view_Name[nbre_de_vues_circuits][25];
int channel_view_link_type[nbre_de_vues_circuits]; // 0:no link 1:memory 2: fader
int channel_view_link_ref[nbre_de_vues_circuits];  // num de mem ou de fader. desaffectation=-1;
int channel_view_type_of_behaviour[nbre_de_vues_circuits]; // 0 normal view // 1 mem // 2 fader
bool channel_view_mode_builder[nbre_de_vues_circuits];     // 0 solo // 1 ALL
int channel_number_in_View[nbre_de_vues_circuits];
int channel_number_of_lines[nbre_de_vues_circuits];
bool index_ask_record_selection_of_view = 0;
bool index_ask_modify_selection_of_view = 0;
bool index_ask_clear_selection_of_view = 0;
bool index_ask_report_selection_of_view = 0;
bool index_ask_build_view = 0;
bool index_do_build_view = 0;
int total_pixel_vision = 0;
float ratioview = 270.0 / total_pixel_vision;
int scroll_pos_preset[nbre_de_vues_circuits]; // affichage vision
bool index_show_main_menu = 0; // main menu par click droit
int x_mainmenu = 0, y_mainmenu = 0;
int size_x_mainmenu = 365;
int size_y_mainmenu = 200;
int expert_mode = 0; // confirmations ou pas de record etc...
bool index_show_first_dim = 0;
int show_first_dim_array[514][4]; // pour affichage du premier grada patché au circuit
bool show_more_than_one_dim[514];
