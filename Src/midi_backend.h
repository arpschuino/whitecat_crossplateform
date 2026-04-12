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
* \brief MIDI abstraction layer - replaces MidiShare with RtMidi
* \author Christoph Guillermet
* \version {0.9.0}
* \date {2026}
*
* White Cat - MIDI Backend
*
* Couche d'abstraction MIDI permanente.
* Cache l'implementation RtMidi au reste du code WhiteCat.
* Remplace MidiShare (abandonne) par RtMidi (crossplatform, maintenu).
*
* MIDI abstraction layer.
* Hides RtMidi implementation from the rest of WhiteCat code.
* Replaces MidiShare (abandoned) with RtMidi (crossplatform, maintained).
*
**/

#ifndef MIDI_BACKEND_H
#define MIDI_BACKEND_H

#include "RtMidi.h"
#include <vector>
#include <queue>
#include <mutex>
#include <string>

// ============================================================
// Structure pour stocker un message MIDI recu
// Structure to store a received MIDI message
// ============================================================
struct MidiMessage {
    int type;    // indices MidiShare : 1=Key On, 2=Key Off, 4=Ctrl Change, 7=Pitch Wheel
    int channel;
    int pitch;
    int velocity;
};

// ============================================================
// Variables globales du backend
// Global backend variables
// ============================================================
static const int RTMIDI_MAX_PORTS_IN = 32;
static RtMidiIn*  rtmidi_in_ports[RTMIDI_MAX_PORTS_IN]; // une instance par port IN ouvert
static bool       rtmidi_port_in_open[RTMIDI_MAX_PORTS_IN]; // true si ce port est actif
static bool       rtmidi_port_in_wanted[RTMIDI_MAX_PORTS_IN]; // true si l'utilisateur veut ce port (hotplug)
static RtMidiIn*  rtmidi_query_in = NULL; // instance de requête seule (getPortCount/Name)
static RtMidiOut* rtmidi_out = NULL;
static std::queue<MidiMessage> midi_queue;
static std::mutex midi_mutex;
static bool midi_backend_initialized = false;
static int rtmidi_open_port_out = -1; // port OUT actuellement ouvert (-1 = aucun)

// ============================================================
// Callback RtMidi - appele dans un thread dedie
// RtMidi callback - called in a dedicated thread
// ============================================================
static void rtmidi_callback(double deltatime,
                            std::vector<unsigned char>* message,
                            void* userData)
{
    if (!message || message->size() < 2) return;

    MidiMessage msg;
    unsigned char status  = (*message)[0];
    unsigned char data1   = message->size() > 1 ? (*message)[1] : 0;
    unsigned char data2   = message->size() > 2 ? (*message)[2] : 0;

    int raw_type    = status >> 4;
    msg.channel     = status & 0x0F;
    msg.pitch       = data1;
    msg.velocity    = data2;

    // Conversion vers les types MidiShare (indices de TblLibEv)
    // 0=Note, 1=Key On, 2=Key Off, 3=Key Press, 4=Ctrl Change,
    // 5=Prog Change, 6=Chan Press, 7=Pitch Wheel
    switch(raw_type) {
        case 0x9: // Note On
            msg.type = (data2 == 0) ? 2 : 1; // vel 0 = Key Off
            break;
        case 0x8: // Note Off
            msg.type = 2;
            break;
        case 0xB: // Control Change
            msg.type = 4;
            break;
        case 0xE: // Pitch Wheel
            msg.type = 7;
            msg.velocity = data2;
            break;
        default:
            return; // ignorer les autres types
    }

    // Ajout thread-safe dans la file
    std::lock_guard<std::mutex> lock(midi_mutex);
    midi_queue.push(msg);
    wc_notify_midi_activity(); // maintient le mode 60fps pendant l'activité MIDI
}

// ============================================================
// Initialisation du backend MIDI
// MIDI backend initialization
// ============================================================
static int midi_backend_init()
{
    // Initialiser les tableaux
    for (int i = 0; i < RTMIDI_MAX_PORTS_IN; i++) {
        rtmidi_in_ports[i] = NULL;
        rtmidi_port_in_open[i] = false;
    }
    try {
        rtmidi_query_in = new RtMidiIn(); // instance de requête uniquement
        rtmidi_out      = new RtMidiOut();

        if (rtmidi_query_in->getPortCount() == 0) {
            return -1; // aucun peripherique MIDI
        }

        // Pas d'auto-ouverture : l'utilisateur sélectionne explicitement via l'UI

        if (rtmidi_out->getPortCount() > 0) {
            rtmidi_out->openPort(0);
            rtmidi_open_port_out = 0;
        }

        midi_backend_initialized = true;
        return 0;
    }
    catch (RtMidiError& error) {
        return -1;
    }
}

// ============================================================
// Fermeture du backend MIDI
// MIDI backend shutdown
// ============================================================
static int midi_backend_close()
{
    for (int i = 0; i < RTMIDI_MAX_PORTS_IN; i++) {
        if (rtmidi_in_ports[i]) {
            if (rtmidi_in_ports[i]->isPortOpen()) {
                rtmidi_in_ports[i]->cancelCallback();
                Sleep(100);
                rtmidi_in_ports[i]->closePort();
            }
            delete rtmidi_in_ports[i];
            rtmidi_in_ports[i] = NULL;
            rtmidi_port_in_open[i] = false;
        }
    }
    if (rtmidi_query_in) {
        delete rtmidi_query_in;
        rtmidi_query_in = NULL;
    }
    if (rtmidi_out) {
        rtmidi_out->closePort();
        delete rtmidi_out;
        rtmidi_out = NULL;
    }
    rtmidi_open_port_out = -1;
    midi_backend_initialized = false;
    return 0;
}

