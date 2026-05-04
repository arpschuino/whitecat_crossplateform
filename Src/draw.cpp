#include "draw.h"

/////////////DRAW///////////////////////////////////////////////////////////////
int x_Wdraw = 100;
int y_Wdraw = 100;
int draw_window_x_size = 600;
int draw_window_y_size = 610;
int draw_tracking_aera_size_x = 555;
int draw_tracking_aera_size_y = 430;
bool index_draw_window = 0;
bool ask_clear_draw_preset = 0;
int draw_prst_to_clear = 0;
int drawing_on_index_case;
int previous_index_case[6];
int index_case[6];
bool index_enable_edit_Draw = 0;
int draw_preset_parameters[6][2]; // grilles de max 25x20 Taille nbre col nbre lignes
int draw_largeur_case[6];
int draw_hauteur_case[6];
int draw_modulo_largeur_case[6];
int draw_modulo_hauteur_case[6];
float draw_preset_levels[6][500];
bool draw_snap_preset_levels[6][500];
int draw_preset_channel_routing[6][500];
int draw_preset_selected = 0;
int previous_draw_preset_selected = -1;
int draw_brush_type[6]; // point / GPL
int draw_mode[6];       // 0= draw 1= erase 2= solo 3=ghost
float draw_level_to_do[6];
float draw_damper_decay_factor[6];
float draw_ghost_to_do[6];
bool draw_point_is_traced[6];
bool previous_draw_brush_type[6];
int previous_draw_mode[6];
float previous_draw_level_to_do[6];
float previous_draw_damper_decay_factor[6];
float previous_draw_ghost_to_do[6];
int draw_centre_x[6];
int draw_centre_y[6];
int DrawAffectedToDck[48][6];
bool index_affect_draw_to_dock = 0;
int draw_channel_recording_mode = 0;
int draw_preset_selected_for_order = 0;
bool index_do_draw_affect_to_dock = 0;
bool dragging_draw = 0;
volatile bool merging_gpl_in_draw = 0;
int draw_get_gpl[6];
int draw_offset_gpl[6];
int draw_arduino_xy[4];
// Variables de calculs damper decay
float damper_target_val = 0.0;
float damper_val = 0.0;
float damper_vel = 0.0;
