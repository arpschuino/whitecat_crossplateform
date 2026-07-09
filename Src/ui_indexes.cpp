#include "ui_indexes.h"

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
////////////////////RETOUR INFOS////////////////////////////////////////////////
char string_debug[120];
int scroll_y_info = 0;
////////////////////APPELS/ MENUS////////////////////////////////////////////
bool index_quit = 0;
bool index_main_function = 1;
bool index_do_dock = 0;   // do dock
bool index_do_report = 0; // report de tout sur scène dans un master a full
bool index_do_modify = 0; // des circuits selectionnés only. ne detruit pas le reste
bool index_main_clear = 0;
bool index_type = 0;
bool index_inspekt = 0;        // voir les etiquettes et qui controle qui
bool index_trichro_window = 0; // indicateur affichage menu trichro
bool index_visual_pad = 0;
bool index_video_window = 0;
bool index_patch_window = 0;
bool index_do_quick_save = 0;
bool index_show_faders = 0;
//////////////////////OVER WINDOW////////////////////////////////////////////////
bool index_over_A_window = 0;
bool index_over_faderspace = 0;
bool index_over_channelspace = 0;
bool index_over_function_call =
    0; // pour eviter de clicker dans les faders quand on est en zone functions ( boutons du menu gauche)
// DMX core -> dmx.h
//////////////////////MEMOIRES//////////////////////////////////////////////////
int mem_to_resurrect = 0;
int CTRLC_mem_to_copy = 0;  // pour CTRL C CTRL V avec numeros
bool index_copy_mem_in = 0; // ask confirm
//////////////////////TEXT ENTRY////////////////////////////////////////////////
char string_numeric_entry[110];
int numeric_postext = 0;
int numeric_cursor = 0;
float numeric_entry = 0.0;
char numeric[100];
int maxchar_numeric = 96;
int idchannel;
//////////////////////UTILITIES///////////////////////////////////////
int xBoxutilities = 580;
int yBoxutilities = 290;
/////////////////CONFIG//////////////////////////////////////////////////
char string_config[15];
char string_title_panel_config[36];
bool index_save_config = 0;
// artnet
bool index_artnet_receiver = 0;
bool index_art_polling = 0;
char tmp_ip_artnet[17];
// dmx
int Survol_interface_numero = 0;
int window_cfgX = 10;
int window_cfgY = 25;
int largeurCFGwindow = 830;
int hauteurCFGwindow = 299;
// config pannel
int select_artnet_to_listen = 0;
bool index_do_affect_net_to_dock = 0; // affecter ou pas dans le dock le type de net
// POLL
volatile int ticks_poll = 0;
int window_proc_x = 300, window_proc_y = 150;
// globales
bool index_affect_dmxin = 0;
bool index_affect_video_tracking_to_dock = 0;
bool index_snap_background = 0;
// CONFIGS
bool index_config_dmx = 0;
bool index_config_midi = 0;
bool index_config_arduino = 0;
bool index_setup_gfx = 0;
bool index_config_network = 0;
bool index_config_general = 0; // main general du setup // 1 car ouverture à l allumage
// patch variables -> patch.h
///////////////////////NUMERIC PAD//////////////////////////////////////////////
int xnum_window = 900, ynum_window = 60;
int default_xnum_window = 900, default_ynum_window = 60; // xy de la window par defaut
int rayon_wheel_level = 20;
float angle_correction_wheel = 1.0; // faire un quart de toour vers la droite
float pad_vx;
float pad_vy;
float angle_snap_pad;
float position_curseur_pad_x;
float position_curseur_pad_y;
int wheellevel_absolutemode = 0;
int previous_level_wheel = 1;
int absolute_level_wheel = 0;
int previous_absolute_level_wheel = 0;
////////////////////////TIME CHRONO///////////////////////////////////////////
float position_curseur_time_x; //=1242,
float position_curseur_time_y; //=228;//pour intialisation au demarage de l api
float time_angle;
float angle_timesnap;
float vtimex, vtimey;
int actual_tickers_chrono = 0;
char visu_chrono_str[64];
char string_tap_tempo_average[36];
char string_actual_tap_tempo[36];
bool index_recording_tap_tempo = 0;
bool do_light_tap_tempo = 0; // index affcihage frappe tempo
bool do_light_send_tap = 0;
int start_tempo_ticks = 0;
float tap_tempo_average = 0.0;
int nbr_steps_tempo = 1; // doit etre 1 et pas 0 pour pas provoquer de NIL
int max_temp_tempo = 12;
int ticks_tap_tempo[16];
int tempo_interm = 0; // pur addition des ticks
// trichro window vars -> trichro.h
/////////////////////functions call//////////////////////////////////////////
char string_name_button[15];
char string_raccourci[5];
////////////////////////////////////////////////////////////////////////////////
// MENU SAVE
bool index_menu_save = 0;
int xsave_window = 200;
int ysave_window = 200;
///////////////////Confirm box//////////////////////////////////////////////////
int XConfirm = 100;
int YConfirm = 100;
bool index_ask_confirm = 0;
bool previous_index_ask_confirm = 0;
char string_confirmation[128];
bool index_do_delete_mem = 0;
bool index_do_create_mem = 0;
bool index_do_create_mem_plus_faders = 0;    // shift f3
bool index_do_overecord_mem_plus_faders = 0; // ctrl f3
bool index_do_link_memonstage = 0;
bool index_do_link_memonpreset = 0;
bool index_do_link_membefore = 0;
bool index_do_link_memother = 0;
bool index_do_record_on_faders = 0;
bool index_do_modify_on_faders = 0;
bool index_do_report_on_faders = 0;
bool index_do_affect_color_on_faders = 0;
bool index_do_dmx_to_dock = 0;
bool index_do_artnet_to_dock = 0;
bool index_do_video_to_dock = 0;
int fader_selected_for_record = 0;
int dock_selected_for_record = 0;
bool index_do_affect_color_trichro = 0;
int couleur_to_affect = 0;
bool index_do_affect_roi = 0;
bool index_do_clear_my_roi = 0;
bool index_do_clear_my_video_preset = 0;
int dock_roi_selected = 0;
int tracking_dock_to_clean = 0;
bool index_do_overrecord_mem = 0;
bool index_do_clear_dock = 0;
bool index_do_clear_on_faders = 0;
bool index_do_reload_mem = 0;
bool index_do_resurrect_mem = 0;
bool index_do_jump_while_cross = 0;
bool index_do_export = 0;
bool index_do_import = 0;
bool index_do_saveshow = 0;
bool index_do_loadshow = 0;
bool index_do_resetshow = 0;
bool index_do_freeze = 0;
bool index_do_clear_patch = 0;
bool index_do_default_patch = 0;
bool index_do_quit_with_save = 0;
bool index_do_quit_without_save = 0;
bool index_do_ask_call_audio_folder = 0;
bool index_do_clear_lock_preset = 0;
bool index_do_banger_memonstage = 0;
bool index_do_banger_memonpreset = 0;
bool index_do_banger_membeforeone = 0;
bool index_do_banger_memother = 0; // les 8 autres memoires
// liste projos -> patch.h
// ArtNet + network -> network.h
// audio variables -> audio.h
bool starting_wcat = 0; // démarrage
int dpi_native_rendering = 1;  // 1 = DPI natif (net, taille réduite) — lu avant SDL_Init
float wc_dpi_scale = 1.0f;    // facteur DPI systeme (ex. 1.25 a 125%) — calcule dans main()
////////////////CONFIG////////////////////////////////////////
bool index_show_config_window = 0;
int config_page_is = 0;
/////////////FOCUS WINDOW////////////////////////////////////
int window_focus_id = 0;          // 900+id fenetre
int previous_window_focus_id = 0; // pour le confirm
int pos_focus_window = 0;
bool im_moving_a_window = 0; // index poru lros de déplacement
int last_window_founded = 0;
/////////////keyboard affectation//////////////////////////////////////////////
int scan_ascii_is = 0;
int scan_allegro_key_is = 0;
char string_clavier_is[72];
char list_keyname[128][16]; // nom des fonction
char string_key_id[16];
int mapping_temporaire[128]; // la table de reroutage
int nbre_key_persos = 5;
// MIDI change signal -> midi.h
////////////sauvegardes automatiques//////////////////////////////////////////
int automatic_time_for_save = 10;
int do_save_at_time = 10000;
int last_time_for_save_was = 0;
/////////////MULTI CORE PROCESS/////////////////
bool index_allow_multicore = 0;
int core_to_assign = 0;
bool index_false_shift = 0;
bool index_false_control = 0;
bool index_mouse_is_tracking = 0;
bool index_ask_clear_a_move = 0;
unsigned char buffer_moving_head[512];
int previous_mem_before_one = 0;
int mem_after_one = 0;
bool someone_changed_in_time_sequences = 0; // specifique affichage du temps de crossfade
int go_channel_is = 0;
int pause_channel_is = 0; // pause channel blink on pause in cuelist
bool index_monitor_key_esc = 0;
//////////////////
bool index_show_shortcuts = 1;
bool index_call_help = 0;
///////////// CORE PROCESSS ///////////////
bool index_config_core = 0;
int core_user_define_nb_faders = 48;
int core_user_define_nb_docks = 6;
int core_user_define_nb_chasers = 128;
int core_user_define_nb_tracks_per_chasers = 24;
int core_user_define_nb_gridplayers = 4;
int core_user_define_nb_bangers = 128;
int core_do_calculations[24];
//////////////////////////////////////
int mouse_level_for_banger = 0;
int mouse_level_for_event = 0;
/////////windows list//////////////////
int window_order[60];
int prev_window_focus_id = 0;
// exclusion de circuits
bool Channels_excluded_from_grand_master[514];
bool index_do_exclude = 0;
bool Midi_Force_Go = 0;
// trichro
bool index_paste_on_the_fly = 0;
// highpass / prio
bool index_do_hipass = 0;
int FaderManipulating = 0;
// nouvelle structure de groupe de faders
int fader_mode_with_buffers[48]; // 0 NORMAL / 1 OFF RENDERING / 2 SUBSTRACT / 3 ADDITIF / 4 SCREEN / EXCLUSION
bool fader_fx_route[48];         /// 1 Buffer séquenciel / 0 Buffer faders
bool channel_is_touched_by_fader_fx[513];
int channel_is_touched_by_fader_number[513];
int channel_is_touched_by_fader_type_fx[513];
int channel_level_mofification_while_crossfade[513]; // correcteur en crossfade
int over_banger_event = 0;
int over_banger_action = 0;
bool over_family = 0; // 0 event 1 action
bool index_do_fgroup = 0; // assignation de groups de faders record dock typ 13
bool index_do_affect_fgroup = 0;
bool fgroup[48][6][48]; // fader dock isingroupaffected=1
int List_view_nbre_channels = 17;
int nbre_macros_per_channel = 4;
bool macro_channel_on[514][4];
int channel_macro_reaction[514][4];
int channel_macro_action[514][4];
int channel_macro_val[514][4][2];
int max_channel_macro_reaction = 8;
int max_channel_macro_action = 17;
unsigned char previous_state_of_outputted_channels[514];
int chan_to_manipulate = 0;

char confirm_name_buf[50] = {0};
int confirm_name_len = 0;
bool index_confirm_name_active = 0;
