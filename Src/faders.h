#pragma once

////////////////////FADERS//////////////////////////////////////////////////////
extern int max_faders;
extern float scroll_faderspace;
extern float facteur_scroll_channel_space;
extern float deltaTime;
extern int goto_scroll_faderspace;
// Damper effect
extern float scroll_target_val;
extern float scroll_damp_constant;
extern float scroll_spring_constant;
extern float scroll_val;
extern float scroll_val_vel;
const float scroll_dt = 0.1;
extern bool index_stop_move;
extern float light_midi_do_order[48];
extern int XFader;
extern int YFader;

extern int LargeurEspaceFaderSize;
extern bool index_moving_fader_space;
extern bool index_moving_x_slide;
extern bool index_moving_y_slide;

extern int dock_used_by_fader_is[48];
extern int previous_dock_used[48];
extern unsigned char Fader[48];
extern unsigned char Fader_before[48];
extern unsigned char Fader_previous[48];

extern bool DockIsSelected[48][6];

extern unsigned char DockTypeIs[48][6];
// 2:artnet IN // 3: DMX IN //4 video // 5 memoire // 6 audio vol 7 pan 8 pitch / 9: vide / 10:
// direct chan mode 11:FX // 12: Grid // 13: groupe faders // 15: Draw // 16: echo
extern unsigned char DockNetIs[48][6];

extern char string_docktypnet[8];
extern char string_docktypmem[8];
extern char string_docktypaudiovol[8];
extern char string_docktypaudiopan[8];
extern char string_docktypaudiopicth[8];
extern char string_docktypdchan[8];
extern char string_docktypfx[16];

extern int ChaserAffectedToDck[48][6];
extern int actual_master_lock;
extern int colorpreset_linked_to_dock[8][2];
extern char string_docktypvideo[8];
extern char DockName[48][6][50];
extern unsigned char FaderDockContains[48][6][514];
extern unsigned char FaderDoDmx[49][514];
extern bool show_who_is_in_FADER_DOCK[514];
extern bool FaderLocked[48];
extern unsigned char OldFaderLockProc[48];
extern int locklevel;
extern bool lock_preset[8];
extern int lock_preset_selected_for_record;
extern bool FaderLocked_Preset[8][48];
extern bool LockFader_is_FullLevel_Preset[8][48];
extern unsigned char StateOfFaderBeforeLock_Preset[8][48];
extern int master_lock_preset[8];
extern unsigned char StateOfFaderBeforeLock[48];
extern bool LockFader_is_FullLevel[48];
extern int highest_level_comes_from_fader[514];
extern int DockHasMem[48][6];
// LFO
extern bool is_dock_for_lfo_selected[49][6];
extern float time_per_dock[49][6][4];
extern int lfo_mode_is[49];
extern bool lfo_cycle_is_on[49];
extern bool lfo_cycle_steps[49];
extern char string_time_is_in_the_dock[49][60];
extern float fraction_lfo_in[49];
extern float fraction_lfo_out[49];
extern float faders_in_float[49];
extern int lfo_speed[49];
extern int before_lfo_speed[49];
extern bool lfo_running_is_upward[49];
extern bool lfo_do_next_step[49][2];
extern bool index_lfoing[49];
extern int time_delay_in[49];
extern int time_delay_out[49];
extern int start_time_for_delays[49];
extern int actual_time;
// StopPos
extern bool StopPosOn[48];
extern unsigned int LevelStopPos[48];
extern unsigned int PreviousLevelStopPos[48];

extern bool ActionnateStopOn[48];
extern bool do_light_setpos[48];

// courbe dans faders
extern int FaderCurves[48];
extern int before_bang_FaderCurves[48];
extern bool index_ask_curv_to_fader;
extern int curve_asked_for_fader;

extern bool autolaunch[48];

extern bool fader_damper_is_on[48];

/// MODE DIRECT CHANNEL POUR LES FADERS
extern int FaderDirectChan[48][6];
extern unsigned char beforeloop_for_directch[48];
extern bool index_direct_chan;
extern bool index_do_record_direct_ch;
extern bool index_fader_is_manipulated[48];

extern bool multiple_direct_chan;
// mini faders panels
extern bool index_show_minifaders;
extern bool index_over_minifaders;
extern bool index_click_move_minifaderswindow;
extern int xMinifaders;
extern int yMinifaders;
extern bool minifaders_selected[48];
extern int position_minifader_selected;
extern char report_minifader_str[64];
extern char str_tmp_minidock_dock[16];
extern char str_minifader_feedback[16][64];
extern bool FaderIsFlash[48];
extern bool FaderIsFlashBefore[48];
extern unsigned char LevelFaderBeforeFlash[48];
extern bool minifaders_preset_selection[8][48];
extern int mf_preset_selected_is;
extern bool minifader_preset_is_empty[8];

extern int before_dock_used_by_fader_is[48];
extern bool do_light_minifaders_commands[24];
extern bool index_record_minifader_preset;
extern bool index_clear_minifader_preset;
extern bool index_record_minifader_lockpreset;
extern int minifader_preset_is;
extern int minifader_lockpreset_is;
extern int over_minifader;

///////////////////////////FADERS SNAPSHOTS GENERAL//////////////////////////////////
extern unsigned char SnapFader[49];
extern bool SnapFaderLocked[49];
extern bool Snapis_dock_for_lfo_selected[49][6];
extern int Snaplfo_mode_is[49];
extern bool Snaplfo_cycle_is_on[49];
extern bool Snaplfo_cycle_steps[49];
extern bool Snaplfo_do_next_step[49][2];
extern int Snaplfo_speed[49];
extern bool stockage_all_at_zero_state[6];
