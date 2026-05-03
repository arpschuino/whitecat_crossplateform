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

/**

* \file whitecat.h
* \brief {header file for all the global variable in whitecat}
* \author Christoph Guillermet
* \modified Jacques Bouault - arpschuino.fr - 2026
* \version {0.8.7.0}
* \date {15/05/2015}

 White Cat {- categorie} {- sous categorie {- sous categorie}}

*   Header qui contient toutes les variables globales déclarées dans whitecat
*
*   Header for all the global variables used in whitecat
*
*/

#include "Crossplateform.h"
#include "patch.h"
#include "audio.h"
#include "dmx.h"
#include "midi.h"
#include "network.h"
#include "chasers.h"

char versionis[72] = {"alpha 0.9.0 - 23 avril 2026"};
char nickname_version[48] = {"arpschuino reborn"};

bool init_done = 0; // démarrage pour éviter envoyer data pdt procedure d initialisation
/////////////////////REPERTOIRE/////////////////////////////////////////////////
char rep[255]; // repertoire avec arborescence complete du fichier
// sab 02/03/2014 char mondirectory[200];//endroit de l exe
/** Directory where is the executable file - Répertoire où se trouve l'exécutable **/
char mondirectory[512];
char working_nameis[120];
char rep_saves[10] = {"saves\\"};
char nomduspectacle[48] = {"last_save\\"};
char video_folder[256];
char my_show_is_coming_from[128] = {"default"};

bool there_is_change_on_show_save_state = 0;

int adapater_is_selected = -1; // numero d adaptateur selectionne pour adresse mac
bool right_click_for_menu = 0;

////////////////////////////////////////////////////////////////////////////////
int index_liste_kbd_custom = 0;
char kbd_custom[256][2];

/////////////////////////////////////////////////////////////////////////////////
enum WINDOW {
    W_SAVEREPORT = 900,
    W_DRAW = 901,
    W_TRICHROMY = 902,
    W_NUMPAD = 903,
    W_TRACKINGVIDEO = 904,
    W_ARTPOLLREPLY = 905,
    W_FADERS = 906,
    W_PATCH = 907,
    W_TIME = 908,
    W_SEQUENCIEL = 909,
    W_ASKCONFIRM = 911,
    W_PLOT = 912,
    W_ECHO = 913,
    W_LIST = 914,
    W_SAVE = 915,
    W_MAINMENU = 916,
    W_BANGER = 917,
    W_ALARM = 918,
    W_AUDIO = 919,
    W_CFGMENU = 920,
    W_WIZARD = 921,
    W_MINIFADERS = 922,
    W_CHASERS = 923,
    W_GRID = 926
};
int max_window_identity_is = 926;
int index_to_navigate_between_window = 0;

int window_opened[72]; // la liste des fenetres ouvertes modification en 0.4.1 d un size 64 en size 72,
// y compris dans save_show.cpp
int nbre_fenetre_actives = 0;
int temp_report_window[72];

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

// gel list + trichro -> trichro.h
#include "trichro.h"

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

// faders -> faders.h
#include "faders.h"
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
// trichro dock vars -> trichro.h
//////////////////////MEMOIRES//////////////////////////////////////////////////
int mem_to_resurrect = 0;
int CTRLC_mem_to_copy = 0;  // pour CTRL C CTRL V avec numeros
bool index_copy_mem_in = 0; // ask confirm

//////////////////////TEXT ENTRY////////////////////////////////////////////////
char string_numeric_entry[36];

int numeric_postext = 0;
float numeric_entry = 0.0;
char numeric[25];
int maxchar_numeric = 24;
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

////////////////////////VIDEO //////////////////////////////////////////////////
double image_recording_size;
double image_recorded_size;
int fps_video_rate = 30;
int default_fps_video_rate = 12;
int recup_val_pix_video = 0;
bool ocvfilter_is_on = 0; // acces reglages images oCV
int ocv_calcul_mode = 0;
char string_ocv_mode[8];
int threshold_level = 65;
int erode_level = 0;
int div_facteur = 1;
char string_threshold_is[24];
char string_erode_is[24];
char string_blur_is[24];
char string_div_is[24];
int flip_image = 1;
int threshold_on = 1;
int erode_mode = 0;
int blur_on = 0;
double pixels_changed = 0;
double old_pixels_changed = 0;
double nbre_pixels_changed = 0;
float ratio_pixels_changed = 0.0;

int camera_size_settings_is = 0; // nouveau système selection de taille image
int camera_size_array[2][2];     // x y size à définir 320x240 640x480
int camera_fps_settings_is = 0;  // nouveau systeme selection fps en index
bool manipulating_camera = 0;    // pour éviter de planter lectrue ecriture de données

