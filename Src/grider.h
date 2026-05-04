#pragma once

//////////GRIDER 24*24 matrice evailable//////////////////

extern bool index_grider_window;

extern bool index_click_move_Grider;
extern bool index_over_Grider;
extern int grider_window_x;
extern int grider_window_y;
extern int grider_begin_channel_is;
extern int grider_nb_col;
extern int grider_nb_row;

extern bool index_enable_edit_Grider;
extern int size_grille;

// previsu d une grid sans edition
extern int hauteurGlobalGridviewer;
extern bool show_global_view_grider;
extern int grid_selected_for_view;
extern int grid_step_view;
extern char string_grid_view_timing_global[24][60];
extern char string_grid_view_timing_global_delays[24][60];
extern char string_grid_count[4][24];
extern int size_preview_case;

extern int index_nbre_griderplayers_visibles;

extern int index_grider_selected[4];
extern int index_grider_step_is[4];
extern int index_grider_seek_pos[4];
extern bool grider_is_playing[4];
extern bool grider_autostopmode[4];
extern bool index_show_grid_player[4];
// options d enclenchement des calculs
extern bool grider_goto_mode[4];
extern bool grider_seekto_mode[4];
extern bool grider_stoplay_mode[4];
extern bool grider_count_mode[4];
extern int grid_count[128][1024];
extern int grid_counted_times[128][1024];
extern bool to_step_editing_mode[4];
extern int index_StepTo[4];
extern int index_Grid_copyto[4][2];
extern bool grid_player_slave[4];
extern bool grid_affect_to_dock[4];

extern int largeurGrider;
extern int grider_facteur_hauteur;
int hauteurGrider =
extern 50 + (index_nbre_players_visibles * (30 + (grider_facteur_hauteur * size_grille))) + hauteurGlobalGridviewer;

extern int position_grid_editing;

extern int temoin_over_grid_channel;

extern char grider_name[128][25];
extern unsigned char grid_levels[128][1024][513];
extern unsigned char temp_grid_levels_for_save[32][1024][513];
extern float grid_times[128][1024][4];
extern int grid_goto[128][1024][2];
extern int grid_seekpos[128][1024];
extern bool grid_stoplay[128][1024];

extern char string_time_in_grider[4][60];
extern char string_time_in_grider_next_step[4][60];

extern char string_next_step_to[4][48];
// pour confirmation actions
extern bool index_clear_a_grid_step;
extern bool index_clear_a_grid;
extern bool index_clear_a_grid_serie_step;
extern int grid_to_clear;
extern int step_grid_to_clear;
extern int index_for_grid_stepto;
extern bool index_for_copy_grid;
extern bool index_for_insert_grid;
extern int destination_grid_for_copy;
extern int destination_step_for_copy;
extern bool index_grid_tostep_or_not;
extern int from_gridstep_to;
extern bool index_do_affect_grid_to_fader;

extern int gridplayer_to_affect_is;
extern int faders_dock_grid_affectation[48][6];

extern int grid_midi_multiple_for_step_call[4];
extern int GplSnapFader[4];

// les 4 players crossfades
extern unsigned char buffer_gridder[4][513];

extern int grid_crossfade_start_time[4];
extern int grid_end_time[4];
extern int grid_crossfade_speed[4];
extern int grid_niveauX1[4];
extern int grid_niveauX2[4];
extern float grid_floatX1[4];
extern float grid_floatX2[4];
extern float grid_fraction_X2_in[4];
extern float grid_fraction_X1_out[4];
extern int grid_in_preset[4][2];

extern float tempIN[4], tempOUT[4];
extern int grid_delay_in[4], grid_delay_out[4];
// liaison Gridplayer MEM sequenciel
extern int set_from_seq_gridplayer1_next_step[10000];
extern bool index_do_affect_step_gridplayer_to_mem;
extern int index_mem_to_be_affected_by_gpl;
extern int gpl1_affect_step_to_mem;
extern bool show_gridplayer_in_seq;
extern bool index_link_speed_crossfade_to_gpl1;
