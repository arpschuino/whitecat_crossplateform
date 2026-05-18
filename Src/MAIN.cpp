/*-------------------------------------------------------------------------------------------------------------
                                 |
          CWWWWWWWW              | Copyright (C) 2009-2013  Christoph Guillermet
                                 |               2026       Jacques Bouault - arpschuino.fr
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

* \file MAIN_janv_2014_opensource_kbd.cpp
* \brief {main loop}
* \author Christoph Guillermet
* \modified Jacques Bouault - arpschuino.fr - 2026
* \version {0.9.0}
* \date {2026}

 White Cat {- categorie} {- sous categorie {- sous categorie}}

*   Main loop avec include des libraries et des différents fichiers C++ de whitecat. Whitecat est en fait codé en C.
*
*   Main loop with include of librairies and the C++ files used in whitecat. Whitecat is in fact coded in C.
*
 **/

// pour whitecat solo
//  --- Migration SDL2 (2026) : Allegro4 + OpenLayer + Audiere → SDL2 ---
#include "graphics_backend.h" // remplace allegro.h + winalleg.h + OpenLayer.hpp
#include "audio_backend.h"    // remplace audiere.h

#include <stdio.h>
#include <assert.h>
#include <Iphlpapi.h>

#include <vector>

#include "SmoothData.h"  // classe compilee separement dans SmoothData.cpp

#include "midi_backend.h"
// création du damper sur les faders
std::vector<SmoothData> Fader_dampered(48);

using namespace audiere;
using namespace ol;

int BPS_RATE = 50; // devient dmx rate
int dmxINrate = 25;
int ARDUINO_RATE = 20;

#define PIknob 3.14159265358979323846264338327950288419716939937510

// ticks
volatile int ticks = 0;
int ticker_rate = BPS_TO_TIMER(BPS_RATE);
int ticker_dmxIn_rate = BPS_TO_TIMER(dmxINrate);
int midi_BPM = 120;
int ticks_passed = 0;
volatile int mouse_maintained;
volatile int mouse_R_button;
volatile int mouse_R_released;

volatile bool calculation_on_faders_done = 0; // pour snap des faders depuis Echo
//////////////////////////////////////////////////////////////////////////////
#define PI 3.14116 // pour ok trichro
// #define PI  3.14159265358979323846264338327950288419716939937510
//////////////////////////////////////////////////////////////////////////////

#include <hpdf.h>
// #include <MidiShare.h>
#include <whitecat.h>

volatile int ticker_midi_clock_rate = BPM_TO_TIMER(24 * midi_BPM);
/*
      SECS_TO_TIMER(secs)  - give the number of seconds between
                             each tick
      MSEC_TO_TIMER(msec)  - give the number of milliseconds
                             between ticks
      BPS_TO_TIMER(bps)    - give the number of ticks each second
      BPM_TO_TIMER(bpm)    - give the number of ticks per minute
24 * 120 MIDI Clocks per minute. So, each MIDI Clock is sent at a rate of 60,000,000/(24 * 120) microseconds).
So, each MIDI Clock is sent at a rate of 60,000,000/(24 * BPM) microseconds).
*/

void ticker_midi_clock() {

    if (index_midi_clock_on == 1) {
        midi_backend_send_system(0xF8); // MIDI Clock
    }
}
END_OF_FUNCTION(ticker_midi_clock);

#include "patch_splines.h"  // compile separement dans patch_splines.cpp

#include "grider_calcul.h"

#include "midi_CORE.h"  // compile separement dans midi_CORE.cpp

#include "core.h"  // compile separement dans core.cpp

#include "saves_export_pdf2.h"
#include "saves_export_import.h"
#include "saves_menu.h"
#include "gestionaire_fenetres2.h"

#include "gui_boutons_rebuild1.h"
#include "grider_core.h"
#include "wizard_operations.h"

#include "faders_operations.h"
#include "chasers_core.h"

#include "plot_core9.cpp"
#include "plot9.cpp"

#include <audio_core.cpp>

#include "save_show.h"
#include "network_artnet_3.h" //artnet functions

#include "logicals_intres.h"

#include "channels_core.h"
#include "wizard.h"
#include "grand_master.h"
#include "faders_core.h"

#include <audio_visu.cpp>

#include "arduino_device_core.h"
#include "banger_core.h"
#include "banger_visu.h"
#include "Call_everybody_5.h"

#include "patch_core.h"
#include "patch_visu.h"
#include "time_core.h"
#include "time_visu_3.h"
#include "trichro_core.h"
#include "trichro_visu2.h"
#include "numpad_core.h"
#include "numpad_visuel.h"
#include "keyboard_functions2.h"
#include <keyboard_routines2.cpp>
#include "minifaders_core.h"
#include "minifaders_visu.h"
#include "faders_visuels.h"
#include "channels_visu.h"
#include "video_tracking_core.h"
#include "video_tracking_visu.h"

#include "list_proj_core.h"
#include "list_proj_visu.h" //liste projecteurs

#include "sequentiel_core.h"
#include "network_MAC_adress_3.h"
#include "midi_launchpad.h"

#include "grider_visu.h"
#include "sequentiel_visu.h"
#include "Draw3.h"
#include "echo3.h"

#include "procs_visuels_rebuild1.h"
#include "dmx_functions.h"

#include "midi_13.h"
#include "CFG_screen.h"

#include "arduino_core.h"
#include "arduino_visu.h"

int time_doing() {
    if (index_play_chrono == 1) {
        ++actual_tickers_chrono;
    }
    ++ticks;
    actual_time++;
    alpha_blinker += 0.05;
    alpha_smooth_blinker += 0.001;
    alpha_blinker_slow += 0.008f;
    if (alpha_blinker > 1) {
        alpha_blinker = 0.2;
    }
    if (alpha_smooth_blinker > 1) {
        alpha_smooth_blinker = 0.0;
    }
    if (alpha_blinker_slow > 1.0f) {
        alpha_blinker_slow = 0.2f;
    }
    return (0);
}
/////////////////TIMER POUR DATA ET REFRESH RATE////////////////////////////////
void ticker_dmxIn() // nettoyage des ticker pour verifier stabilité
{
    if (starting_wcat)
        return;
    Receive_DMX_IN();
}
END_OF_FUNCTION(ticker_dmxIn);

