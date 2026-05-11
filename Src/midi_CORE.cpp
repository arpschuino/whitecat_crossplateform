/*-------------------------------------------------------------------------------------------------------------
                                 |
          CWWWWWWWW              | Copyright (C) 2009-2016  Christoph Guillermet
       WWWWWWWWWWWWWWW           | Copyright (C) 2026       Jacques Bouault - arpschuino.fr
     WWWWWWWWWWWWWWWWWWW         |
    WWWWWWWWWWWWWWWWWCWWWW       |
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
* \brief MIDI Core - implementation backend RtMidi + fonctions init/quit
* \author Christoph Guillermet
* \author Jacques Bouault - arpschuino.fr
* \version {0.9.1}
* \date {2026}
**/

#include "wc_tus.h"
#include "RtMidi.h"
#include "midi_backend.h"
#include <vector>
#include <queue>
#include <mutex>

// ============================================================
// Etat interne du backend (prive a cette TU)
// ============================================================
static RtMidiIn*  rtmidi_in_ports[RTMIDI_MAX_PORTS_IN];
static bool       rtmidi_port_in_open[RTMIDI_MAX_PORTS_IN];
static RtMidiIn*  rtmidi_query_in      = NULL;
static RtMidiOut* rtmidi_out           = NULL;
static std::queue<MidiMessage> midi_queue;
static std::mutex midi_mutex;
static bool midi_backend_initialized   = false;
static int  rtmidi_open_port_out       = -1;

// ============================================================
// Callback RtMidi - appele dans un thread dedie
// ============================================================
static void rtmidi_callback(double /*deltatime*/,
                            std::vector<unsigned char>* message,
                            void* /*userData*/)
{
    if (!message || message->size() < 2) return;

    MidiMessage msg;
    unsigned char status = (*message)[0];
    unsigned char data1  = message->size() > 1 ? (*message)[1] : 0;
    unsigned char data2  = message->size() > 2 ? (*message)[2] : 0;

    int raw_type = status >> 4;
    msg.channel  = status & 0x0F;
    msg.pitch    = data1;
    msg.velocity = data2;

    switch(raw_type) {
        case 0x9: msg.type = (data2 == 0) ? 2 : 1; break;
        case 0x8: msg.type = 2; break;
        case 0xB: msg.type = 4; break;
        case 0xE: msg.type = 7; msg.velocity = data2; break;
        default: return;
    }

    std::lock_guard<std::mutex> lock(midi_mutex);
    midi_queue.push(msg);
    wc_notify_midi_activity();
}

// ============================================================
// Initialisation du backend MIDI
// ============================================================
int midi_backend_init()
{
    for (int i = 0; i < RTMIDI_MAX_PORTS_IN; i++) {
        rtmidi_in_ports[i]     = NULL;
        rtmidi_port_in_open[i] = false;
    }
    try {
        rtmidi_query_in = new RtMidiIn();
        rtmidi_out      = new RtMidiOut();

        if (rtmidi_query_in->getPortCount() == 0) {
            return -1;
        }

        if (rtmidi_out->getPortCount() > 0) {
            rtmidi_out->openPort(0);
            rtmidi_open_port_out = 0;
        }

        midi_backend_initialized = true;
        return 0;
    }
    catch (RtMidiError&) {
        return -1;
    }
}

