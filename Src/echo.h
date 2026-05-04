#pragma once

////ECHO
extern bool index_show_echo_window;

extern int x_echo;
extern int y_echo;
extern int echo_window_x_size;
extern int echo_window_y_size;

extern bool index_enable_edit_echo;

extern bool index_affect_echo_to_dock;
extern bool index_do_affect_echo_to_dock;
extern int index_selected_echo_for_manipulation;

extern bool index_ask_clear_echo_preset;
extern int echo_over_channel;
extern bool dragging_channel_in_echo;

extern int echo_affected_to_dock[48][6];

extern int core_user_define_nb_echo;

extern int echo_selected;

extern int echo_pointing_fader_num[24];
extern bool echo_is_echo_mode[24];
extern float echo_global_params[24][3];
extern bool echo_channel_manipulate_mode[24];
extern int echo_grid_channel_position[24];
extern float echo_levels[24][2][513];
extern bool do_bounce[24];

extern float tmp_echo[24][513];
extern bool echo_way[24][513];
extern float tmp_falling_from_level[24][513];
extern bool bounce_is_prepared[24];
extern int fader_before_bounce[48];

extern float snap_echo_to_recall[24][513];

extern float une_valeur_de_debug;
