#pragma once

int set_refresh_mode_for_chaser(bool value);
int chaser_clear_preset(int numchaser, int presettrk);
int DoClearChaserTrack(int numchaser, int numtrack);
int DoClearAChaser(int numchaser);
int DoDockChaserTrack(int numchaser, int numtrack);
int chaser_store_in_preset(int numchaser, int presettrk);
int DoModifyChaserTrack(int numchaser, int numtrack);
int DoReportChaserTrack(int numchaser, int numtrack);
