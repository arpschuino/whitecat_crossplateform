#pragma once

/////////////DRAW///////////////////////////////////////////////////////////////

extern int x_Wdraw;
extern int y_Wdraw;
extern int draw_window_x_size;
extern int draw_window_y_size;

extern int draw_tracking_aera_size_x;
extern int draw_tracking_aera_size_y;

extern bool index_draw_window;

extern bool ask_clear_draw_preset;
extern int draw_prst_to_clear;
extern int drawing_on_index_case;
extern int previous_index_case[6];
extern int index_case[6];

extern bool index_enable_edit_Draw;

extern int draw_preset_parameters[6][2];

extern int draw_largeur_case[6];
extern int draw_hauteur_case[6];
extern int draw_modulo_largeur_case[6];
extern int draw_modulo_hauteur_case[6];

extern float draw_preset_levels[6][500];
extern bool draw_snap_preset_levels[6][500];
extern int draw_preset_channel_routing[6][500];

extern int draw_preset_selected;
extern int previous_draw_preset_selected;

extern int draw_brush_type[6];
extern int draw_mode[6];
extern float draw_level_to_do[6];
extern float draw_damper_decay_factor[6];
extern float draw_ghost_to_do[6];

extern bool draw_point_is_traced[6];

extern bool previous_draw_brush_type[6];
extern int previous_draw_mode[6];
extern float previous_draw_level_to_do[6];
extern float previous_draw_damper_decay_factor[6];
extern float previous_draw_ghost_to_do[6];

extern int draw_centre_x[6];
extern int draw_centre_y[6];

extern int DrawAffectedToDck[48][6];

extern bool index_affect_draw_to_dock;

extern int draw_channel_recording_mode;

extern int draw_preset_selected_for_order;

extern bool index_do_draw_affect_to_dock;
extern bool dragging_draw;

extern volatile bool merging_gpl_in_draw;

extern int draw_get_gpl[6];
extern int draw_offset_gpl[6];

extern int draw_arduino_xy[4];

// Variables de calculs damper decay
extern float damper_target_val;
extern float damper_val;
extern float damper_vel;
