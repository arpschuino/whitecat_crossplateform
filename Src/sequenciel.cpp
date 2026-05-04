#include "sequenciel.h"

////////////////////////TIME////////////////////////////////////////////////////
bool index_time = 0;
int xtime_window = 100;
int ytime_window = 100;
bool index_play_chrono = 0;
bool index_type_of_time_to_affect[4]; // 0=DIN 1=IN 2=DOUT 3=OUT
int time_centiemes = 0;
int time_secondes = 0;
int time_minutes = 0;
bool index_affect_time = 0;
bool show_im_recording_a_time = 0;
int time_wheel_datatype_is = 1; // 0=minutes 1=secondes 2=dixiemes
float angle_timesnap_min = 0.0;
float angle_timesnap_sec = 0.0;
float angle_timesnap_dix = 0.0;
char string_conversion_timeis[12];
char string_time_memonstage[4][52];
char string_time_mem8after[35][4][52];
//////////////////////SEQUENTIEL////////////////////////////////////////////////
bool index_window_sequentiel = 0;
int xseq_window = 50;
int yseq_window = 10;
int nbre_memoires_visualisables_en_preset = 8;
inline int calc_hauteur_sequenciel(int n) {
    return 180 + 35 * (n + 1) + 40;
}
int hauteur_globale_sequenciel = calc_hauteur_sequenciel(nbre_memoires_visualisables_en_preset);
bool MemoiresExistantes[10000];
unsigned char Memoires[10000][514];
int position_onstage = 0;
int position_preset = 0;
int mem_before_one = 0;
int other_mem_in_loop = 0;
char descriptif_memoires[10000][25];
char annotation_memoires[10000][25];
bool MemoiresExclues[10000];
char string_ratio_x1x2[4];
char str_crossfade_speed_is[6];
bool index_text_auto_close = 0;
float Times_Memoires[10000][4]; // 0=DIN 1=IN 2=DOUT 3=OUT
bool Links_Memoires[10000];
bool index_link_is_on = 0;
int index_blink_change_memories = 0;
float default_time = 3.0;
int niveauX1 = 255;
int niveauX2 = 0;
bool call_preset = 0;
bool call_stage = 0;
char string_mem_before_one[6];
char string_mem_onstage[6];
char string_mem_preset[6];
char string_next_mem[6];
// cross manuel
bool index_x1_x2_together = 1;
int ratio_X1X2_together = 0;
int remapX1[255];
int remapX2[255];
// cross fade
bool index_go = 0;
bool index_pause = 0;
bool index_go_back = 0;
int crossfade_start_time = 0;
int crossfade_elapsed_time = 0;
int crossfade_done_time = 0;
int crossfade_speed = 64;
bool auto_reset_crossfade_speed_on_link = 0;
float floatX1 = 0.0;
float floatX2 = 0.0;
float fraction_X2_in = 0.0;
float fraction_X1_out = 0.0;
int crossfade_time_delay_in = 0;
int crossfade_time_delay_out = 0;
float fraction_goback_X2_in = 0.0;
float fraction_goback_X1_out = 0.0;
float alpha_blinker = 0.0;
float alpha_smooth_blinker = 0.0;
float default_time_back = 3.0;
int ratio_cross_manuel[10000];
bool index_get_back_faders_need_to_be_done = 0;
char cross_to_come[52];
char cross_din[12];
char cross_in[12];
char cross_dout[12];
char cross_out[12];
// time left affichage sur crossfade
float report_dIn = 0.0;
float report_In = 0.0;
float report_dOut = 0.0;
float report_Out = 0.0;
float time_left = 0.0;
float totalTimeCrossfade = 0.0;
char string_time_left_is[52];
char string_total_time_is[52];
char string_last_copy_mem[25];
