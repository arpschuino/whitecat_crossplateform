#pragma once

///////////////////////////////////BANGER///////////////////////////////////////
int X_banger = 100;
int Y_banger = 100;
int size_X_Banger = 610;
int size_Y_Banger = 300;
bool index_show_banger_window = 0;
int index_over_banger_window = 0;
bool index_banger_is_on = 0;
int index_banger_selected = 0;
bool index_enable_edit_banger = 0;
char string_event[72];
int Banger_Memoire[10000];
char bangers_name[128][25];
int bangers_type[128][6];
int bangers_action[128][6];
int bangers_params[128][6][2];
float bangers_delay[128][6];
int start_time_for_banger[128];
float end_time_for_banger[128];
bool event_sended[128][6];
bool bang_is_sended[128];

bool do_loop_banger[128];
float time_loop_banger[128];
volatile int ticker_loop_banger[128];

char keyname[256][16];

bool bang_in_mem_is_sended = 0;
int remember_state_of_banged_fader[49][24];
int memoire_asked_in_bang = 0;
char string_alarm[128][25];
char string_THE_alarm[25];
bool index_show_alarm = 0;
bool index_over_alarm = 0;
int XAlarm = 300;
int YAlarm = 200;
bool index_click_move_alarm = 0;
int index_alarm_from_banger_num = 0;
int previous_preset_color = 0;
int previous_preset_video = 0;
int affect_banger_number = 0;
bool index_crossfading = 0;
bool do_light_bang_it = 0;
bool do_light_bang_solo[6];

int xVisuBanger = 1050;
int yVisuBanger = 40;
float default_time_of_the_bang = 1.0;
int last_banger_sended_manually = 0;
int bang_the_chan_is = 0;
int bang_val_the_chan_is = 0;

bool editing_banger_family = 0;
bool editing_banger_action = 0;

bool index_ask_copy_banger = 0;
bool index_copy_banger = 0;
int index_banger_to_copy_in = -999;

bool index_ask_clear_banger = 0;
