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
extern bool index_save_mode_export_or_binary; // met la fenetre save en mode export=1 ou binary classic=0
extern int idf;                               // curseur position retour info save load
extern bool specify_who_to_save_load[80];         // 72 d utiliser max
extern bool preset_specify_who_to_save_load[80][4];
extern bool index_save_global_is; // 1 global 0 detailled
// coordonnees fenetre save load
extern int report_SL_X;
extern int report_SL_Y;
extern int default_report_SL_X;
extern int default_report_SL_Y;
extern bool there_is_an_error_on_save_load;
extern bool index_show_save_load_report;
extern bool b_report_error[256];
extern char string_save_load_report[256][64]; // 256 slot de report de 64 char
extern int position_view_line;            // pour defilement affichage
extern char string_typeexport_view[24];
extern bool index_is_saving; // pour eviter de planter les pointeurs en faisant deux saves simulatnnés
extern int timer_save_tmp;
extern int temps_sauvegarde_tmp; // secondes
extern bool index_please_do_not_save;
/////////////SAVE MENU//////////////////////
// EXPORTS IMPORTS
extern int index_export_choice;
extern char list_import_files[128][72];
extern int line_import;
extern int nbre_import_files; // total fichiers import (pour ascenseur)
extern int importfile_selected; // num de ligne selectionné
extern char importfile_name[72];
extern bool enable_export; // pour les choix schwz / pdf / txt / ascii
extern bool enable_import;
////BINARIES
extern char list_save_files[128][72];
extern int line_save;
extern int nbre_save_files; // total dossiers save (pour ascenseur)
extern int savefile_selected; // num de ligne selectionné
extern char savefile_name[72];
// état drag ascenseurs fenêtre save
extern bool save_import_scroll_dragging;
extern int  save_import_scroll_drag_start_y;
extern int  save_import_scroll_drag_start_scroll;
extern bool save_binary_scroll_dragging;
extern int  save_binary_scroll_drag_start_y;
extern int  save_binary_scroll_drag_start_scroll;
