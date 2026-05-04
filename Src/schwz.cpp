#include "schwz.h"

// IMPORT SCHWZ////////////////////////////////////////////////////////////////////////
char f_cues[25] = {"cues.dat"};
char f_boolcues[25] = {"bool_cues.dat"};
char f_autogo[25] = {"autogo.dat"};
char f_times[25] = {"times.dat"};
char f_patch[25] = {"patch.dat"};
char f_descriptif[25] = {"descriptif.dat"};
char f_shadow_sub[30] = {"shadows_subs.dat"};
int SchwzMemoires[121][5011];
bool Schwzautogo[5011];
float SchwzMemTime[5011][4];
bool SchwzMemoiresExistantes[5011];
char Schwzdescriptif_mem[5011][13];
int SchwzPatch[513];
int Schwzshadow_sub[8][5][121];
unsigned int SchwzMemoires_size = 121 * 5011;
unsigned int SchwzMemTime_size = 5011 * 4;
unsigned int SchwzPatch_size = 513 * 1;
unsigned int SchwzMemoiresExistantes_size = 5011 * 1;
unsigned int Schwzautogo_size = 5011 * 1;
unsigned int Schwzdescriptif_size = 5011 * 13;
unsigned int Schwzsize_of_shadow_sub = 40 * 121;
bool isSchwz = 0, isASCII = 0, isPdf = 0, isAlq = 0;
bool specify_who_to_save_PDF[36];