// ============================================================
// Fermeture du backend MIDI
// ============================================================
int midi_backend_close()
{
    for (int i = 0; i < RTMIDI_MAX_PORTS_IN; i++) {
        if (rtmidi_in_ports[i]) {
            if (rtmidi_in_ports[i]->isPortOpen()) {
                rtmidi_in_ports[i]->cancelCallback();
                Sleep(100);
                rtmidi_in_ports[i]->closePort();
            }
            delete rtmidi_in_ports[i];
            rtmidi_in_ports[i]     = NULL;
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
    rtmidi_open_port_out     = -1;
    midi_backend_initialized = false;
    return 0;
}

int midi_backend_close_port_in(int index)
{
    if (index < 0 || index >= RTMIDI_MAX_PORTS_IN) return -1;
    if (!rtmidi_in_ports[index]) return -1;
    if (rtmidi_in_ports[index]->isPortOpen()) {
        rtmidi_in_ports[index]->cancelCallback();
        Sleep(100);
        rtmidi_in_ports[index]->closePort();
        Sleep(100);
    }
    delete rtmidi_in_ports[index];
    rtmidi_in_ports[index]     = NULL;
    rtmidi_port_in_open[index] = false;
    return 0;
}

int midi_backend_close_out()
{
    if (!rtmidi_out) return -1;
    rtmidi_out->closePort();
    rtmidi_open_port_out = -1;
    return 0;
}

bool midi_backend_is_port_in_open(int index)
{
    if (index < 0 || index >= RTMIDI_MAX_PORTS_IN) return false;
    return rtmidi_port_in_open[index];
}

int midi_backend_get_open_port_out() { return rtmidi_open_port_out; }

// ============================================================
// Lecture de la file de messages MIDI
// ============================================================
bool midi_backend_poll(MidiMessage& msg)
{
    std::lock_guard<std::mutex> lock(midi_mutex);
    if (midi_queue.empty()) return false;
    msg = midi_queue.front();
    midi_queue.pop();
    return true;
}

// ============================================================
// Envoi d'un message MIDI
// ============================================================
int midi_backend_send(int type, int channel, int pitch, int velocity)
{
    if (!rtmidi_out || !rtmidi_out->isPortOpen()) return -1;

    std::vector<unsigned char> message(3);
    switch(type) {
        case 1:
            message[0] = 0x90 | (channel & 0x0F);
            message[1] = pitch;
            message[2] = velocity;
            break;
        case 2:
            message[0] = 0x80 | (channel & 0x0F);
            message[1] = pitch;
            message[2] = velocity;
            break;
        case 3:
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
    catch (RtMidiError&) {
        return -1;
    }
}

// ============================================================
// Liste des peripheriques MIDI
// ============================================================
int midi_backend_get_device_count_in()
{
    if (!rtmidi_query_in) return 0;
    return (int)rtmidi_query_in->getPortCount();
}

int midi_backend_get_device_count_out()
{
    if (!rtmidi_out) return 0;
    return (int)rtmidi_out->getPortCount();
}

std::string midi_backend_get_device_name_in(int index)
{
    if (!rtmidi_query_in) return "";
    try { return rtmidi_query_in->getPortName(index); }
    catch (RtMidiError&) { return ""; }
}

std::string midi_backend_get_device_name_out(int index)
{
    if (!rtmidi_out) return "";
    try { return rtmidi_out->getPortName(index); }
    catch (RtMidiError&) { return ""; }
}

// ============================================================
// Ouverture de peripheriques
// ============================================================
int midi_backend_open_device_in(int index)
{
    if (index < 0 || index >= RTMIDI_MAX_PORTS_IN) return -1;
    if (rtmidi_port_in_open[index]) return 0;
    try {
        rtmidi_in_ports[index] = new RtMidiIn();
        rtmidi_in_ports[index]->openPort(index);
        rtmidi_in_ports[index]->setCallback(&rtmidi_callback);
        rtmidi_in_ports[index]->ignoreTypes(false, false, false);
        rtmidi_port_in_open[index] = true;
        return 0;
    }
    catch (RtMidiError&) {
        if (rtmidi_in_ports[index]) {
            delete rtmidi_in_ports[index];
            rtmidi_in_ports[index] = NULL;
        }
        rtmidi_port_in_open[index] = false;
        return -1;
    }
}

int midi_backend_open_device_out(int index)
{
    if (!rtmidi_out) return -1;
    try {
        if (rtmidi_out->isPortOpen()) rtmidi_out->closePort();
        rtmidi_out->openPort(index);
        rtmidi_open_port_out = index;
        return 0;
    }
    catch (RtMidiError&) {
        rtmidi_open_port_out = -1;
        return -1;
    }
}

// ============================================================
// Messages systeme MIDI (Start, Stop, Continue, Clock)
// ============================================================
int midi_backend_send_system(unsigned char status)
{
    if (!rtmidi_out || !rtmidi_out->isPortOpen()) return -1;
    std::vector<unsigned char> message(1);
    message[0] = status;
    try {
        rtmidi_out->sendMessage(&message);
        return 0;
    }
    catch (RtMidiError&) {
        return -1;
    }
}

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
