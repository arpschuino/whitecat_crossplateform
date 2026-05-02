#pragma once

//////////////////////MIDI///////////////////////////////////////////////////////
bool index_midi_mute = 0;
bool index_auto_mute_cuelist_speed = 0;
int index_midi_auto_desaffect = 0;
short myRefNum;
bool index_midi_clock_on = 0;

char TblLibEv[256][20];
char my_midi_string[64];
char midi_historic[256][64];
int midi_historic_data[256][4];
int position_midi_historic = 0;

char my_midi_original_string[64];
char my_midi_out_string[128];
char tableau_peripheriques_in[32][64];
bool tableau_peripheriques_indexs_in[16];
char tableau_peripheriques_out[32][64];
bool tableau_peripheriques_indexs_out[16];
char string_nbre_de_devices[32];
int compt_midi_in = 0;
int compt_midi_out = 0;
int compt_first_device_out = 0;
int compt_first_device_inout = 0;
int nbre_devices_in = 0;
int nbre_devices_out = 0;
int do_connect_out[32];
bool midi_out_is_connected[32];
int do_connect_in[16];
bool midi_in_is_connected[32];
bool index_midi_global_thruth = 0;
char typ[50];
int midi_keyboard_wait = 100;
int midi_wait = 150;
int duree_note = 10;
char string_last_midi_id[128];
char string_shortview_midi[24];
int isport = 0;
int ischan = 0;
int ispitch = 0;
int isvel = 0;
int istyp = 0;
byte isrefnum = 0;
int miditable[3][3072];
int over_fader = 0, over_dock = 0;
int midi_levels[3072];
bool midi_send_out[3072];
bool index_send_midi_out[3072];
bool index_global_midi_send_on_faders = 0;
bool do_light_midi_send_on_faders = 0;
int midi_page = 0;
int Midi_Faders_Affectation_Type = 0;
int Midi_Faders_Affectation_Mode = 0;
bool toggle_numerical_midi_way = 0;
int fakemidichan = 0;
int fakemidipitch = 1;
int fakemiditype = 4;

bool index_midi_affectation_autoclose = 1;
char thetypinfo[12];
int type_of_midi_button = 0;
bool cheat_key_off = 0;
bool cheat_key_off_to_key_on = 1;

int bpm_personnal[16];
int relativ_encoder_midi_clock_value = 10;
bool clocklevel_absolutemode = 0;
int clock_level_is = 0;
float clock_vx;
float clock_vy;
float angle_snap_clock;
float position_curseur_clock_x;
float position_curseur_clock_y;

bool is_raccrochage_midi_remote[3072];
int val_raccrochage_midi[3072];

bool refresh_midi_chasers = 0;
bool index_midi_auto_demute = 0;
bool index_midi_mute_on_lfo = 0;

////////////MIDI CHANGE SIGNAL///////////////////////////////////////////////////
int line_midi_changesignal = 0;
char string_midichan[16];
int change_vel_midichan_selected = 0;
int midi_change_vel_type[16][128];
int tempvel = 0;
bool midi_recognize_on_off[16][128];

/////////////////////MIDI LAUNCHPAD/////////////////////////////////////////////
bool enable_launchpad = 0;
int midi_duree_launchpad = 10;
int temp_launchpad = 0;
bool midi_launchpad_state[3072];
bool midi_launchpad_state_before[3072];
bool launchpad_impulse_type_is[3072];
int launchpad_color_defined[3072];
int launchpad_color[16];
int lch_orange = 63;
int lch_green = 60;
int lch_yellow = 62;
int lch_red = 15;
int lch_ambre = 31;
int lch_orange_fonce = 30;
int facteur_cycling = 0;
bool entered_main = 0;
bool launchpad_is_a_cycling_effect[3072];
bool midi_needs_no_key_on_key_off[3072];
bool launchpad_buffer = 0;

char list_midi_affect[3072][36];
