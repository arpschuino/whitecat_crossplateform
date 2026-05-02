#pragma once

//////////////WIZARD /////////////////////////
int Xwizard = 100;
int Ywizard = 100;
bool index_show_wizard_window = 0;
bool index_over_wizard_window = 0;
bool index_click_move_wizardwindow = 0;
bool index_wizard_ch_mode = 1;

//////////////WIZARD CH/////////////////////////
int wizard_action = 0; // 0 Set at level / 1 Add / 2 Reduce / 3 exchange
bool wizard_amplitude_is_global = 1;
int wizard_level_is = 5;
int wizard_from_mem = 0;
int wizard_to_mem = 0;
int number_inchannels = 0;
int number_outchannels = 0;
bool index_do_wizard_ch = 0;
bool wizard_buffer_in[514];
char tmp_wizbuffer[2160];
char show_buff_wizIN[37][73];
int position_ligne_wizard_in = 0;
bool wizard_buffer_out[514];
char show_buff_wizOUT[37][73];
int position_ligne_wizard_out = 0;

////////////WIZARD MEM//////////////////////////
int wizard_action_mem = 0;
int position_ligne_wizard_mem = 0;
bool index_do_wizard_mem = 0;
int nbre_de_mems_manipulees = 0;
int mode_rebuild = 0; // 0 digit 1 unit 2 dizaines
int variable_wizard_mem = 0;
bool Wiz_MemoiresExistantes[10000];
unsigned char Wiz_Memoires[10000][514];
float Wiz_Times_Memoires[10000][4];
bool Wiz_Links_Memoires[10000];
int Wiz_Banger_Memoire[10000];
char Wiz_descriptif_memoires[10000][25];
char Wiz_annotation_memoires[10000][25];
bool MemoiresDetruites[10000];
bool index_wizard_do_reload_from_disk = 0;
