#pragma once
int Chrono_Reset();
int store_state_of_fader_before_bang(int fader_num);
int restore_state_of_fader_before_bang(int fader_num);
int Bang_event_back(int banger_num, int event_num);
int back_up_fader_if_a_fader_inside(int banger_num);
int Bang_event(int banger_num, int event_num);
int do_loop_bang(int banger_is);
int do_bang(int banger_is);
int do_go_back_bang(int banger_is);
int do_logical_feedback_banger(int xvis, int yvis);
int do_logical_fenetre_banger(int xb, int yb);
int refresh_banger_wx();
int do_send_bang();
