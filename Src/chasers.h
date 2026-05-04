#pragma once

//////////////////////////CHASERS//////////////////////////////////////////////
extern bool index_window_chasers;
extern int index_affichage_digit_time_unit;

extern int Xchasers;
extern int Ychasers;
extern bool index_over_chasers;
extern bool index_click_move_chasers;
extern int chaser_selected;
extern bool index_enable_edit_chaser;
extern bool index_affect_chaser_to_dock;
extern bool index_do_affect_fx;
extern int nbre_track_visualisables;
extern int nbre_de_cases_par_track;
extern int nbre_tracks_par_chaser;
extern int chaser_operator_is;

extern char chaser_name[128][25];
extern char chaser_track_name[128][24][25];
extern float time_unit[128];

extern int chaser_step_operation[128][24][36];
extern bool chaser_is_playing[128];
extern bool chaser_is_in_loop[128];
extern bool chaser_way[128];
extern bool chaser_aller_retour[128];
extern bool track_is_on[128][24];
extern int track_level[128][24];
extern int position_affichage_track_num[128];
extern int TrackTypeIs[128][24];
extern int TrackContains[128][24][514];
extern int TracksBuffer[128][24][514];
extern int MergerBufferChasers[128][514];

extern float index_progression_chaser_step[128];
extern float previous_index_progression_chaser_step[128];
extern int chaser_begin_step_is[128];
extern int chaser_end_step_is[128];
extern bool chaser_time_mode[128];

extern int chaser_time_position[128];
extern int chaser_start_time[128];
extern float chaser_start_of_step[128];
extern float chaser_end_of_step[128];
extern int chaser_step_is[128];
extern int previous_chaser_step_is[128];

extern int count_steps_for_fades[128][24];
extern int joint_begin_step[128][24];
extern int joint_end_step[128][24];
extern int joint_nbr_step[128][24];

extern int chaser_selected_for_record;
extern int track_selected_for_record;
extern int chaser_preset_selected_for_record;

extern bool index_do_dock_track;
extern bool index_do_clear_track;
extern bool index_do_report_track;
extern bool index_do_modify_track;
extern bool index_do_clear_chaser;
extern bool index_do_store_chaser_preset;
extern bool index_do_clear_chaser_preset;

extern int view_chaser_affected_to_fader[128][2];
extern bool index_slave_chaser_to_accelerometre[128];

extern float chaser_slaviness[128];
extern float previous_chaser_slaviness[128];
extern bool chaser_preset[128][4][24];

extern bool launchpad_chaser_mode;
extern int chaser_midi_rows;
extern int previous_color_on_chaser[8][8];
extern int chaser_step_launchpad[128];

extern int mem_to_load_in_chaser;
extern bool index_do_store_mem_in_chaser;
extern int TrackHasMem[128][24];
extern int over_track_show_channel[514];
extern bool i_m_over_a_track;
