#pragma once

///////////////////////////////////BANGER///////////////////////////////////////
extern int X_banger;
extern int Y_banger;
extern int size_X_Banger;
extern int size_Y_Banger;
extern bool index_show_banger_window;
extern int index_over_banger_window;
extern bool index_banger_is_on;
extern int index_banger_selected;
extern bool index_enable_edit_banger;
extern char string_event[72];
extern int Banger_Memoire[10000];
extern char bangers_name[128][25];
extern int bangers_type[128][6];
extern int bangers_action[128][6];
extern int bangers_params[128][6][2];
extern float bangers_delay[128][6];
extern int start_time_for_banger[128];
extern float end_time_for_banger[128];
extern bool event_sended[128][6];
extern bool bang_is_sended[128];

extern bool do_loop_banger[128];
extern float time_loop_banger[128];
extern volatile int ticker_loop_banger[128];

extern char keyname[256][16];

extern bool bang_in_mem_is_sended;
extern int remember_state_of_banged_fader[49][24];
extern int memoire_asked_in_bang;
extern char string_alarm[128][25];
extern char string_THE_alarm[25];
extern bool index_show_alarm;
extern bool index_over_alarm;
extern int XAlarm;
extern int YAlarm;
extern bool index_click_move_alarm;
extern int index_alarm_from_banger_num;
extern int previous_preset_color;
extern int previous_preset_video;
extern int affect_banger_number;
extern bool index_crossfading;
extern bool do_light_bang_it;
extern bool do_light_bang_solo[6];

extern int xVisuBanger;
extern int yVisuBanger;
extern float default_time_of_the_bang;
extern int last_banger_sended_manually;
extern int bang_the_chan_is;
extern int bang_val_the_chan_is;

extern bool editing_banger_family;
extern bool editing_banger_action;

extern bool index_ask_copy_banger;
extern bool index_copy_banger;
extern int index_banger_to_copy_in;

extern bool index_ask_clear_banger;
