#pragma once
#include <string>

int  scan_savesfolder();
void scan_importfolder(const char* subdir);
int  choose_personnal_preset_binary_save_load(int xs, int ys);
int  do_logical_choose_personnal_preset_binary_save_load(int xs, int ys);
int  check_import_type();
void deroule_repertoire_export_import(int xrep, int yrep, const std::string label);
void do_logical_deroule_repertoire_export_import(int xrep, int yrep, const std::string label);
int  deroule_repertoire_export_import(int xrep, int yrep, char* name_of_rep);
int  do_logical_deroule_repertoire_export_import(int xrep, int yrep, char* name_of_rep);
void deroule_repertoire_classical_save(int xrep, int yrep, const std::string label);
void do_logical_deroule_repertoire_classical_save(int xrep, int yrep, const std::string label);
int  selecteur_PDF_save_solo_global(int xrep, int yrep);
int  do_logical_selecteur_PDF_save_solo_global(int xrep, int yrep);
int  selecteur_binary_save_solo_global(int xrep, int yrep);
int  do_logical_selecteur_binary_save_solo_global(int xrep, int yrep);
int  Save_Menu(int xsave, int ysave);
int  do_logical_Save_Menu(int xsave, int ysave);
