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
* \file audio_backend.h
* \brief Couche d'abstraction audio - remplace Audiere par SDL2_mixer
* \author Jacques Bouault - arpschuino.fr
* \version 0.9.0
* \date 2026
*
* Reproduit l'API Audiere (AudioDevicePtr, OutputStreamPtr, OpenDevice, OpenSound,
* player->play/stop/setVolume/setPosition/getLength/setRepeat/isPlaying/getPan...)
* sur SDL2_mixer + 4 canaux dedies aux players WhiteCat.
*
* Reproduces the Audiere API on SDL2_mixer + 4 dedicated channels for WhiteCat players.
*
* Limitations / Limitations :
*  - getPosition() : estime via SDL_GetTicks (pas d'acces PCM exact)
*  - setPosition() : restart depuis 0 si pos=0, approximatif sinon
*  - getPitchShift() : toujours 1.0 (non supporte par SDL2_mixer)
*  - Formats supportes : WAV, OGG, MP3, FLAC (selon build SDL2_mixer)
**/

#ifndef AUDIO_BACKEND_H
#define AUDIO_BACKEND_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>
#include <cstdio>
#include <cstring>
#include <string>
#include <cmath>

// ============================================================
// Constantes SDL2_mixer
// ============================================================
#define WC_AUDIO_FREQUENCY  44100
#define WC_AUDIO_CHANNELS   2       // stereo
#define WC_AUDIO_CHUNKSIZE  2048
#define WC_AUDIO_FORMAT     MIX_DEFAULT_FORMAT

// 4 canaux dedies aux players 1-4 / 4 dedicated channels for players 1-4
#define WC_PLAYER_CHANNELS  4

namespace audiere {

// ============================================================
// WCAudioPlayer — equivalent OutputStreamPtr
// Wraps un Mix_Chunk + un canal SDL2_mixer
// Wraps a Mix_Chunk + an SDL2_mixer channel
// ============================================================
class WCAudioPlayer {
    Mix_Chunk* chunk;
    int        channel;       // canal SDL2_mixer assigne / assigned SDL2_mixer channel
    float      vol;           // 0.0 - 1.0
    float      pan_val;       // -1.0 (gauche) a +1.0 (droite)
    bool       looping;
    Uint32     play_start_ms; // SDL_GetTicks() au debut de la lecture
    Uint32     pause_pos_ms;  // position sauvegardee a la pause
    bool       paused;
    std::string filepath;

    // Calcul duree totale depuis les bytes du chunk
    // Total duration from chunk byte length
    int calcLengthMs() const {
        if (!chunk) return 0;
        // Format : 44100 Hz, 16-bit stereo = 4 bytes/sample
        int bytes_per_sample = 4;
        return (int)((Uint32)chunk->alen * 1000 / (WC_AUDIO_FREQUENCY * bytes_per_sample));
    }

public:
    WCAudioPlayer(int ch) : chunk(nullptr), channel(ch), vol(1.0f), pan_val(0.0f),
        looping(false), play_start_ms(0), pause_pos_ms(0), paused(false) {}

    ~WCAudioPlayer() {
        if (chunk) {
            Mix_HaltChannel(channel);
            Mix_FreeChunk(chunk);
        }
    }

    bool getRepeat() const { return looping; }

    bool loadFile(const char* filename) {
        if (chunk) {
            Mix_HaltChannel(channel);
            Mix_FreeChunk(chunk);
            chunk = nullptr;
        }
        chunk = Mix_LoadWAV(filename);
        if (!chunk) return false;
        filepath = filename;
        pause_pos_ms = 0;
        paused       = false;
        return true;
    }

    // ---- API Audiere ----

    void play() {
        if (!chunk) return;
        int loops = looping ? -1 : 0;
        Mix_Volume(channel, (int)(vol * MIX_MAX_VOLUME));
        Mix_PlayChannel(channel, chunk, loops);
        play_start_ms = SDL_GetTicks();
        paused = false;
    }

    void stop() {
        Mix_HaltChannel(channel);
        pause_pos_ms = 0;
        paused = false;
    }

    bool isPlaying() const {
        return Mix_Playing(channel) != 0 && !Mix_Paused(channel);
    }

    // Volume 0.0 - 1.0
    void setVolume(float v) {
        vol = (v < 0.0f) ? 0.0f : (v > 1.0f) ? 1.0f : v;
        Mix_Volume(channel, (int)(vol * MIX_MAX_VOLUME));
    }

    float getVolume() const { return vol; }

    void setRepeat(bool repeat) {
        looping = repeat;
    }

    // setPosition en ms depuis le debut / setPosition in ms from start
    // SDL2_mixer ne supporte pas le seek sur chunks, on restart
    void setPosition(float pos_ms) {
        if (!chunk) return;
        if (pos_ms <= 0.0f) {
            // Restart depuis 0
            bool was_playing = isPlaying();
            Mix_HaltChannel(channel);
            pause_pos_ms  = 0;
            play_start_ms = SDL_GetTicks();
            if (was_playing) play();
        }
        // Pour pos > 0 : non supporte sans acces PCM direct
        // For pos > 0: not supported without direct PCM access
    }

    float getPosition() const {
        if (!chunk) return 0.0f;
        if (!isPlaying()) return (float)pause_pos_ms;
        Uint32 now = SDL_GetTicks();
        return (float)(now - play_start_ms + pause_pos_ms);
    }

    // Duree totale en ms / Total duration in ms
    float getLength() const {
        return (float)calcLengthMs();
    }

    // Pan : -1.0 (left) .. 0.0 (center) .. +1.0 (right)
    void setPan(float p) {
        pan_val = (p < -1.0f) ? -1.0f : (p > 1.0f) ? 1.0f : p;
        Uint8 left  = (Uint8)(255.0f * (1.0f - (pan_val > 0 ? pan_val : 0.0f)));
        Uint8 right = (Uint8)(255.0f * (1.0f + (pan_val < 0 ? pan_val : 0.0f)));
        Mix_SetPanning(channel, left, right);
    }

    float getPan() const { return pan_val; }

    // PitchShift : non supporte / not supported
    float getPitchShift() const { return 1.0f; }
    void  setPitchShift(float)  {}
};

// ============================================================
// WCAudioDevice — equivalent AudioDevicePtr
// ============================================================
class WCAudioDevice {
    bool   initialized;
    char   name_buf[128];
    WCAudioPlayer* players[WC_PLAYER_CHANNELS];

public:
    WCAudioDevice() : initialized(false) {
        strncpy(name_buf, "SDL2_mixer", sizeof(name_buf)-1);
        for (int i = 0; i < WC_PLAYER_CHANNELS; i++) {
            players[i] = new WCAudioPlayer(i);
        }
    }

    ~WCAudioDevice() {
        for (int i = 0; i < WC_PLAYER_CHANNELS; i++) delete players[i];
        if (initialized) Mix_CloseAudio();
    }

    bool init() {
        if (Mix_OpenAudio(WC_AUDIO_FREQUENCY, WC_AUDIO_FORMAT,
                          WC_AUDIO_CHANNELS, WC_AUDIO_CHUNKSIZE) < 0) {
            return false;
        }
        Mix_AllocateChannels(WC_PLAYER_CHANNELS);
        initialized = true;
        return true;
    }

    const char* getName() const { return name_buf; }
    bool isOk()          const { return initialized; }

    WCAudioPlayer* getPlayer(int idx) {
        if (idx < 0 || idx >= WC_PLAYER_CHANNELS) return nullptr;
        return players[idx];
    }
};

// ============================================================
// Pointeurs smart (compatibilite Audiere)
// Smart pointer wrappers (Audiere compatibility)
// ============================================================
typedef WCAudioDevice*  AudioDevicePtr;
typedef WCAudioPlayer*  OutputStreamPtr;

// ============================================================
// AudioDeviceDesc (Audiere device descriptor)
// ============================================================
struct AudioDeviceDesc {
    std::string name;
    std::string description;
};

// ============================================================
// Fonctions Audiere globales / Global Audiere functions
// ============================================================

// OpenDevice(0) — ouvre le device audio par defaut
inline AudioDevicePtr OpenDevice(int /*index*/ = 0) {
    WCAudioDevice* dev = new WCAudioDevice();
    if (!dev->init()) {
        // Echec : device null (le code existant verifie if(!device))
        delete dev;
        return nullptr;
    }
    return dev;
}

// Compteur global pour assigner un canal par player
// Global counter to assign one channel per player
static int wc_next_player_channel = 0;

// OpenSound(device, filename, preloaded) — charge un fichier audio
// Retourne un OutputStreamPtr (WCAudioPlayer*)
inline OutputStreamPtr OpenSound(AudioDevicePtr device, const char* filename, bool /*preloaded*/ = false) {
    if (!device || !filename) return nullptr;
    int ch = wc_next_player_channel;
    if (ch >= WC_PLAYER_CHANNELS) ch = WC_PLAYER_CHANNELS - 1;
    wc_next_player_channel++;
    WCAudioPlayer* player = device->getPlayer(ch);
    if (!player) return nullptr;
    if (!player->loadFile(filename)) return nullptr;
    return player;
}

// GetSupportedAudioDevices (Audiere API)
inline void GetSupportedAudioDevices(std::vector<AudioDeviceDesc>& devices) {
    int n = SDL_GetNumAudioDevices(0);
    devices.clear();
    for (int i = 0; i < n; i++) {
        AudioDeviceDesc d;
        const char* name = SDL_GetAudioDeviceName(i, 0);
        d.name = name ? name : "unknown";
        d.description = d.name;
        devices.push_back(d);
    }
}

// ShowSupportedAudioDevices est definie dans audio_core5.cpp — pas de doublon ici

} // namespace audiere

#endif // AUDIO_BACKEND_H