/////////////////////////////////////////////

void ticker() {
    time_doing();

    if (index_is_saving == 0 && init_done == 1 && index_writing_curve == 0 && index_quit == 0) {
        for (int i = 0; i < 9; i++) {
            if (core_do_calculations[i] == 1) {
                switch (i) {
                case 0:
                    detect_actual_master_lock_is();
                    do_lfos();
                    break;
                case 1:
                    do_crossfade();
                    if (index_go == 1) {
                        index_crossfading = 1;
                    } else {
                        index_crossfading = 0;
                    }
                    do_goback();
                    break;
                case 2:
                    // for (int i=0;i<core_user_define_nb_bangers;i++){do_bang(i);}// dans full loop
                    break;
                case 3:
                    // trichro_back_buffer(315/2,550/2,125,15);//calcul trichro ( triangle et saturation dans buffer
                    // separé)
                    break; // obligé dans main loop
                case 4:

                    if (manipulating_camera == 0)
                        ventilation_video_trackers();
                    break;
                case 5:
                    do_chaser();
                    break;
                case 6:
                    do_grid();
                    break;
                case 7:
                    do_channels_macro();
                    break;
                case 8: // echo
                    do_echo();
                    break;
                default:
                    break;
                }
            }
        }

        // wc_request_refresh() uniquement si une animation est réellement en cours.
        // Évite de forcer 25fps quand tout est statique (automation "enabled" mais rien ne bouge).
        // — crossfade / go-back en cours, OU vient juste de se terminer (frame final).
        {
            static bool prev_go = false;
            static bool prev_go_back = false;
            bool go_just_ended = (prev_go && !index_go) || (prev_go_back && !index_go_back);
            if (index_go || index_go_back || go_just_ended) {
                wc_request_refresh();
            }
            prev_go      = (bool)index_go;
            prev_go_back = (bool)index_go_back;
        }
        // — au moins un LFO actif sur un fader (up/down ou cyclique)
        if (core_do_calculations[0]) {
            for (int i = 0; i < 48; i++) {
                if (lfo_mode_is[i] != 0 || lfo_cycle_is_on[i] != 0) {
                    wc_request_refresh();
                    break;
                }
            }
        }
        // — au moins un chaser en lecture
        if (core_do_calculations[5]) {
            for (int i = 0; i < core_user_define_nb_chasers; i++) {
                if (chaser_is_playing[i]) { wc_request_refresh(); break; }
            }
        }

        for (int pr = 0; pr < 6; pr++) {
            if (draw_point_is_traced[pr] == 1)
                wc_request_refresh();
            merge_draw_and_grid_player(pr);
        }

        // — blinker : wc_blink_needed → Canvas::Refresh réduit le timeout à 40ms (25fps)
        // et pose wc_dirty=true pour que alpha_blinker continue d'animer en idle.
        // Seuls les vrais popups/dialogs sont dans la whitelist (les fenêtres workspace
        // comme W_FADERS, W_CFGMENU... ne blinkent pas : pas besoin de les réveiller).
        {
            bool has_popup = false;
            for (int _wi = 0; _wi < 72 && window_opened[_wi] != 0; _wi++) {
                int _w = window_opened[_wi];
                if (_w == W_ASKCONFIRM || _w == W_MAINMENU || _w == W_NUMPAD ||
                    _w == W_SAVE || _w == W_ALARM) {
                    has_popup = true; break;
                }
            }
            wc_blink_needed = (index_false_shift != 0) || (index_false_control != 0) || has_popup;
        }

        // damper of faders
        for (int i = 0; i < 48; i++) {
            if (fader_damper_is_on[i] == 1) {
                Fader_dampered[i].damper();
                Fader[i] = Fader_dampered[i].getdmxvalue_dampered();
                midi_levels[i] = ((Fader_dampered[i].getdmxvalue_dampered()) / 2);
                index_send_midi_out[1960 + i] = 1; //???
                if (Fader_dampered[i].calculating() == 1) {
                    index_fader_is_manipulated[i] = 1;
                    wc_request_refresh();
                } // direct chan
            }
        }

        //
        Merger();
        SendData_to_interface();
        ventilation_midi_sur_crossfade();
        emit_midi_out();
        refresh_chaser_window_timeline_for_midi_out();
        if (enable_launchpad == 1 && entered_main == 1 && index_quit == 0 && index_is_saving == 0) {
            launchpad_refresh_buffer_led();
        }

        do_autolaunch(); // attention contient rafraichissement Faders
    }
}

END_OF_FUNCTION(ticker);

/////////////////////////////////////////////////////////////////////////////////////////////

#include "CFG_config_panel.h"

#include "chasers_visu.h"

#include <graphics_rebuild1.cpp>

//////////////////////////////////MOUSE/////////////////////////////////////////

int do_mouse_right_click_menu() {
    x_mainmenu = mouse_x, y_mainmenu = mouse_y;
    index_show_main_menu = toggle(index_show_main_menu);
    if (index_show_main_menu == 1) {
        add_a_window(W_MAINMENU);
    } else {
        substract_a_window(W_MAINMENU);
    }

    right_click_for_menu = 0;
    return (0);
}

