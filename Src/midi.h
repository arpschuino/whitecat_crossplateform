#pragma once

//////////////////////MIDI///////////////////////////////////////////////////////
extern bool index_midi_mute;
extern bool index_auto_mute_cuelist_speed;
extern int index_midi_auto_desaffect;
extern short myRefNum;
extern bool index_midi_clock_on;

extern char TblLibEv[256][20];
extern char my_midi_string[64];
extern char midi_historic[256][64];
extern int midi_historic_data[256][4];
extern int position_midi_historic;

extern char my_midi_original_string[64];
extern char my_midi_out_string[128];
extern char tableau_peripheriques_in[32][64];
extern bool tableau_peripheriques_indexs_in[16];
extern char tableau_peripheriques_out[32][64];
extern bool tableau_peripheriques_indexs_out[16];
extern char string_nbre_de_devices[32];
extern int compt_midi_in;
extern int compt_midi_out;
extern int compt_first_device_out;
extern int compt_first_device_inout;
extern int nbre_devices_in;
extern int nbre_devices_out;
extern int do_connect_out[32];
extern bool midi_out_is_connected[32];
extern int do_connect_in[16];
extern bool midi_in_is_connected[32];
extern bool index_midi_global_thruth;
extern char typ[50];
extern int midi_keyboard_wait;
extern int midi_wait;
extern int duree_note;
extern char string_last_midi_id[128];
extern char string_shortview_midi[24];
extern int isport;
extern int ischan;
extern int ispitch;
extern int isvel;
extern int istyp;
extern byte isrefnum;
extern int miditable[3][3072];
extern int over_fader, over_dock;
extern int midi_levels[3072];
extern bool midi_send_out[3072];
extern bool index_send_midi_out[3072];
extern bool index_global_midi_send_on_faders;
extern bool do_light_midi_send_on_faders;
extern int midi_page;
extern int Midi_Faders_Affectation_Type;
extern int Midi_Faders_Affectation_Mode;
extern bool toggle_numerical_midi_way;
extern int fakemidichan;
extern int fakemidipitch;
extern int fakemiditype;

extern bool index_midi_affectation_autoclose;
extern char thetypinfo[12];
extern int type_of_midi_button;
extern bool cheat_key_off;
extern bool cheat_key_off_to_key_on;

extern int bpm_personnal[16];
extern int relativ_encoder_midi_clock_value;
extern bool clocklevel_absolutemode;
extern int clock_level_is;
extern float clock_vx;
extern float clock_vy;
extern float angle_snap_clock;
extern float position_curseur_clock_x;
extern float position_curseur_clock_y;

extern bool is_raccrochage_midi_remote[3072];
extern int val_raccrochage_midi[3072];

extern bool refresh_midi_chasers;
extern bool index_midi_auto_demute;
extern bool index_midi_mute_on_lfo;

////////////MIDI CHANGE SIGNAL///////////////////////////////////////////////////
extern int line_midi_changesignal;
extern bool midi_changesignal_scroll_dragging;
extern int midi_changesignal_scroll_drag_start_y;
extern int midi_changesignal_scroll_drag_start_scroll;
extern int last_scroll_mouse_for_midi_changesignal;
extern char string_midichan[16];
extern int change_vel_midichan_selected;
extern int midi_change_vel_type[16][128];
extern int tempvel;
extern bool midi_recognize_on_off[16][128];

/////////////////////MIDI LAUNCHPAD/////////////////////////////////////////////
extern bool enable_launchpad;
extern int midi_duree_launchpad;
extern int temp_launchpad;
extern bool midi_launchpad_state[3072];
extern bool midi_launchpad_state_before[3072];
extern bool launchpad_impulse_type_is[3072];
extern int launchpad_color_defined[3072];
extern int launchpad_color[16];
extern int lch_orange;
extern int lch_green;
extern int lch_yellow;
extern int lch_red;
extern int lch_ambre;
extern int lch_orange_fonce;
extern int facteur_cycling;
extern bool entered_main;
extern bool launchpad_is_a_cycling_effect[3072];
extern bool midi_needs_no_key_on_key_off[3072];
extern bool launchpad_buffer;

extern char list_midi_affect[3072][36];
