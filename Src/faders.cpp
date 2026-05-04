#include "graphics_backend.h"
using namespace ol;
#include "display.h"
#include "faders.h"

////////////////////FADERS//////////////////////////////////////////////////////
int max_faders = 48;
float scroll_faderspace = 0.0;
float facteur_scroll_channel_space = 10.0;
float deltaTime = 0.0;
int goto_scroll_faderspace = 0;
// Damper effect
float scroll_target_val = 0.0;
float scroll_damp_constant = 0.1;
float scroll_spring_constant = 1;
float scroll_val = 0;     // value
float scroll_val_vel = 0; // velocity
bool index_stop_move = 0;
float light_midi_do_order[48];
int XFader = 200;
int YFader = 400;
int LargeurEspaceFaderSize = largeur_ecran;
bool index_moving_fader_space = 0;
bool index_moving_x_slide = 0;
bool index_moving_y_slide = 0;
int dock_used_by_fader_is[48];
int previous_dock_used[48]; // pour autolaunch sur banger, dock + - et animations
unsigned char Fader[48];
unsigned char Fader_before[48];
unsigned char Fader_previous[48]; // pour chasers autolaunch
bool DockIsSelected[48][6];
unsigned char DockTypeIs[48][6]; // 0: dock niveau normal // 1: trichro
// 2:artnet IN // 3: DMX IN //4 video // 5 memoire // 6 audio vol 7 pan 8 pitch / 9: vide / 10:
// direct chan mode 11:FX // 12: Grid // 13: groupe faders // 15: Draw // 16: echo
unsigned char DockNetIs[48][6]; // num Universe artnet(0-15) ou Config Port UDP(0-5)
char string_docktypnet[8];
char string_docktypmem[8];
char string_docktypaudiovol[8];
char string_docktypaudiopan[8];
char string_docktypaudiopicth[8];
char string_docktypdchan[8];
char string_docktypfx[8];
int ChaserAffectedToDck[48][6];
int actual_master_lock = 0;
int colorpreset_linked_to_dock[8][2];
char string_docktypvideo[8];
char DockName[48][6][25];
unsigned char FaderDockContains[48][6][514];
unsigned char FaderDoDmx[49][514]; // sortie de chaque fader // 49 pour éviter débordement ECHO SNAP
bool show_who_is_in_FADER_DOCK[514];
bool FaderLocked[48];
unsigned char OldFaderLockProc[48];
int locklevel = 0;
bool lock_preset[8];
int lock_preset_selected_for_record = 0;
bool FaderLocked_Preset[8][48];
bool LockFader_is_FullLevel_Preset[8][48];
unsigned char StateOfFaderBeforeLock_Preset[8][48];
int master_lock_preset[8];
unsigned char StateOfFaderBeforeLock[48];
bool LockFader_is_FullLevel[48];
int highest_level_comes_from_fader[514];
int DockHasMem[48][6];
// LFO
bool is_dock_for_lfo_selected[49][6];
float time_per_dock[49][6][4]; // WAIT IN- IN - WAIT OUT- OUT
int lfo_mode_is[49];           // 0 NO LFO- 1 UP ONCE - 2 DOWN ONCE
bool lfo_cycle_is_on[49];
bool lfo_cycle_steps[49];
char string_time_is_in_the_dock[49][60];
float fraction_lfo_in[49];
float fraction_lfo_out[49];
float faders_in_float[49];
int lfo_speed[49];
int before_lfo_speed[49];
bool lfo_running_is_upward[49];
bool lfo_do_next_step[49][2]; // 0 backward 1 forward
bool index_lfoing[49];
int time_delay_in[49];
int time_delay_out[49];
int start_time_for_delays[49];
int actual_time;
// StopPos
bool StopPosOn[48];
unsigned int LevelStopPos[48];
unsigned int PreviousLevelStopPos[48]; // pour banger back
bool ActionnateStopOn[48];
bool do_light_setpos[48];
// courbe dans faders
int FaderCurves[48];
int before_bang_FaderCurves[48];
bool index_ask_curv_to_fader = 0;
int curve_asked_for_fader = 0;
bool autolaunch[48];
bool fader_damper_is_on[48];
/// MODE DIRECT CHANNEL POUR LES FADERS
int FaderDirectChan[48][6];
unsigned char beforeloop_for_directch[48];
bool index_direct_chan = 0;
bool index_do_record_direct_ch = 0;
bool index_fader_is_manipulated[48];
bool multiple_direct_chan = 0;
// mini faders panels
bool index_show_minifaders = 0;
bool index_over_minifaders = 0;
bool index_click_move_minifaderswindow = 0;
int xMinifaders = 580;
int yMinifaders = 420;
bool minifaders_selected[48];
int position_minifader_selected = 0;
char report_minifader_str[64];
char str_tmp_minidock_dock[16];
char str_minifader_feedback[16][64];
bool FaderIsFlash[48];
bool FaderIsFlashBefore[48];
unsigned char LevelFaderBeforeFlash[48];
bool minifaders_preset_selection[8][48];
int mf_preset_selected_is = 0;
bool minifader_preset_is_empty[8];
int before_dock_used_by_fader_is[48];
bool do_light_minifaders_commands[24];
bool index_record_minifader_preset = 0;
bool index_clear_minifader_preset = 0;
bool index_record_minifader_lockpreset = 0;
int minifader_preset_is = 0;
int minifader_lockpreset_is = 0;
int over_minifader = 0;
///////////////////////////FADERS SNAPSHOTS GENERAL//////////////////////////////////
unsigned char SnapFader[49];
bool SnapFaderLocked[49];
bool Snapis_dock_for_lfo_selected[49][6];
int Snaplfo_mode_is[49];      // 0 NO LFO- 1 UP ONCE - 2 DOWN ONCE
bool Snaplfo_cycle_is_on[49]; // SAW up down
bool Snaplfo_cycle_steps[49];
bool Snaplfo_do_next_step[49][2]; // 0 backward 1 forward
int Snaplfo_speed[49];
bool stockage_all_at_zero_state[6];
