#include "grider.h"

//////////GRIDER 24*24 matrice evailable//////////////////
bool index_grider_window = 0;
bool index_click_move_Grider = 0;
bool index_over_Grider = 0;
int grider_window_x = 100;
int grider_window_y = 100;
int grider_begin_channel_is = 1;
int grider_nb_col = 20;
int grider_nb_row = 10;
bool index_enable_edit_Grider = 0;
int size_grille = 15;
// previsu d une grid sans edition
int hauteurGlobalGridviewer = 100;
bool show_global_view_grider = 0;
int grid_selected_for_view = 0;
int grid_step_view = 0;
char string_grid_view_timing_global[24][60];
char string_grid_view_timing_global_delays[24][60];
char string_grid_count[4][24];
int size_preview_case = 5;
int index_nbre_griderplayers_visibles = 0;
int index_grider_selected[4];
int index_grider_step_is[4];
int index_grider_seek_pos[4];
bool grider_is_playing[4];
bool grider_autostopmode[4];
bool index_show_grid_player[4];
// options d enclenchement des calculs
bool grider_goto_mode[4];
bool grider_seekto_mode[4];
bool grider_stoplay_mode[4];
bool grider_count_mode[4];
int grid_count[128][1024];
int grid_counted_times[128][1024];
bool to_step_editing_mode[4];
int index_StepTo[4];
int index_Grid_copyto[4][2]; // 0 grid 1 step
bool grid_player_slave[4];
bool grid_affect_to_dock[4];
int largeurGrider = 710;
int grider_facteur_hauteur = 10;
extern int index_nbre_players_visibles;
int hauteurGrider =
    50 + (index_nbre_players_visibles * (30 + (grider_facteur_hauteur * size_grille))) + hauteurGlobalGridviewer;
int position_grid_editing;
int temoin_over_grid_channel = 0;
char grider_name[128][25];
unsigned char grid_levels[128][1024][513];              // grid number // steps // 24*24 les cases max
unsigned char temp_grid_levels_for_save[32][1024][513]; // pour sauvegarde
float grid_times[128][1024][4];                         // 0=DIN 1=IN 2=DOUT 3=OUT  // ATTENTION NO DELAY !!!!
int grid_goto[128][1024][2];                            // 0 grid 1 step
int grid_seekpos[128][1024];
bool grid_stoplay[128][1024];                           // 1=pause
char string_time_in_grider[4][60];
char string_time_in_grider_next_step[4][60];
char string_next_step_to[4][48];
// pour confirmation actions
bool index_clear_a_grid_step = 0;
bool index_clear_a_grid = 0;
bool index_clear_a_grid_serie_step = 0;
int grid_to_clear = 0;
int step_grid_to_clear = 0;
int index_for_grid_stepto = 0;
bool index_for_copy_grid = 0;
bool index_for_insert_grid = 0;
int destination_grid_for_copy = 0;
int destination_step_for_copy = 0;
bool index_grid_tostep_or_not = 0;
int from_gridstep_to = 0;
bool index_do_affect_grid_to_fader = 0;
int gridplayer_to_affect_is = -1;
int faders_dock_grid_affectation[48][6];
int grid_midi_multiple_for_step_call[4];
int GplSnapFader[4];
// les 4 players crossfades
unsigned short buffer_gridder[4][513]; // [grid 16 bit] sortie du crossfade grid en 16 bit (endpoints 8 bit interpoles finement)
int grid_crossfade_start_time[4];
int grid_end_time[4];
int grid_crossfade_speed[4];
int grid_niveauX1[4];
int grid_niveauX2[4];
float grid_floatX1[4];
float grid_floatX2[4];
float grid_fraction_X2_in[4];
float grid_fraction_X1_out[4];
int grid_in_preset[4][2]; // 0 la grille 1 le step
float tempIN[4], tempOUT[4];
int grid_delay_in[4], grid_delay_out[4];
// liaison Gridplayer MEM sequenciel
int set_from_seq_gridplayer1_next_step[10000];
bool index_do_affect_step_gridplayer_to_mem = 0;
int index_mem_to_be_affected_by_gpl = 0;
int gpl1_affect_step_to_mem = 0;
bool show_gridplayer_in_seq = 0;
bool index_link_speed_crossfade_to_gpl1 = 0;