void my_callback(int flags) {

    if (flags & MOUSE_FLAG_LEFT_DBLCLICK) {
        mouse_double_click = 1;
    }

    if (flags & MOUSE_FLAG_LEFT_DOWN) {
        mouse_double_click = 0;
        mouse_button = 1;
        original_posx = mouse_x;
        original_posy = mouse_y;
        // sab 29/05/2013 deb ---------------------------------------------------------------
        // window_focus_id=detection_over_window();
        if (wc_askConfirmWindowIsOpen()) {
            window_focus_id = W_ASKCONFIRM;
        } else {
            window_focus_id = detection_over_window();
        }
        // sab 29/05/2013 fin ---------------------------------------------------------------
        if (window_focus_id == 0 || window_focus_id == W_LIST) {
            snap_channels_selection_array();
        }
        mouse_released = 0;
    }

    else if (flags & MOUSE_FLAG_LEFT_UP) // relevage bouton
    {
        mouse_button = 0;
        mouse_released = 1; // liberation du curseur souris
        index_click_move_faderspace = 0;
        im_moving_a_window = 0;
        index_mouse_is_tracking = 0;
        audio_folder_scroll_dragging = 0;
        audio_filelist_scroll_dragging = 0;
        for (int i = 0; i < 4; i++) audio_seekbar_dragging[i] = 0;
        for (int i = 0; i < 4; i++) audio_pan_dragging[i] = 0;
        for (int i = 0; i < 4; i++) audio_pitch_dragging[i] = 0;
        index_moving_fader_space = 0;
        index_moving_x_slide = 0;
        index_moving_y_slide = 0;
        index_click_inside_plot = 0;
        plot_facteur_move_x = 0;
        plot_facteur_move_y = 0;
        index_click_inside_relativ_xy = 0;
        rlativ_xm = 0;
        rlativ_ym = 0;
        index_editing_theatre_plan = 0;
        moving_plan_relativ_x = 0;
        moving_plan_relativ_y = 0;
        editing_plan_data_type = 0;
        editing_plot_sizey = 0;
        editing_plot_sizex = 0;
        moving_size_relativ_x = 0;
        moving_size_relativ_y = 0;
        plot_editing_color_line = 0;
        plot_editing_color_background = 0;
        index_adjusting_shape_x = 0;
        index_adjusting_shape_y = 0;
        handle_selected_for_line_editing = 0;
        editing_shape_line_number = 0;
        index_moving_channel_scroller = 0;
        if (dragging_draw == 1) {
            draw_point_is_traced[draw_preset_selected] = 0;
        }
        dragging_draw = 0;
        dragging_channel_in_echo = 0;
    }

    if (flags & MOUSE_FLAG_RIGHT_DOWN) {
        original_posx = mouse_x;
        original_posy = mouse_y;
        mouse_R_button = 1;
        mouse_R_released = 0;

        if (window_focus_id == W_PLOT) {
            index_move_plot_view_port = 1;
            reset_symbols_selected(view_plot_calc_number_is);
            unselect_all_shapes();
            key_unselect_ch();
        } else {
            if (mouse_R_released == 0) {
                mouse_R_released = 1;
                right_click_for_menu =
                    1; // renvoi vers les procs en 10eme de secondes pour enlever le bug d extinctions fenetres
            }
        }
    }

    else if (flags & MOUSE_FLAG_RIGHT_UP) {
        mouse_R_button = 0;
        mouse_R_released = 1;
        index_move_plot_view_port = 0;
        plot_facteur_move_x = 0;
        plot_facteur_move_y = 0;
        index_click_inside_plot = 0;
        set_mouse_range(0, 0, SCREEN_W - 1, SCREEN_H - 1); // liberation du curseur souris
    }

    if (mouse_button == 0) {
        set_mouse_range(0, 0, SCREEN_W - 1, SCREEN_H - 1);
    }
}
END_OF_FUNCTION(my_callback);

////////////checks 10emes et souris/////////////////////////////////////////////////
int ticker_dixiemes_de_secondes_check = BPS_TO_TIMER(10); // 10eme de secondes

void dixiemes_de_secondes() {
    ticks_dixieme_for_icat_and_draw++;
    if (index_is_saving == 0 && init_done == 1 && index_writing_curve == 0 && index_quit == 0) {
        get_current_time();
        for (int yr = 0; yr < 48; yr++) {
            dock_used_by_fader_is[yr] = detect_dock_used(yr);
        }

        do_sprintf_job(); // include time_left calculs
        refresh_minifader_state_view_core(
            position_minifader_selected); // les infos dock temps etc dans fenetre minifaders
        match_minifaders();               // verif du preset en cours: correspond ou pas à un des 8 preset
        nbre_fenetre_actives = check_nbre_opened_windows();

        if (index_do_quick_save == 1) {

            index_is_saving = 1;
            Save_Show();
            sprintf(string_Last_Order, ">> Show Saved at %s", tmp_time);
            index_do_quick_save = 0;
            index_is_saving = 0;
        }

        right_click_on_plot();
    }

    if (index_play_chrono == 1) {
        time_minutes = (int)((float)(actual_tickers_chrono) / (BPS_RATE * 60)) % 60;
        time_secondes = (int)((float)(actual_tickers_chrono) / BPS_RATE) % 60;
        time_centiemes = (int)((float)(actual_tickers_chrono) / (((float)BPS_RATE) / 100)) % 100;
        // report des angles popur garder la mesure en fin de chrono.
        angle_timesnap_min = ((float)(time_minutes) * 5.980005) / 60; //=(int)((angle_timesnap_min/6.280005)*63);
        angle_timesnap_sec =
            ((float)(time_secondes) * 5.980005) / 60; //=(int)((angle_timesnap_sec/((PI*360) / 180))*63);
        angle_timesnap_dix = ((float)(time_centiemes) * 5.980005) / 100; //=(int)((angle_timesnap_dix/6.280005)*10);
        set_time_cursor_to_time_type(time_wheel_datatype_is);
    }

    sprintf(string_last_ch, "Last Ch. selected: %d", last_ch_selected);
    sprintf(string_last_copy_mem, "Mem to copy: %d.%d", CTRLC_mem_to_copy / 10, CTRLC_mem_to_copy % 10);
    if (!index_patch_window)
    {
        sprintf(string_secondary_feeback,
                string_last_over_dock); // last over dock: permet de savoir quelle selection orange est allumée
    }
    else
    {
        sprintf(string_secondary_feeback, string_monitor_patch);
    }
    sprintf(string_mem_onstage, "%d.%d", position_onstage / 10, position_onstage % 10);
    sprintf(string_mem_preset, "%d.%d", position_preset / 10, position_preset % 10);

    if (index_inspekt == 1) {
        indicate_wich_fader_is_the_highest();
    }

    // fenetre confirm always on top
    if (index_ask_confirm == 1 && previous_index_ask_confirm == 0) {
        previous_window_focus_id = window_focus_id;
        window_focus_id = W_ASKCONFIRM;
        add_a_window(window_focus_id);
    }
    previous_index_ask_confirm = index_ask_confirm;

    if (expert_mode == 1) {
        index_edit_listproj = 1;
        index_enable_edit_banger = 1;
        index_edit_audio = 1;
        index_enable_edit_chaser = 1;
        index_enable_edit_Grider = 1;
        index_edit_light_plot = 1;
        index_enable_edit_Draw = 1;
        index_enable_edit_echo = 1;
    }
    if (right_click_for_menu == 1) // sortie du call back pour écrire correctement fermeture fenetres
    {
        do_mouse_right_click_menu();
    }

    do_audio_midi_function_next_prev_track(); // christoph 22/04/14 debugging midi next prev function by outputting it
                                              // inside the 1/10th second loop
}
END_OF_FUNCTION(dixiemes_de_secondes);

