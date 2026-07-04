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

////////////////////RETOUR INFOS////////////////////////////////////////////////
extern char string_debug[120];
extern int scroll_y_info;

////////////////////APPELS/ MENUS////////////////////////////////////////////
extern bool index_quit;
extern bool index_main_function;

extern bool index_do_dock;
extern bool index_do_report;
extern bool index_do_modify;
extern bool index_main_clear;
extern bool index_type;
extern bool index_inspekt;
extern bool index_trichro_window;
extern bool index_visual_pad;
extern bool index_video_window;
extern bool index_patch_window;
extern bool index_do_quick_save;
extern bool index_show_faders;
//////////////////////OVER WINDOW////////////////////////////////////////////////
extern bool index_over_A_window;
extern bool index_over_faderspace;
extern bool index_over_channelspace;
extern bool index_over_function_call; // pour eviter de clicker dans les faders quand on est en zone functions

// DMX core -> dmx.h

//////////////////////MEMOIRES//////////////////////////////////////////////////
extern int mem_to_resurrect;
extern int CTRLC_mem_to_copy;
extern bool index_copy_mem_in;

//////////////////////TEXT ENTRY////////////////////////////////////////////////
extern char string_numeric_entry[110];

extern int numeric_postext;
extern int numeric_cursor;
extern float numeric_entry;
extern char numeric[100];
extern int maxchar_numeric;
extern int idchannel;

//////////////////////UTILITIES///////////////////////////////////////
extern int xBoxutilities;
extern int yBoxutilities;

/////////////////CONFIG//////////////////////////////////////////////////
extern char string_config[15];
extern char string_title_panel_config[36];
extern bool index_save_config;
// artnet
extern bool index_artnet_receiver;

extern bool index_art_polling;
extern char tmp_ip_artnet[17];

// dmx
extern int Survol_interface_numero;
extern int window_cfgX;
extern int window_cfgY;
extern int largeurCFGwindow;
extern int hauteurCFGwindow;
// config pannel
extern int select_artnet_to_listen;
extern bool index_do_affect_net_to_dock;
// POLL
extern volatile int ticks_poll;
extern int window_proc_x, window_proc_y;
// globales
extern bool index_affect_dmxin;
extern bool index_affect_video_tracking_to_dock;
extern bool index_snap_background;
// CONFIGS
extern bool index_config_dmx;
extern bool index_config_midi;
extern bool index_config_arduino;
extern bool index_setup_gfx;
extern bool index_config_network;
extern bool index_config_general;
// patch variables -> patch.h

///////////////////////NUMERIC PAD//////////////////////////////////////////////
extern int xnum_window, ynum_window;
extern int default_xnum_window, default_ynum_window;
extern int rayon_wheel_level;
extern float angle_correction_wheel;
extern float pad_vx;
extern float pad_vy;
extern float angle_snap_pad;
extern float position_curseur_pad_x;
extern float position_curseur_pad_y;
extern int wheellevel_absolutemode;
extern int previous_level_wheel;
extern int absolute_level_wheel;
extern int previous_absolute_level_wheel;

////////////////////////TIME CHRONO///////////////////////////////////////////

extern float position_curseur_time_x;
extern float position_curseur_time_y;
extern float time_angle;
extern float angle_timesnap;
extern float vtimex, vtimey;
extern int actual_tickers_chrono;
extern char visu_chrono_str[64];

extern char string_tap_tempo_average[36];
extern char string_actual_tap_tempo[36];
extern bool index_recording_tap_tempo;
extern bool do_light_tap_tempo;
extern bool do_light_send_tap;
extern int start_tempo_ticks;
extern float tap_tempo_average;
extern int nbr_steps_tempo;
extern int max_temp_tempo;
extern int ticks_tap_tempo[16];
extern int tempo_interm;
// trichro window vars -> trichro.h

/////////////////////functions call//////////////////////////////////////////
extern char string_name_button[15];
extern char string_raccourci[5];

////////////////////////////////////////////////////////////////////////////////
// MENU SAVE
extern bool index_menu_save;
extern int xsave_window;
extern int ysave_window;
///////////////////Confirm box//////////////////////////////////////////////////
extern int XConfirm;
extern int YConfirm;
extern bool index_ask_confirm;
extern bool previous_index_ask_confirm;
extern char string_confirmation[128];

