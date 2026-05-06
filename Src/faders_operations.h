#pragma once

int Unselect_other_docks(int themaster, int thedock);
int DoTimeToDock(int faa, int docc);
int ClearDock(int fad, int dk);
int affect_echo_to_dock(int echo, int ff, int dd);
int affect_draw_preset_to_dock(int pr, int ff, int dd);
int ClearFader(int fad);
int record_minifaders_selected_as_fgroup(int fd, int dk);
int affect_selected_faders_to_fgroup(int fd, int dk);
int DoDock(int fad, int dk);
int DoModify(int fad, int dk);
int DoReport(int fad, int dk);
int affect_color_to_dock(int fa, int doc);
int affect_network_to_dock(int faa, int docc);
int affect_dmxIN_to_dock(int faa, int docc);
int affect_video_tracking_to_dock(int faa, int docc);
int affect_audio_control_to_dock(int faa, int docc);
int affect_chaser_to_dock(int chas, int fd, int dk);
int Unselect_all_channels();
int affect_color_to(int dockC_sel, int couleur);
int affect_to_tracker(int dock_selected, int tracker_selected);
int do_direct_channel(int fad, int dk);