///////////////FULL LOOP FUNCTION/////////////////////////////////////////////
int ticker_full_loop_rate = BPS_TO_TIMER(50); // 20ms - 50Hz, sufficient for keyboard/mouse/network
void ticker_full_loop() {

    if (core_do_calculations[2] == 1 && starting_wcat == 0) {
        for (int i = 0; i < core_user_define_nb_bangers; i++) {
            do_loop_bang(i);
            do_bang(i);
        }
        sound_core_processing();
    }

    // check_graphics_mouse_handling/move_window déplacés dans le main loop (thread safety)
    if (index_quit == 0 && index_is_saving == 0) {

        if (allow_artnet_in == 1 && artnet_serveur_is_initialized == 1) {
            if ((bytesreceived = recvfrom(sock, artnet_message, sizeof(artnet_message), 0, (SOCKADDR *)&sinServ,
                                          &sinsizeServ) > 0)) {
                receiving_bytes = 1;
                ReceiveArtDmx();
            } else {
                receiving_bytes = 0;
            }
        }
        // commandes_clavier et DoMouseLevel déplacés dans le main loop (thread safety : wc_key_queue non protégée)
    }
}
END_OF_FUNCTION(ticker_full_loop);

////////////////////////ARTNET VOLATILE ALLEGRO///////////////////////////////////////////////////////////
// artnet envoi toutes les 3 secondes

volatile int ticks_for_artnet = 0;
int ticker_artnet_rate = BPS_TO_TIMER(1);
void ticker_artnet() {
    ticks_for_artnet++;
    if (ticks_for_artnet == 3) {
        for (int bup = 0; bup < 512; bup++) {
            artnet_backup[bup] = 0;
        } // reset du backup pour enclencher l envoi de data automatiquement
        ticks_for_artnet = 0;
        index_art_polling = 0;
        /*if (myDMXinterfaceis!=1 && client_artnet_is_closed==0 && index_artnet_doubledmx==0)
        {fermeture_client_artnet();}//dans le cas d un art poll avec interface usb*/
    }
}
END_OF_FUNCTION(ticker_artnet);