extern bool index_do_delete_mem;
extern bool index_do_create_mem;
extern bool index_do_create_mem_plus_faders;
extern bool index_do_overecord_mem_plus_faders;
extern bool index_do_link_memonstage;
extern bool index_do_link_memonpreset;
extern bool index_do_link_membefore;
extern bool index_do_link_memother;
extern bool index_do_record_on_faders;
extern bool index_do_modify_on_faders;
extern bool index_do_report_on_faders;
extern bool index_do_affect_color_on_faders;
extern bool index_do_dmx_to_dock;
extern bool index_do_artnet_to_dock;
extern bool index_do_video_to_dock;
extern int fader_selected_for_record;
extern int dock_selected_for_record;
extern bool index_do_affect_color_trichro;
extern int couleur_to_affect;
extern bool index_do_affect_roi;
extern bool index_do_clear_my_roi;
extern bool index_do_clear_my_video_preset;
extern int dock_roi_selected;
extern int tracking_dock_to_clean;
extern bool index_do_overrecord_mem;
extern bool index_do_clear_dock;
extern bool index_do_clear_on_faders;
extern bool index_do_reload_mem;
extern bool index_do_resurrect_mem;
extern bool index_do_jump_while_cross;
extern bool index_do_export;
extern bool index_do_import;
extern bool index_do_saveshow;
extern bool index_do_loadshow;
extern bool index_do_resetshow;
extern bool index_do_freeze;

extern bool index_do_clear_patch;
extern bool index_do_default_patch;
extern bool index_do_quit_with_save;
extern bool index_do_quit_without_save;
extern bool index_do_ask_call_audio_folder;
extern bool index_do_clear_lock_preset;
extern bool index_do_banger_memonstage;
extern bool index_do_banger_memonpreset;
extern bool index_do_banger_membeforeone;
extern bool index_do_banger_memother;
// liste projos -> patch.h
// ArtNet + network -> network.h

// audio variables -> audio.h
extern bool starting_wcat;
extern int dpi_native_rendering;
extern float wc_dpi_scale;
////////////////CONFIG////////////////////////////////////////
extern bool index_show_config_window;
extern int config_page_is;
/////////////FOCUS WINDOW////////////////////////////////////
extern int window_focus_id;
extern int previous_window_focus_id;

extern int pos_focus_window;

extern bool im_moving_a_window;
extern int last_window_founded;

/////////////keyboard affectation//////////////////////////////////////////////
extern int scan_ascii_is;
extern int scan_allegro_key_is;
extern char string_clavier_is[72];
extern char list_keyname[128][16];
extern char string_key_id[16];
extern int mapping_temporaire[128];
extern int nbre_key_persos;
// MIDI change signal -> midi.h
////////////sauvegardes automatiques//////////////////////////////////////////
extern int automatic_time_for_save;
extern int do_save_at_time;
extern int last_time_for_save_was;

/////////////MULTI CORE PROCESS/////////////////
extern bool index_allow_multicore;
extern int core_to_assign;

extern bool index_false_shift;
extern bool index_false_control;
extern bool index_mouse_is_tracking;
extern bool index_ask_clear_a_move;
extern unsigned char buffer_moving_head[512];

extern int previous_mem_before_one;
extern int mem_after_one;
extern bool someone_changed_in_sequences;
extern bool someone_changed_in_time_sequences;

extern int go_channel_is;
extern int pause_channel_is;

extern bool index_monitor_key_esc;

//////////////////

extern bool index_show_shortcuts;
extern bool index_call_help;

///////////// CORE PROCESSS ///////////////
extern bool index_config_core;
extern int core_user_define_nb_faders;
extern int core_user_define_nb_docks;
extern int core_user_define_nb_chasers;
extern int core_user_define_nb_tracks_per_chasers;
extern int core_user_define_nb_gridplayers;
extern int core_user_define_nb_bangers;
extern int core_do_calculations[24];

//////////////////////////////////////
extern int mouse_level_for_banger;
extern int mouse_level_for_event;
/////////windows list//////////////////
extern int window_order[60];
extern int prev_window_focus_id;

// exclusion de circuits
extern bool Channels_excluded_from_grand_master[514];
extern bool index_do_exclude;

extern bool Midi_Force_Go;

// trichro
extern bool index_paste_on_the_fly;

// highpass / prio
extern bool index_do_hipass;
extern int FaderManipulating;

// nouvelle structure de groupe de faders
extern int fader_mode_with_buffers[48];
extern bool fader_fx_route[48];
extern bool channel_is_touched_by_fader_fx[513];
extern int channel_is_touched_by_fader_number[513];
extern int channel_is_touched_by_fader_type_fx[513];

extern int channel_level_mofification_while_crossfade[513];

extern int over_banger_event;
extern int over_banger_action;
extern bool over_family;

extern bool index_do_fgroup;
extern bool index_do_affect_fgroup;
extern bool fgroup[48][6][48];

extern int List_view_nbre_channels;
extern int nbre_macros_per_channel;
extern bool macro_channel_on[514][4];
extern int channel_macro_reaction[514][4];
extern int channel_macro_action[514][4];
extern int channel_macro_val[514][4][2];
extern int max_channel_macro_reaction;
extern int max_channel_macro_action;
extern unsigned char previous_state_of_outputted_channels[514];
extern int chan_to_manipulate;

// Champ nom dans la boite de confirmation (création mémoire)
extern char confirm_name_buf[50];
extern int confirm_name_len;
extern bool index_confirm_name_active;
extern bool savename_editing; // [save inline] edition inline du nom de show
