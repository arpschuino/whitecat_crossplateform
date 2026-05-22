#pragma once

////////////////////////TIME////////////////////////////////////////////////////
extern bool index_time;
extern int xtime_window;
extern int ytime_window;
extern bool index_play_chrono;
extern bool index_type_of_time_to_affect[4];
extern int time_centiemes;
extern int time_secondes;
extern int time_minutes;
extern bool index_affect_time;
extern bool show_im_recording_a_time;
extern int time_wheel_datatype_is;
extern float angle_timesnap_min;
extern float angle_timesnap_sec;
extern float angle_timesnap_dix;
extern char string_conversion_timeis[12];
extern char string_time_memonstage[4][52];
extern char string_time_mem8after[35][4][52];

//////////////////////SEQUENTIEL////////////////////////////////////////////////
extern bool index_window_sequentiel;
extern int xseq_window;
extern int yseq_window;
extern int nbre_memoires_visualisables_en_preset;
inline int calc_hauteur_sequenciel(int n) {
    return 180 + 35 * (n + 1) + 40;
}
extern int hauteur_globale_sequenciel;
extern bool MemoiresExistantes[10000];
extern unsigned char Memoires[10000][514];
extern int position_onstage;
extern int position_preset;
extern int mem_before_one;
extern int other_mem_in_loop;
extern char descriptif_memoires[10000][50];
extern char annotation_memoires[10000][50];
extern bool MemoiresExclues[10000];
extern char string_ratio_x1x2[4];
extern char str_crossfade_speed_is[6];
extern bool index_text_auto_close;
extern float Times_Memoires[10000][4];
extern bool Links_Memoires[10000];
extern bool index_link_is_on;
extern int index_blink_change_memories;
extern float default_time;
extern int niveauX1;
extern int niveauX2;
extern bool call_preset;
extern bool call_stage;
extern char string_mem_before_one[6];
extern char string_mem_onstage[6];
extern char string_mem_preset[6];
extern char string_next_mem[6];
// cross manuel
extern bool index_x1_x2_together;
extern int ratio_X1X2_together;
extern int remapX1[255];
extern int remapX2[255];
// cross fade
extern bool index_go;
extern bool index_pause;
extern bool index_go_back;
extern int crossfade_start_time;
extern int crossfade_elapsed_time;
extern int crossfade_done_time;
extern int crossfade_speed;
extern bool auto_reset_crossfade_speed_on_link;
extern float floatX1;
extern float floatX2;
extern float fraction_X2_in;
extern float fraction_X1_out;
extern int crossfade_time_delay_in;
extern int crossfade_time_delay_out;
extern float fraction_goback_X2_in;
extern float fraction_goback_X1_out;
extern float alpha_blinker;
extern float alpha_smooth_blinker;
extern float alpha_blinker_slow;
extern float default_time_back;
extern int ratio_cross_manuel[10000];
extern bool index_get_back_faders_need_to_be_done;
extern char cross_to_come[52];
extern char cross_din[12];
extern char cross_in[12];
extern char cross_dout[12];
extern char cross_out[12];
// time left affichage sur crossfade
extern float report_dIn;
extern float report_In;
extern float report_dOut;
extern float report_Out;
extern float time_left;
extern float totalTimeCrossfade;
extern char string_time_left_is[52];
extern char string_total_time_is[52];
extern char string_last_copy_mem[25];
