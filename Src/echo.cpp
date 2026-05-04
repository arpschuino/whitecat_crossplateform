#include "echo.h"

////ECHO
bool index_show_echo_window = 0;
int x_echo = 100;
int y_echo = 100;
int echo_window_x_size = 675;
int echo_window_y_size = 285;
bool index_enable_edit_echo = 0;
bool index_affect_echo_to_dock = 0;
bool index_do_affect_echo_to_dock = 0;
int index_selected_echo_for_manipulation = 0;
bool index_ask_clear_echo_preset = 0;
int echo_over_channel = 0;
bool dragging_channel_in_echo = 0;
int echo_affected_to_dock[48][6];
int core_user_define_nb_echo = 23;
int echo_selected = 0;
int echo_pointing_fader_num[24];
bool echo_is_echo_mode[24];      // 0= bounce mode 1= continuous Echo mode
float echo_global_params[24][3];
bool echo_channel_manipulate_mode[24]; // 0 level up, 1 ground
int echo_grid_channel_position[24];
float echo_levels[24][2][513];
bool do_bounce[24];
float tmp_echo[24][513];
bool echo_way[24][513];
float tmp_falling_from_level[24][513];
bool bounce_is_prepared[24];
int fader_before_bounce[48];
float snap_echo_to_recall[24][513];
float une_valeur_de_debug = 0.0;
