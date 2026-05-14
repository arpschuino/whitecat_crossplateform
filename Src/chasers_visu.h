#pragma once

int set_cue_in_view(int xp, int yp, bool state);
int set_cue_out_view(int xp, int yp, bool state);
int seek_cue_in_view(int xp, int yp, bool state);
int set_forward_sens_view(int xp, int yp, bool state);
int set_backward_sens_view(int xp, int yp, bool state);
int set_back_and_forward_sens_view(int xp, int yp, bool state);
int bouton_track_up(int xp, int yp, bool state);
int bouton_track_stay(int xp, int yp, bool state);
int bouton_track_down(int xp, int yp, bool state);
int bouton_view_track_downup(int xcha, int ycha, bool state);
int bouton_view_launchpad_pos(int xcha, int ycha, bool state);
int Track_draw(int xp, int yp, int num_track);
int chaser_window(int xcha, int ycha);
