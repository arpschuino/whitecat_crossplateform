#pragma once

//////////////WIZARD /////////////////////////
extern int Xwizard;
extern int Ywizard;
extern bool index_show_wizard_window;
extern bool index_over_wizard_window;
extern bool index_click_move_wizardwindow;
extern bool index_wizard_ch_mode;

//////////////WIZARD CH/////////////////////////
extern int wizard_action;
extern bool wizard_amplitude_is_global;
extern int wizard_level_is;
extern int wizard_from_mem;
extern int wizard_to_mem;
extern int number_inchannels;
extern int number_outchannels;
extern bool index_do_wizard_ch;
extern bool wizard_buffer_in[514];
extern char tmp_wizbuffer[2160];
extern char show_buff_wizIN[37][73];
extern int position_ligne_wizard_in;
extern bool wizard_buffer_out[514];
extern char show_buff_wizOUT[37][73];
extern int position_ligne_wizard_out;

////////////WIZARD MEM//////////////////////////
extern int wizard_action_mem;
extern int position_ligne_wizard_mem;
extern bool index_do_wizard_mem;
extern int nbre_de_mems_manipulees;
extern int mode_rebuild;
extern int variable_wizard_mem;
extern bool Wiz_MemoiresExistantes[10000];
extern unsigned char Wiz_Memoires[10000][514];
extern float Wiz_Times_Memoires[10000][4];
extern bool Wiz_Links_Memoires[10000];
extern int Wiz_Banger_Memoire[10000];
extern char Wiz_descriptif_memoires[10000][25];
extern char Wiz_annotation_memoires[10000][25];
extern bool MemoiresDetruites[10000];
extern bool index_wizard_do_reload_from_disk;

int clear_wizard_store_ch_in();
int clear_wizard_store_ch_out();
int wizard_store_channel_in();
int wizard_store_channel_out();
int Box_mem_to_mem(int xb, int yb);
int fenetre_wizard(int xb, int yb);