void Load_Fonts() {
    doom.Load("Fonts/doom.ttf", 25, 25, CouleurLigne);
    if (!doom) {
        allegro_message("Couldn't load Fonts/doom.ttf!");
        exit(-1);
    }
    doomblanc.Load("Fonts/doom.ttf", 20, 20, CouleurLigne);
    if (!doomblanc) {
        allegro_message("Couldn't load Fonts/doom.ttf!");
        exit(-1);
    }
    doomrouge.Load("Fonts/doom.ttf", 25, 25, CouleurBlind);
    if (!doomrouge) {
        allegro_message("Couldn't load Fonts/doom.ttf!");
        exit(-1);
    }
    petitdoomblanc.Load("Fonts/doom.ttf", 18, 18, CouleurLigne);
    if (!doomrouge) {
        allegro_message("Couldn't load Fonts/doom.ttf!");
        exit(-1);
    }
    petitdoomInspekt.Load("Fonts/doom.ttf", 10, 10, CouleurFader);
    if (!petitdoomInspekt) {
        allegro_message("Couldn't load Fonts/doom.ttf!");
        exit(-1);
    }

    petitdoomrouge.Load("Fonts/doom.ttf", 10, 10, CouleurBlind);
    if (!petitdoomrouge) {
        allegro_message("Couldn't load Fonts/doom.ttf!");
        exit(-1);
    }

    minidoomblanc.Load("Fonts/doom.ttf", 8, 8, CouleurLigne);
    if (!minidoomblanc) {
        allegro_message("Couldn't load Fonts/doom.ttf!");
        exit(-1);
    }

    axaxax12.Load("Fonts/axaxax.ttf", 12, 12, CouleurLigne);
    if (!axaxax12) {
        allegro_message("Couldn't load Fonts/doom.ttf!");
        exit(-1);
    }

    neuro.Load("Fonts/prototype.ttf", 20, 20, CouleurLigne);
    if (!neuro) {
        allegro_message("Couldn't load Fonts/prototype.ttf!");
        exit(-1);
    }
    neuromoyen.Load("Fonts/prototype.ttf", 14, 14, CouleurLigne);
    if (!neuromoyen) {
        allegro_message("Couldn't load Fonts/prototype.ttf!");
        exit(-1);
    }
    petitchiffre.Load("Fonts/prototype.ttf", 12, 12, CouleurLigne);
    if (!petitchiffre) {
        allegro_message("Couldn't load Fonts/prototype.ttf!");
        exit(-1);
    }
    petitpetitchiffre.Load("Fonts/prototype.ttf", 10, 10, CouleurLigne);
    if (!petitpetitchiffre) {
        allegro_message("Couldn't load Fonts/prototype.ttf!");
        exit(-1);
    }
    minichiffre.Load("Fonts/prototype.ttf", 8, 8, CouleurLigne);
    if (!minichiffre) {
        allegro_message("Couldn't load Fonts/prototype.ttf!");
        exit(-1);
    }
    petitpetitchiffregris.Load("Fonts/prototype.ttf", 9, 9, CouleurLigne.WithAlpha(0.7));
    if (!petitpetitchiffre) {
        allegro_message("Couldn't load Fonts/prototype.ttf!");
        exit(-1);
    }
    minichiffregris.Load("Fonts/prototype.ttf", 8, 8, CouleurLigne.WithAlpha(0.7));
    if (!minichiffre) {
        allegro_message("Couldn't load Fonts/prototype.ttf!");
        exit(-1);
    }
    minichiffrenoir.Load("Fonts/prototype.ttf", 8, 8, CouleurNoir);
    if (!minichiffrenoir) {
        allegro_message("Couldn't load Fonts/prototype.ttf!");
        exit(-1);
    }
    petitpetitchiffrerouge.Load("Fonts/prototype.ttf", 10, 10, CouleurBlind);
    if (!petitpetitchiffrerouge) {
        allegro_message("Couldn't load Fonts/prototype.ttf!");
        exit(-1);
    }
    petitchiffrerouge.Load("Fonts/prototype.ttf", 12, 12, CouleurBlind);
    if (!petitchiffrerouge) {
        allegro_message("Couldn't load Fonts/prototype.ttf!");
        exit(-1);
    }
    minichiffrerouge.Load("Fonts/prototype.ttf", 8, 8, CouleurBlind);
    if (!minichiffrerouge) {
        allegro_message("Couldn't load Fonts/prototype.ttf!");
        exit(-1);
    }
    petitchiffrenoir.Load("Fonts/prototype.ttf", 12, 12, CouleurFond);
    if (!petitchiffrenoir) {
        allegro_message("Couldn't load Fonts/prototype.ttf!");
        exit(-1);
    }
    circuitlevel.Load("Fonts/prototype.ttf", 10, 10, CouleurLevel);
    if (!circuitlevel) {
        allegro_message("Couldn't load Fonts/prototype.ttf!");
        exit(-1);
    }
    circuitfaderlevel.Load("Fonts/prototype.ttf", 10, 10, CouleurFader);
    if (!circuitfaderlevel) {
        allegro_message("Couldn't load Fonts/prototype.ttf!");
        exit(-1);
    }
    circuitblindlevel.Load("Fonts/prototype.ttf", 10, 10, CouleurBlind);
    if (!circuitblindlevel) {
        allegro_message("Couldn't load Fonts/prototype.ttf!");
        exit(-1);
    }
    neuroTitle.Load("Fonts/neuropol.ttf", 70, 70, CouleurLigne);
    if (!neuroTitle) {
        allegro_message("Couldn't load Fonts/neuropol.ttf!");
        exit(-1);
    }
}

////////////////////////////////////////////////////////////////////////////////
int main_actions_on_screen() {
    WC_FDEBUG("main_actions-start");
    Canvas::Fill(CouleurFond);
    if (index_writing_curve == 0) {
        Boxes();
    }
    WC_FDEBUG("main_actions-after-Boxes");
    if (core_do_calculations[3] == 1) {
        trichro_back_buffer(315 / 2, 550 / 2, 125, 15); // calcul trichro ( triangle et saturation dans buffer separé)
    }
    DoMouse();
    previous_ch_selected = last_ch_selected;
    return (0);
}

static void wc_open_log(FILE **out) {
    *out = fopen(WC_LOG_FILE, "a"); // WC_LOG_FILE = %TEMP%\wc_debug.txt
}
static void sigabrt_handler(int) {
    FILE *f;
    wc_open_log(&f);
    if (f) {
        fprintf(f, "*** SIGABRT: abort() called, timer=%s\n", wc_current_timer);
        fclose(f);
    }
    signal(SIGABRT, SIG_DFL);
    raise(SIGABRT);
}
static LONG WINAPI crash_handler(EXCEPTION_POINTERS *ep) {
    FILE *f;
    wc_open_log(&f);
    if (f) {
        fprintf(f, "*** CRASH: code=0x%08lX addr=0x%p thread=%lu timer=%s\n", ep->ExceptionRecord->ExceptionCode,
                ep->ExceptionRecord->ExceptionAddress, GetCurrentThreadId(), wc_current_timer);
        fclose(f);
    }
    return EXCEPTION_EXECUTE_HANDLER;
}
static void wc_terminate_handler() {
    FILE *f;
    wc_open_log(&f);
    if (f) {
        fprintf(f, "*** TERMINATE: std::terminate called, timer=%s\n", wc_current_timer);
        fclose(f);
    }
    abort();
}
static void wc_atexit_handler() {
    FILE *f;
    wc_open_log(&f);
    if (f) {
        fprintf(f, "*** atexit: process exiting normally (exit() appele)\n");
        fclose(f);
    }
}

