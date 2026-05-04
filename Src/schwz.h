#pragma once

// IMPORT SCHWZ////////////////////////////////////////////////////////////////////////
extern char f_cues[25];
extern char f_boolcues[25];
extern char f_autogo[25];
extern char f_times[25];
extern char f_patch[25];
extern char f_descriptif[25];
extern char f_shadow_sub[30];
extern int SchwzMemoires[121][5011];
extern bool Schwzautogo[5011];
extern float SchwzMemTime[5011][4];
extern bool SchwzMemoiresExistantes[5011];
extern char Schwzdescriptif_mem[5011][13];
extern int SchwzPatch[513];
extern int Schwzshadow_sub[8][5][121];

extern unsigned int SchwzMemoires_size;
extern unsigned int SchwzMemTime_size;
extern unsigned int SchwzPatch_size;
extern unsigned int SchwzMemoiresExistantes_size;
extern unsigned int Schwzautogo_size;
extern unsigned int Schwzdescriptif_size;
extern unsigned int Schwzsize_of_shadow_sub;

extern bool isSchwz, isASCII, isPdf, isAlq;

extern bool specify_who_to_save_PDF[36];