int camera_modes_and_settings[8][16]; // ocv_calcul_mode /levels
float level_visu = 1.0;
int index_count_trackers = 0;
int frame_video_x, frame_video_y;
int video_size_x = 320, video_size_y = 200;
bool camera_is_on = 0;
int camera_on_open = 0;
int camera_original_fps_is = 15;
float display_fps;
// 6 tracking docks // 12 espaces de tracking par tracking dock//
int tracking_coordonates[6][12][4]; // dock selected / tracker / x y largeur x largeur y
bool tracking_contents[6][12][512]; // channel affectation
int buffer_tracker[514];
int tracker_level[6][12];
int tracker_to_edit = 0;
// smooth
float tracker_target_val[6][12];
float tracker_val[6][12];
float tracker_decay_constant = 1;
float tracker_dt = 1;
int index_decay_tracker = 10;
bool edit_tracker = 0; // size
bool move_tracker = 0; // x y
bool tracker_clearmode = 0;
bool view_levels_tracker = 1;
int videoX = 860, videoY = 20;
int default_videoX = 860, default_videoY = 20;
char string_tracker_edited_dat[48];
// tracking cam
// 6 tracking docks // 12 espaces de tracking par tracking dock//
int tracking_dock_selected = 0;
bool tracking_spaces_on_off[6][12]; // dock selected / tracker

////////////////////VIDEO AVI///////////////////////////////////////////////////
char list_my_video[25][16];   // 24 videos
char annote_my_video[25][64]; // 24 videos
int nbre_de_videos = 0;

///////////////////CHANNELS////////////////////////////////////////////////////

int XChannels = -20, YChannels = 70;
int ChScrollX = 580, ChScrollY = 50; // scroller ascenceur
float Ch_Scroll_Factor = 12.0;
int last_scroll_mouse_for_chan = 0;
bool index_moving_channel_scroller = 0;
char string_last_ch[36];

// Enttec Pro -> dmx.h

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
//////////////////////SAVE LOAD/////////////////////////////////////////////////
bool index_save_mode_export_or_binary = 0; // met la fenetre save en mode export=1 ou binary classic=0
int idf = 0;                               // curseur position retour info save load
bool specify_who_to_save_load[80];         // 72 d utiliser max
bool preset_specify_who_to_save_load[80][4];
bool index_save_global_is = 1; // 1 global 0 detailled
// coordonnees fenetre save load
int report_SL_X = 30;
int report_SL_Y = 40;
int default_report_SL_X = 30;
int default_report_SL_Y = 30;
bool there_is_an_error_on_save_load = 0;
bool index_show_save_load_report = 0;
bool b_report_error[256];
char string_save_load_report[256][64]; // 256 slot de report de 64 char
int position_view_line = 0;            // pour defilement affichage
char string_typeexport_view[24];
bool index_is_saving = 0; // pour eviter de planter les pointeurs en faisant deux saves simulatnnés
int timer_save_tmp = 0;
int temps_sauvegarde_tmp = 15 * 60; // secondes
bool index_please_do_not_save = 0;
/////////////SAVE MENU//////////////////////
// EXPORTS IMPORTS
int index_export_choice = 0;
char list_import_files[128][72];
int line_import = 0;
int importfile_selected = 0; // num de ligne selectionné
char importfile_name[72];
bool enable_export = 0; // pour les choix schwz / pdf / txt / ascii
bool enable_import = 0;
////BINARIES
char list_save_files[128][72];
int line_save = 0;
int savefile_selected = 0; // num de ligne selectionné
char savefile_name[72];

// sequenciel, time, memoires -> sequenciel.h
#include "sequenciel.h"
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

// couleurs et polices -> theme.h
#include "theme.h"
// import SCHWZ -> schwz.h
#include "schwz.h"

// Freeze -> dmx.h
// fader snapshots -> faders.h

// banger -> banger.h
#include "banger.h"
// Grand Master + DMX extra -> dmx.h

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

// wizard -> wizard.h
#include "wizard.h"

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

// arduino -> arduino.h
#include "arduino.h"

// MIDI launchpad + affect list -> midi.h

// Chasers variables -> chasers.h

bool index_false_shift = 0;
bool index_false_control = 0;
bool index_mouse_is_tracking = 0;
bool index_ask_clear_a_move = 0;
unsigned char buffer_moving_head[512];

int previous_mem_before_one = 0;
int mem_after_one = 0;
bool someone_changed_in_sequences = 0;
bool someone_changed_in_time_sequences = 0; // specifique affichage du temps de crossfade

int go_channel_is = 0;
int pause_channel_is = 0; // pause channel blink on pause in cuelist

bool index_monitor_key_esc = 0;

//////////////////

bool index_show_shortcuts = 1;
bool index_call_help = 0;

// grider -> grider.h
#include "grider.h"
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

// light plot -> plot.h
#include "plot.h"
///////////AFFICHAGES DIVERS CHANNEL SPACE/////////////////////////////////////

bool ClassicalChannelView = 1;
int ChannelXMenu = 0, ChannelYMenu = 0;
int hauteur_ChannelMenu = 30;
int largeur_ChannelMenu = 1180;
const int nbre_de_vues_circuits = 16;
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

// descriptif_network_adapter -> network.h

// draw -> draw.h
#include "draw.h"

// echo -> echo.h
#include "echo.h"