int main(int /*argc*/, char ** /*argv*/) {
    GetModuleFileName(NULL, mondirectory, 512);
    // Enlever le nom de l'exe pour garder seulement le dossier
    for (int i = strlen(mondirectory) - 1; i >= 0; i--) {
        if (mondirectory[i] == '\\') {
            mondirectory[i] = '\0';
            break;
        }
    }
    {
        char _tmp[512];
        if (GetTempPathA(512, _tmp) > 0)
            snprintf(wc_log_path, 512, "%swc_debug.txt", _tmp);
    } // log dans %TEMP%
    load_screen_config();

    if (dpi_native_rendering)
        SDL_SetHint(SDL_HINT_WINDOWS_DPI_AWARENESS, "permonitorv2");

    {
        int logical_w = GetSystemMetrics(SM_CXSCREEN);
        DEVMODE dm;
        memset(&dm, 0, sizeof(dm));
        dm.dmSize = sizeof(dm);
        if (EnumDisplaySettings(NULL, ENUM_CURRENT_SETTINGS, &dm) && dm.dmPelsWidth > 0 && logical_w > 0)
            wc_dpi_scale = (float)dm.dmPelsWidth / (float)logical_w;
    }

    Settings::SetWindowBorder(false); // plus de momde border window, car inutilisable avec les menus

    Setup::SetupProgram(KEYBOARD | MOUSE);

    if (index_fullscreen == 0) {
        Setup::SetupScreen(largeur_ecran, hauteur_ecran, WINDOWED, desktop_color_depth());
    } else {
        Setup::SetupScreen(largeur_ecran, hauteur_ecran, FULLSCREEN, desktop_color_depth());
    }

    HWND hwnd = win_get_window();
    if (hwnd != NULL)
        MoveWindow(hwnd, posX_mainwindow, posY_mainwindow, SCREEN_W, SCREEN_H, true);

    install_joystick(JOY_TYPE_AUTODETECT);
    calibrate_joystick_name(0);

    install_timer();

    jpgalleg_init();

    mouse_callback = my_callback;

    SetUnhandledExceptionFilter(crash_handler);
    std::set_terminate(wc_terminate_handler);
    signal(SIGABRT, sigabrt_handler);
    atexit(wc_atexit_handler);
    starting_wcat = 1; // bloque les timers pendant toute l'init (réinitialisé à 0 ligne ~1098)
    // Init texture cache + mutex avant les timers (thread safety garantie)
    if (!wc_cache_mutex)
        wc_cache_mutex = SDL_CreateMutex();
    if (!wc_cache)
        wc_cache = (WC_CacheSlot *)calloc(WC_CACHE_SIZE, sizeof(WC_CacheSlot));
    install_int_ex(ticker_dmxIn, ticker_dmxIn_rate, "ticker_dmxIn");
    install_int_ex(ticker_artnet, ticker_artnet_rate, "ticker_artnet");
    install_int_ex(ticker, ticker_rate, "ticker");
    install_int_ex(ticker_arduino, BPS_TO_TIMER(ARDUINO_RATE), "ticker_arduino");
    install_int_ex(dixiemes_de_secondes, ticker_dixiemes_de_secondes_check, "dixiemes_de_secondes");
    install_int_ex(ticker_full_loop, ticker_full_loop_rate, "ticker_full_loop");
    install_int_ex(ticker_midi_clock, ticker_midi_clock_rate, "ticker_midi_clock");

    load_indexes();
    LoadWhiteCatColorProfil();

    Canvas::Fill(CouleurFond);
    Canvas::Refresh();

    Load_Fonts();
    save_load_print_to_screen("Loaded Fonts");

    logo.Load("gfx/logo.png"); // ne s'affiche que en 32 bits !!!

    if (logo.Load("gfx/logo.png") != true) {
        allegro_message("Couldn't load gfx/logo.png");
        exit(-1);
    }

    Canvas::Fill(CouleurFond);
    Canvas::Refresh();
    FILE *dbg = fopen(WC_LOG_FILE, "w");
    if (dbg) {
        fprintf(dbg, "After Canvas::Refresh\n");
        fclose(dbg);
    }
    dbg = fopen(WC_LOG_FILE, "a");
    if (dbg) {
        fprintf(dbg, "Before save_load_print\n");
        fclose(dbg);
    }
    save_load_print_to_screen("Loaded Gfx");
    dbg = fopen(WC_LOG_FILE, "a");
    if (dbg) {
        fprintf(dbg, "Before Load_setup_conf\n");
        fclose(dbg);
    }
    Load_setup_conf();
    dbg = fopen(WC_LOG_FILE, "a");
    if (dbg) {
        fprintf(dbg, "Before GlobInit\n");
        fclose(dbg);
    }
    GlobInit();
    dbg = fopen(WC_LOG_FILE, "a");
    if (dbg) {
        fprintf(dbg, "Before InitMidi\n");
        fclose(dbg);
    }
    InitMidi();
    dbg = fopen(WC_LOG_FILE, "a");
    if (dbg) {
        fprintf(dbg, "Before load_dmx_conf\n");
        fclose(dbg);
    }
    load_dmx_conf();
    dbg = fopen(WC_LOG_FILE, "a");
    if (dbg) {
        fprintf(dbg, "Before load_artnet_conf\n");
        fclose(dbg);
    }
    load_artnet_conf();
    dbg = fopen(WC_LOG_FILE, "a");
    if (dbg) {
        fprintf(dbg, "Before Load_Video_Conf\n");
        fclose(dbg);
    }
    Load_Video_Conf();
    dbg = fopen(WC_LOG_FILE, "a");
    if (dbg) {
        fprintf(dbg, "After Load_Video_Conf\n");
        fclose(dbg);
    }
    dbg = fopen(WC_LOG_FILE, "a");
    if (dbg) {
        fprintf(dbg, "Before load_gel_list\n");
        fclose(dbg);
    }
    // load_gel_list_numerical();
    dbg = fopen(WC_LOG_FILE, "a");
    if (dbg) {
        fprintf(dbg, "mondirectory=%s\n", mondirectory);
        fclose(dbg);
    }

    sprintf(string_last_ch, "Last Ch. selected: %d", last_ch_selected);
    sprintf(string_Last_Order, ">> This is Last Order");

    save_load_print_to_screen("Loading setup conf");
    Load_setup_conf(); // avant tout sinon, le cfg ecrit ailleurs et ca fout la zone
    save_load_print_to_screen("Init Arrays");

    // sauvegarde chargement, en tout dernier
    for (int r = 0; r < 72; r++) {
        specify_who_to_save_load[r] = 1;
    }

    GlobInit(); // rajout version 0.8.2.3
    // reset des bangs
    reset_all_bangers();
    generation_Tableau_noms_clavier_FR();
    // generation_Tableau_noms_fonctions() ;
    save_load_print_to_screen("Init Midi");
    InitMidi();                       // init avant les appels de fichiers
    midi_init_sepecial_case_key_on(); // pour régler pb de cle flashs et key on key off
    ////////////////////////////////////////////////////////////

    load_onstart_config();
    load_core_config();
    ///////////////////////////////////////////////////////
    save_load_print_to_screen("Loading Dmx conf");
    load_dmx_conf();
    save_load_print_to_screen("Loading Art-net conf");
    load_artnet_conf();

    detection_mise_en_place_carte_reseaux();
    // opening artnet conf
    if (dmx_interface_active[1] == 1) {
        initialisation_client_artnet();
        // ConstructArtPoll();
        ConstructArtPollReply();
        nbrbytessended = sendto(sockartnet, ArtPollBuffer, sizeof(ArtPollBuffer), 0, (SOCKADDR *)&sinS, sinsize);
        // ArtNet
        ArtDmx();
        save_load_print_to_screen("Double DMX Art-net ON");
    }
    load_network_conf(); // icat
    save_load_print_to_screen("Loading Art-netnetwork conf");
    sprintf(tmp_ip_artnet, ip_artnet);
    load_show_coming_from();
    idf++;
    On_Open_name_of_directory();
    save_load_print_to_screen("Loading Gels List");
    load_gel_list_numerical();
    idf++;
    Canvas::Fill(CouleurFond);
    Canvas::Refresh();
    save_load_print_to_screen("Init Sound");
    InitSound();
    Load_Show();
    init_kbd_custom();
    save_load_print_to_screen("Init Keyboard");
    Show_report_save_load();
    save_load_print_to_screen("Init Dmx");
    Init_dmx_interface();

    if (camera_on_open == 1) {
        save_load_print_to_screen("Init Camera");
        InitVideo();
    }

    scan_importfolder("");
    scan_savesfolder();
    Load_Audio_Conf();
    scan_audiofolder();

    if (open_arduino_on_open == 1) {
        save_load_print_to_screen("Init Arduino");
        arduino_init(0);
    }

    save_load_print_to_screen("Init Backamnesia");
    if (set_display_switch_mode(SWITCH_BACKGROUND)) {
        set_display_switch_mode(SWITCH_BACKAMNESIA);
    }
    init_done = 1;
    if (there_is_an_error_on_save_load == 1) {
        index_show_save_load_report = 1;
        there_is_change_on_show_save_state = 1;
    }

    mouse_released = 0;
    mouse_double_click = 0;
    entered_main = 1;
    // launchpad séparé
    if (enable_launchpad == 1) {
        reset_launchpad();
    }

    init_artnet_variables();
    // serveur
    if (allow_artnet_in == 1 && artnet_serveur_is_initialized == 0) {
        initialisation_serveur_artnet();
    }
    bmp_buffer_trichro = create_bitmap(315, 550);
    clear_bitmap(bmp_buffer_trichro);
    rafraichissement_padwheel();
    rafraichissement_clockwheel();
    recalculate_draw_sizes(draw_preset_selected);
    // init du flash de bang en cours
    bang_is_sended[index_banger_selected] = 1;
    rest(100);
    starting_wcat = 1;
    for (int i = 0; i < 4; i++) {
        audiofile_selected = player_has_file_coming_from_pos[i];
        sprintf(audiofile_name, list_audio_files[audiofile_selected]);
        if (strcmp(audiofile_name, "") != 0) {
            AffectSoundFile(i);
        }
        rest(10);
    }
    if (index_loading_a_sound_file != 0) {
        for (int i = 0; i < 4; i++) {
            // position player
            if (player_ignited[i] == 1 && player_position_on_save[i] <= length_of_file_in_player[i]) {
                switch (i) {
                case 0:
                    player1->setPosition(player_position_on_save[i]);
                    break;
                case 1:
                    player2->setPosition(player_position_on_save[i]);
                    break;
                case 2:
                    player3->setPosition(player_position_on_save[i]);
                    break;
                case 3:
                    player4->setPosition(player_position_on_save[i]);
                    break;
                default:
                    break;
                }
                position_of_file_in_player[i] = player_position_on_save[i];
            }
        }
    }

    reset_temp_state_for_channel_macros_launch(); // christoph 18/12/14 pour intialisation au démarrage de wcat des
                                                  // channels macros

    starting_wcat = 0;
    SetUnhandledExceptionFilter(crash_handler);
    signal(SIGABRT, sigabrt_handler);
    {
        FILE *_d = fopen(WC_LOG_FILE, "a");
        if (_d) {
            fprintf(_d, "=== ADDR MAP ===\n");
            fprintf(_d, "Memoires           : %p  end:%p\n", (void *)Memoires, (void *)(Memoires + 10000));
            fprintf(_d, "MemoiresExistantes : %p  end:%p\n", (void *)MemoiresExistantes,
                    (void *)(MemoiresExistantes + 10000));
            fprintf(_d, "Times_Memoires     : %p  end:%p\n", (void *)Times_Memoires, (void *)(Times_Memoires + 10000));
            fprintf(_d, "SchwzMemoires      : %p  end:%p\n", (void *)SchwzMemoires, (void *)(SchwzMemoires + 121));
            fprintf(_d, "grid_levels        : %p  end:%p\n", (void *)grid_levels, (void *)(grid_levels + 128));
            fprintf(_d, "=== END MAP ===\n");
            fclose(_d);
        }
    }
    try {
        while (index_quit != 1) {
            MemoiresExistantes[0] = 1;
            show_im_recording_a_time = 0; // met à zéro l'affichage du stock visuel du time

            // must be in main loop to avoid freezing
            if (arduino_device_0_is_ignited == 1 && ticks_arduino != old_ticks_arduino && index_is_saving == 0 &&
                init_done == 1 && index_writing_curve == 0 && index_quit == 0) {
                arduino_merge_and_do_data_out();
                arduino_read(); // doit etre posé après data out
                serial0.Flush();
                old_ticks_arduino = ticks_arduino;
                arduino_do_digital_in_whitecat();
                arduino_do_analog_in_whitecat();
            }

            if (!index_art_polling)
            {
                process_midi_input();
                DoMouseLevel();
                if ((mouse_button == 1 && mouse_released == 0) || wc_click_pending) {
                    wc_click_pending = false;
                    if (!im_moving_a_window)
                    {
                        check_graphics_mouse_handling();
                    }
                    else
                    {
                        move_window(window_focus_id);
                    }
                    wc_dirty = true; // redraw while mouse held (for hold-to-scroll visual feedback)
                }
                if (wc_dirty) {
                    main_actions_on_screen();
                    wc_dirty = false;
                }
                rest(10);
            }
            else
            {
                if ((bytesreceived = recvfrom(sock, artpollreply_message, sizeof(artpollreply_message), 0,
                                              (SOCKADDR *)&sinS, &sinsize) != 0)) {
                    AnalyseArtPollReply();
                }
                Procedure("Art-Net Polling", "Please wait 3 seconds, polling network ...");
            }
            if (there_is_change_on_show_save_state == 1) {
                wc_dirty = true;
                check_save_load_report_window();
                there_is_change_on_show_save_state = 0;
            }

            WC_FDEBUG("main-before-Canvas-Refresh");
            Canvas::Refresh();
            // Drain supplémentaire : récupère tout KEYDOWN arrivé pendant rest(10)/rest(5)
            // (SDL_Delay ne traite pas les events — ils s'accumulent dans la queue SDL).
            // Ne pas appeler wc_process_events() ici (s'arrête à MOUSEBUTTONDOWN) :
            // on veut récupérer les KEYDOWN sans risquer de dépiler un MOUSEDOWN.
            {
                SDL_Event _ke;
                while (SDL_PeepEvents(&_ke, 1, SDL_GETEVENT, SDL_KEYDOWN, SDL_KEYDOWN) > 0)
                    wc_handle_event(_ke);
            }
            // Merger() (ticker, 50Hz) calcule bufferSequenciel depuis bufferSaisie.
            // Si on dessine avant qu'il ait tourné, l'affichage montre les anciennes valeurs
            // alors que le DMX est déjà mis à jour (source du bug "premier appui invisible").
            // Quand une touche est en attente : 25ms >= 1 cycle ticker (20ms) — bufferSequenciel frais.
            bool _had_key = !wc_key_queue.empty();
            commandes_clavier();

            if (index_do_a_screen_capture == 1) {
                do_a_screen_capture();
                index_do_a_screen_capture = 0;
            }
            if (index_do_a_plot_screen_capture == 1) {
                do_plot_screen_capture(plot_name_of_capture);
                index_do_a_plot_screen_capture = 0;
            }

            rest(_had_key ? 25 : 5); // 25ms si touche : attend Merger() ticker
        }
    } catch (const std::exception &e) {
        FILE *f = fopen(WC_LOG_FILE, "a");
        if (f) {
            fprintf(f, "*** C++ EXCEPTION: %s\n", e.what());
            fclose(f);
        }
    } catch (...) {
        FILE *f = fopen(WC_LOG_FILE, "a");
        if (f) {
            fprintf(f, "*** UNKNOWN C++ EXCEPTION\n");
            fclose(f);
        }
    }

    entered_main = 0;
    substract_a_window(W_ASKCONFIRM);
    window_focus_id = window_who_is_on_top();

    init_done = 0; // evite envois data pendant save de fermeture

    Canvas::Refresh();
    Canvas::Fill(CouleurFond);

    save_load_print_to_screen("Quitting");
    Show_report_save_load();
    save_load_print_to_screen("Closing dmx");
    Close_dmx_interface();
    save_load_print_to_screen("Closing Arduino");
    arduino_close(0);

    save_load_print_to_screen("Closing video");
    CloseVideo();
    save_load_print_to_screen("Closing midi");

    save_load_print_to_screen("Saving Show");
    if (index_please_do_not_save == 0) // a garder sinon plante sur le please not save
    {
        Save_Show(); // avant QuitMidi() pour capturer l'état des ports MIDI IN
        save_load_print_to_screen("Saving Video Conf");
        Save_Video_Conf();
        save_load_print_to_screen("Saving Screen Conf");
        Save_Screen_Config();
        save_load_print_to_screen("Saving Setup Conf");
        Save_setup_conf();
    }
    QuitMidi(); // après Save_Show() pour que les ports MIDI soient encore ouverts au moment de la sauvegarde
    destroy_bitmap(bmp_buffer_trichro);
    remove_int(ticker_full_loop);
    remove_int(ticker);
    remove_int(ticker_dmxIn);
    remove_int(ticker_artnet);
    remove_int(ticker_arduino);
    remove_int(dixiemes_de_secondes);
    remove_int(ticker_midi_clock);
    remove_timer();
    WSACleanup(); // liberation librairie socket
    midi_backend_close();
    Sleep(200);
    WSACleanup();
    exit(0);
    return 0;
}
END_OF_MAIN()
