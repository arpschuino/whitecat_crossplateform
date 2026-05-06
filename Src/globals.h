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

extern char versionis[72];
extern char nickname_version[48];

extern bool init_done;
/////////////////////REPERTOIRE/////////////////////////////////////////////////
extern char rep[255];
// sab 02/03/2014 char mondirectory[200];//endroit de l exe
/** Directory where is the executable file - Répertoire où se trouve l'exécutable **/
extern char mondirectory[512];
extern char working_nameis[120];
extern char rep_saves[10];
extern char nomduspectacle[48];
extern char video_folder[256];
extern char my_show_is_coming_from[128];

extern bool there_is_change_on_show_save_state;

extern int adapater_is_selected;
extern bool right_click_for_menu;

////////////////////////////////////////////////////////////////////////////////
extern int index_liste_kbd_custom;
extern char kbd_custom[256][2];

/////////////////////////////////////////////////////////////////////////////////
enum WINDOW {
    W_SAVEREPORT = 900,
    W_DRAW = 901,
    W_TRICHROMY = 902,
    W_NUMPAD = 903,
    W_TRACKINGVIDEO = 904,
    W_ARTPOLLREPLY = 905,
    W_FADERS = 906,
    W_PATCH = 907,
    W_TIME = 908,
    W_SEQUENCIEL = 909,
    W_ASKCONFIRM = 911,
    W_PLOT = 912,
    W_ECHO = 913,
    W_LIST = 914,
    W_SAVE = 915,
    W_MAINMENU = 916,
    W_BANGER = 917,
    W_ALARM = 918,
    W_AUDIO = 919,
    W_CFGMENU = 920,
    W_WIZARD = 921,
    W_MINIFADERS = 922,
    W_CHASERS = 923,
    W_GRID = 926
};
extern int max_window_identity_is;
extern int index_to_navigate_between_window;

extern int window_opened[72];
// y compris dans save_show.cpp
extern int nbre_fenetre_actives;
extern int temp_report_window[72];
extern int BPS_RATE;
extern int dmxINrate;
extern int ARDUINO_RATE;
extern int ticks_passed;
extern int midi_BPM;
extern volatile int ticker_midi_clock_rate;
extern volatile bool calculation_on_faders_done;
