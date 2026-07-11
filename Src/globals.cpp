#include "globals.h"

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
char versionis[72] = {"0.9.2 - juin 2026"};
char nickname_version[48] = {"arpschuino reborn"};
bool init_done = 0; // démarrage pour éviter envoyer data pdt procedure d initialisation
/////////////////////REPERTOIRE/////////////////////////////////////////////////
char rep[255]; // repertoire avec arborescence complete du fichier
// sab 02/03/2014 char mondirectory[200];//endroit de l exe
/** Directory where is the executable file - Répertoire où se trouve l'exécutable **/
char mondirectory[512];
char working_nameis[120];
char rep_saves[10] = {"saves/"};       // '/' portable : valide sous Windows ET Linux/macOS
char nomduspectacle[48] = {"last_save/"};
char video_folder[256];
char my_show_is_coming_from[128] = {"default"};
bool there_is_change_on_show_save_state = 0;
int adapater_is_selected = -1; // numero d adaptateur selectionne pour adresse mac
bool right_click_for_menu = 0;
////////////////////////////////////////////////////////////////////////////////
int index_liste_kbd_custom = 0;
char kbd_custom[256][2];
/////////////////////////////////////////////////////////////////////////////////
int max_window_identity_is = 927;
int index_to_navigate_between_window = 0;
int window_opened[72]; // la liste des fenetres ouvertes modification en 0.4.1 d un size 64 en size 72,
// y compris dans save_show.cpp
int nbre_fenetre_actives = 0;
int temp_report_window[72];
