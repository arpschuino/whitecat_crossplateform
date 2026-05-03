/*-------------------------------------------------------------------------------------------------------------
                                 |
          CWWWWWWWW              | Copyright (C) 2009-2013  Christoph Guillermet
       WWWWWWWWWWWWWWW           |
       WWWWWWWWWWWWWWW           |               2026       Jacques Bouault - arpschuino.fr
     WWWWWWWWWWWWWWWWWWW         | This file is part of White Cat.
    WWWWWWWWWWWWWWWWWCWWWW       |
   WWWWWWWWWWWWWWWWW tWWWWW      | White Cat is free software: you can redistribute it and/or modify
  WWWW   WWWWWWWWWW  tWWWWWW     | it under the terms of the GNU General Public License as published by
 WWWWWt              tWWWWWWa    | the Free Software Foundation, either version 2 of the License, or
 WWWWWW               WWWWWWW    | (at your option) any later version.
WWWWWWWW              WWWWWWW    |
WWWWWWWW               WWWWWWW   | White Cat is distributed in the hope that it will be useful,
WWWWWWW               WWWWWWWW   | but WITHOUT ANY WARRANTY; without even the implied warranty of
WWWWWWW      CWWW    W WWWWWWW   | MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
WWWWWWW            aW  WWWWWWW   | GNU General Public License for more details.
WWWWWWWW           C  WWWWWWWW   |
 WWWWWWWW            CWWWWWWW    | You should have received a copy of the GNU General Public License
 WWWWWWWWW          WWWWWWWWW    | along with White Cat.  If not, see <http://www.gnu.org/licenses/>.
  WWWWWWWWWWC    CWWWWWWWWWW     |
   WWWWWWWWWWWWWWWWWWWWWWWW      |
    WWWWWWWWWWWWWWWWWWWWWW       |
      WWWWWWWWWWWWWWWWWWa        |
        WWWWWWWWWWWWWWW          |
           WWWWWWWWt             |
                                 |
---------------------------------------------------------------------------------------------------------------*/

#pragma once

//////////////////////SAVE LOAD/////////////////////////////////////////////////
bool index_save_mode_export_or_binary = 0; // met la fenetre save en mode export=1 ou binary classic=0
int idf = 0;                               // curseur position retour info save load
bool specify_who_to_save_load[80];         // 72 d utiliser max
bool preset_specify_who_to_save_load[80][4];
bool index_save_global_is = 1; // 1 global 0 detailled
// coordonnees fenetre save load
int report_SL_X = 30;
int report_SL_Y = 40;
int default_report_SL_X = 30;
int default_report_SL_Y = 30;
bool there_is_an_error_on_save_load = 0;
bool index_show_save_load_report = 0;
bool b_report_error[256];
char string_save_load_report[256][64]; // 256 slot de report de 64 char
int position_view_line = 0;            // pour defilement affichage
char string_typeexport_view[24];
bool index_is_saving = 0; // pour eviter de planter les pointeurs en faisant deux saves simulatnnés
int timer_save_tmp = 0;
int temps_sauvegarde_tmp = 15 * 60; // secondes
bool index_please_do_not_save = 0;
/////////////SAVE MENU//////////////////////
// EXPORTS IMPORTS
int index_export_choice = 0;
char list_import_files[128][72];
int line_import = 0;
int importfile_selected = 0; // num de ligne selectionné
char importfile_name[72];
bool enable_export = 0; // pour les choix schwz / pdf / txt / ascii
bool enable_import = 0;
////BINARIES
char list_save_files[128][72];
int line_save = 0;
int savefile_selected = 0; // num de ligne selectionné
char savefile_name[72];
