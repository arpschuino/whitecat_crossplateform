#pragma once

int refresh_chaser_midi_out();
int set_refresh_mode_for_chaser(bool value);
int refresh_launchpad_on_chaser();
int chaser_calcul_time_joint(int numchaser);
int chaser_calcul_step(int numchaser, float ratio_for_no_reset);
int do_chaser();
int chaser_clear_preset(int numchaser, int presettrk);
int DoClearChaserTrack(int numchaser, int numtrack);
int DoClearAChaser(int numchaser);
int DoDockChaserTrack(int numchaser, int numtrack);
int chaser_load_from_preset(int numchaser, int presettrk);
int chaser_store_in_preset(int numchaser, int presettrk);
int DoModifyChaserTrack(int numchaser, int numtrack);
int DoReportChaserTrack(int numchaser, int numtrack);
int chaser_seek_at_beg(int numchase);
int chaser_select_all_track(int numchase);
int chaser_select_off_all_track(int numchase);
int chaser_select_inv_track(int numchase);
int do_logical_fenetre_chasers(int xcha, int ycha);
int set_chaser_tempo_unit(int the_chas);
int do_autolaunch();
int refresh_chaser_window_timeline_for_midi_out();
