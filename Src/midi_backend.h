/*-------------------------------------------------------------------------------------------------------------
                                 |
          CWWWWWWWW              | Copyright (C) 2009-2016  Christoph Guillermet
       WWWWWWWWWWWWWWW           | Copyright (C) 2026       Jacques Bouault - arpschuino.fr
     WWWWWWWWWWWWWWWWWWW         |
    WWWWWWWWWWWWWWWWWCWWWW       |
   WWWWWWWWWWWWWWWWW tWWWWW      | This file is part of White Cat.
  WWWW   WWWWWWWWWW  tWWWWWW     |
 WWWWWt              tWWWWWWa    | White Cat is free software: you can redistribute it and/or modify
 WWWWWW               WWWWWWW    | it under the terms of the GNU General Public License as published by
WWWWWWWW              WWWWWWW    | the Free Software Foundation, either version 2 of the License, or
WWWWWWWW               WWWWWWW   | (at your option) any later version.
WWWWWWW               WWWWWWWW   |
WWWWWWW      CWWW    W WWWWWWW   | White Cat is distributed in the hope that it will be useful,
WWWWWWW            aW  WWWWWWW   | but WITHOUT ANY WARRANTY; without even the implied warranty of
WWWWWWWW           C  WWWWWWWW   | MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 WWWWWWWW            CWWWWWWW    | GNU General Public License for more details.
 WWWWWWWWW          WWWWWWWWW    |
  WWWWWWWWWWC    CWWWWWWWWWW     | You should have received a copy of the GNU General Public License
   WWWWWWWWWWWWWWWWWWWWWWWW      | along with White Cat.  If not, see <http://www.gnu.org/licenses/>.
    WWWWWWWWWWWWWWWWWWWWWW       |
      WWWWWWWWWWWWWWWWWWa        |
        WWWWWWWWWWWWWWW          |
           WWWWWWWWt             |
                                 |
---------------------------------------------------------------------------------------------------------------*/
/**
* \file midi_backend.h
* \brief MIDI abstraction layer - interface publique (prototypes uniquement)
* \author Christoph Guillermet
* \author Jacques Bouault - arpschuino.fr
* \version {0.9.1}
* \date {2026}
*
* Couche d'abstraction MIDI.
* Ce header expose uniquement les prototypes et types publics.
* L'implementation et l'etat interne sont dans midi_CORE.cpp.
**/

#ifndef MIDI_BACKEND_H
#define MIDI_BACKEND_H

#include <string>

// ============================================================
// Message MIDI recu (types indices MidiShare pour compatibilite)
// ============================================================
struct MidiMessage {
    int type;     // 1=Key On, 2=Key Off, 4=Ctrl Change, 7=Pitch Wheel
    int channel;
    int pitch;
    int velocity;
};

static const int RTMIDI_MAX_PORTS_IN = 32;

#define MIDI_START    0xFA
#define MIDI_STOP     0xFC
#define MIDI_CONTINUE 0xFB

// ============================================================
// Interface publique du backend MIDI
// Implementee dans midi_CORE.cpp
// ============================================================
int  midi_backend_init();
int  midi_backend_close();
int  midi_backend_close_port_in(int index);
int  midi_backend_close_out();
bool midi_backend_is_port_in_open(int index);
int  midi_backend_get_open_port_out();
bool midi_backend_poll(MidiMessage& msg);
int  midi_backend_send(int type, int channel, int pitch, int velocity);
int  midi_backend_get_device_count_in();
int  midi_backend_get_device_count_out();
std::string midi_backend_get_device_name_in(int index);
std::string midi_backend_get_device_name_out(int index);
int  midi_backend_open_device_in(int index);
int  midi_backend_open_device_out(int index);
int  midi_backend_send_system(unsigned char status);

#endif // MIDI_BACKEND_H
