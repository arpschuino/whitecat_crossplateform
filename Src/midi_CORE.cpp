/*-------------------------------------------------------------------------------------------------------------
                                 |
          CWWWWWWWW              | Copyright (C) 2009-2016  Christoph Guillermet
       WWWWWWWWWWWWWWW           | Copyright (C) 2026       Jacques Bouault - arpschuino.fr
     WWWWWWWWWWWWWWWWWWW         |
    WWWWWWWWWWWWWWWWWCWWWW       | This file is part of White Cat.
   WWWWWWWWWWWWWWWWW tWWWWW      |
  WWWW   WWWWWWWWWW  tWWWWWW     | White Cat is free software: you can redistribute it and/or modify
 WWWWWt              tWWWWWWa    | it under the terms of the GNU General Public License as published by
 WWWWWW               WWWWWWW    | the Free Software Foundation, either version 2 of the License, or
WWWWWWWW              WWWWWWW    | (at your option) any later version.
WWWWWWWW               WWWWWWW   |
WWWWWWW               WWWWWWWW   | White Cat is distributed in the hope that it will be useful,
WWWWWWW      CWWW    W WWWWWWW   | but WITHOUT ANY WARRANTY; without even the implied warranty of
WWWWWWW            aW  WWWWWWW   | MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
WWWWWWWW           C  WWWWWWWW   | GNU General Public License for more details.
 WWWWWWWW            CWWWWWWW    |
 WWWWWWWWW          WWWWWWWWW    | You should have received a copy of the GNU General Public License
  WWWWWWWWWWC    CWWWWWWWWWW     | along with White Cat.  If not, see <http://www.gnu.org/licenses/>.
   WWWWWWWWWWWWWWWWWWWWWWWW      |
    WWWWWWWWWWWWWWWWWWWWWW       |
      WWWWWWWWWWWWWWWWWWa        |
        WWWWWWWWWWWWWWW          |
           WWWWWWWWt             |
                                 |
---------------------------------------------------------------------------------------------------------------*/
/**
* \file midi_CORE.cpp
* \brief MIDI Core - rewritten using midi_backend.h (RtMidi)
* \author Christoph Guillermet
* \author Jacques Bouault - arpschuino.fr
* \version {0.9.0}
* \date {2026}
*
* White Cat - MIDI Core
*
* Remplace l'implementation MidiShare par midi_backend.h (RtMidi).
* Les interfaces InitMidi() et QuitMidi() sont conservees pour
* la compatibilite avec le reste du code.
*
* Replaces MidiShare implementation with midi_backend.h (RtMidi).
* InitMidi() and QuitMidi() interfaces are kept for compatibility
* with the rest of the code.
*
**/

#define WC_SKIP_GLOBALS
#include "graphics_backend.h"
using namespace ol;
#include "whitecat.h"
#include "midi_backend.h"

/////////////////////////////////////////////////////////////////////////////////
int midi_init_sepecial_case_key_on()
{
    for(int i=0;i<48;i++)
    {
        midi_needs_no_key_on_key_off[900+i]=1;
    }
    return(0);
}

/////////////////////////////////////////////////////////////////////////////////
int InitTblLibEv()
{
    int i;
    for (i=0;i<255;i++) strcpy(TblLibEv[i],"");

    strcpy(TblLibEv[0],  "Note        ");
    strcpy(TblLibEv[1],  "Key On      ");
    strcpy(TblLibEv[2],  "Key Off     ");
    strcpy(TblLibEv[3],  "Key Press   ");
    strcpy(TblLibEv[4],  "Ctrl Change ");
    strcpy(TblLibEv[5],  "Prog Change ");
    strcpy(TblLibEv[6],  "Chan Press  ");
    strcpy(TblLibEv[7],  "Pitch Wheel ");
    strcpy(TblLibEv[8],  "SongPos     ");
    strcpy(TblLibEv[9],  "SongSel     ");
    strcpy(TblLibEv[10], "Clock       ");
    strcpy(TblLibEv[11], "Start       ");
    strcpy(TblLibEv[12], "Continue    ");
    strcpy(TblLibEv[13], "Stop        ");
    strcpy(TblLibEv[14], "Tune        ");
    strcpy(TblLibEv[15], "Active Sens.");
    strcpy(TblLibEv[16], "Reset       ");
    strcpy(TblLibEv[17], "System Excl.");
    strcpy(TblLibEv[18], "Stream      ");
    strcpy(TblLibEv[19], "Private     ");
    strcpy(TblLibEv[20], "Process     ");
    strcpy(TblLibEv[21], "DProcess    ");
    strcpy(TblLibEv[22], "QuarterFrame");
    strcpy(TblLibEv[23], "Ctrl 14bits ");
    strcpy(TblLibEv[24], "NonRegParam.");
    strcpy(TblLibEv[25], "Reg Param   ");
    strcpy(TblLibEv[26], "Seq Number  ");
    strcpy(TblLibEv[27], "Text        ");
    strcpy(TblLibEv[28], "Copyright   ");
    strcpy(TblLibEv[29], "Seq Name    ");
    strcpy(TblLibEv[30], "Instr Name  ");
    strcpy(TblLibEv[31], "Lyric       ");
    strcpy(TblLibEv[32], "Marker      ");
    strcpy(TblLibEv[33], "CuePoint    ");
    strcpy(TblLibEv[34], "Chan Prefix ");
    strcpy(TblLibEv[35], "Port Prefix ");
    strcpy(TblLibEv[36], "End Track   ");
    strcpy(TblLibEv[37], "Tempo       ");
    strcpy(TblLibEv[38], "SMPTE Offset");
    strcpy(TblLibEv[39], "Time Sign.  ");
    strcpy(TblLibEv[40], "Key Sign.   ");
    strcpy(TblLibEv[41], "Specific    ");
    strcpy(TblLibEv[42], "Reserved    ");
    strcpy(TblLibEv[43], "Dead        ");
    return(0);
}

/* ------------------------------------------------------------------------- */
int InitMidi()
{
    InitTblLibEv();

    int result = midi_backend_init();
    if(result < 0)
    {
        sprintf(string_Last_Order,"MIDI init failed - no device found");
    }
    else
    {
        sprintf(string_Last_Order,"MIDI OK - %d device(s) found",
                midi_backend_get_device_count_in());
    }
    return(0);
}

/* ------------------------------------------------------------------------- */
int QuitMidi()
{
    midi_backend_close();
    return(0);
}