// Ferme un port IN spécifique
static int midi_backend_close_port_in(int index)
{
    if (index < 0 || index >= RTMIDI_MAX_PORTS_IN) return -1;
    if (!rtmidi_in_ports[index]) return -1;
    if (rtmidi_in_ports[index]->isPortOpen()) {
        rtmidi_in_ports[index]->cancelCallback();
        Sleep(100); // laisse le thread callback se terminer (Windows MM)
        rtmidi_in_ports[index]->closePort();
        Sleep(100); // laisse Windows MM libérer le device avant toute réouverture
    }
    delete rtmidi_in_ports[index];
    rtmidi_in_ports[index] = NULL;
    rtmidi_port_in_open[index] = false;
    return 0;
}

// Ferme uniquement le port OUT
static int midi_backend_close_out()
{
    if (!rtmidi_out) return -1;
    rtmidi_out->closePort();
    rtmidi_open_port_out = -1;
    return 0;
}

// Retourne true si le port IN donné est ouvert
static bool midi_backend_is_port_in_open(int index)
{
    if (index < 0 || index >= RTMIDI_MAX_PORTS_IN) return false;
    return rtmidi_port_in_open[index];
}

static int midi_backend_get_open_port_out() { return rtmidi_open_port_out; }

// ============================================================
// Lecture de la file de messages MIDI
// Returns true si un message est disponible
// Read MIDI message queue
// Returns true if a message is available
// ============================================================
static bool midi_backend_poll(MidiMessage& msg)
{
    std::lock_guard<std::mutex> lock(midi_mutex);
    if (midi_queue.empty()) return false;
    msg = midi_queue.front();
    midi_queue.pop();
    return true;
}

// ============================================================
// Envoi d'un message MIDI
// Send a MIDI message
// ============================================================
static int midi_backend_send(int type, int channel, int pitch, int velocity)
{
    if (!rtmidi_out || !rtmidi_out->isPortOpen()) return -1;

    std::vector<unsigned char> message(3);
    switch(type) {
        case 1: // Note On
            message[0] = 0x90 | (channel & 0x0F);
            message[1] = pitch;
            message[2] = velocity;
            break;
        case 2: // Note Off
            message[0] = 0x80 | (channel & 0x0F);
            message[1] = pitch;
            message[2] = velocity;
            break;
        case 3: // Control Change
            message[0] = 0xB0 | (channel & 0x0F);
            message[1] = pitch;
            message[2] = velocity;
            break;
        default:
            return -1;
    }

    try {
        rtmidi_out->sendMessage(&message);
        return 0;
    }
    catch (RtMidiError& error) {
        return -1;
    }
}

// ============================================================
// Liste des peripheriques MIDI disponibles
// List available MIDI devices
// ============================================================
static int midi_backend_get_device_count_in()
{
    if (!rtmidi_query_in) return 0;
    return (int)rtmidi_query_in->getPortCount();
}

static int midi_backend_get_device_count_out()
{
    if (!rtmidi_out) return 0;
    return (int)rtmidi_out->getPortCount();
}

static std::string midi_backend_get_device_name_in(int index)
{
    if (!rtmidi_query_in) return "";
    try {
        return rtmidi_query_in->getPortName(index);
    }
    catch (RtMidiError& error) {
        return "";
    }
}

static std::string midi_backend_get_device_name_out(int index)
{
    if (!rtmidi_out) return "";
    try {
        return rtmidi_out->getPortName(index);
    }
    catch (RtMidiError& error) {
        return "";
    }
}

// ============================================================
// Changement de peripherique MIDI
// Change MIDI device
// ============================================================
static int midi_backend_open_device_in(int index)
{
    if (index < 0 || index >= RTMIDI_MAX_PORTS_IN) return -1;
    if (rtmidi_port_in_open[index]) return 0; // déjà ouvert
    try {
        rtmidi_in_ports[index] = new RtMidiIn();
        rtmidi_in_ports[index]->openPort(index);
        rtmidi_in_ports[index]->setCallback(&rtmidi_callback);
        rtmidi_in_ports[index]->ignoreTypes(false, false, false);
        rtmidi_port_in_open[index] = true;
        return 0;
    }
    catch (RtMidiError& error) {
        if (rtmidi_in_ports[index]) {
            delete rtmidi_in_ports[index];
            rtmidi_in_ports[index] = NULL;
        }
        rtmidi_port_in_open[index] = false;
        return -1;
    }
}

static int midi_backend_open_device_out(int index)
{
    if (!rtmidi_out) return -1;
    try {
        if (rtmidi_out->isPortOpen()) rtmidi_out->closePort();
        rtmidi_out->openPort(index);
        rtmidi_open_port_out = index;
        return 0;
    }
    catch (RtMidiError& error) {
        rtmidi_open_port_out = -1;
        return -1;
    }
}

// ============================================================
// Envoi messages systeme MIDI (Start, Stop, Continue)
// Send MIDI system messages
// ============================================================
static int midi_backend_send_system(unsigned char status)
{
    if (!rtmidi_out || !rtmidi_out->isPortOpen()) return -1;
    std::vector<unsigned char> message(1);
    message[0] = status;
    try {
        rtmidi_out->sendMessage(&message);
        return 0;
    }
    catch (RtMidiError& error) {
        return -1;
    }
}

#define MIDI_START    0xFA
#define MIDI_STOP     0xFC
#define MIDI_CONTINUE 0xFB

#endif // MIDI_BACKEND_H