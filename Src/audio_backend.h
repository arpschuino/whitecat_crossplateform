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
WWWWWWWWW           C  WWWWWWWW  | GNU General Public License for more details.
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
* \brief Couche d'abstraction audio streaming — WAV (raw PCM), MP3 (minimp3), OGG/FLAC (SDL_mixer)
* \author Jacques Bouault - arpschuino.fr
* \version 0.9.2
* \date 2026
*
* - WAV  : header parsé, PCM streamé depuis le disque (taille illimitée)
* - MP3  : décodé frame-par-frame par minimp3 (taille illimitée, pas de RAM peak)
* - OGG/FLAC : décodés par Mix_LoadWAV (taille raisonnable après compression)
* - Tous les players mixés via Mix_HookMusic (thread audio SDL)
* - Thread-safe : SDL_LockAudio/SDL_UnlockAudio sur toute modification d'état
**/

#ifndef AUDIO_BACKEND_H
#define AUDIO_BACKEND_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>
#include <cstdio>
#include <cstdarg>
#include <cstring>
#include <string>
#include <cmath>
#include <vector>

// minimp3 — décodeur MP3 léger, MIT/CC0, cross-platform
#define MINIMP3_IMPLEMENTATION
#include "minimp3.h"

// stb_vorbis — décodeur OGG Vorbis (Sean Barrett, public domain)
// Utilisé pour bypasser Mix_LoadWAV qui tient SDL_LockAudio pendant SDL_ConvertAudio
#define STB_VORBIS_NO_PUSHDATA_API
#include "stb_vorbis.c"
#undef L
#undef C
#undef R

// dr_flac — décodeur FLAC (David Reid, public domain / MIT-0)
#define DR_FLAC_IMPLEMENTATION
#define DR_FLAC_NO_STDIO   // on lit le fichier via SDL_RWFromFile (UTF-8 safe)
#include "dr_flac.h"

// ============================================================
// Format de sortie — doit correspondre aux paramètres de Mix_OpenAudio()
// ============================================================
#define WC_AUDIO_FREQUENCY   44100
#define WC_AUDIO_CHANNELS    2
#define WC_AUDIO_CHUNKSIZE   4096
#define WC_AUDIO_FORMAT      AUDIO_S16LSB   // S16 little-endian stéréo

#define WC_PLAYER_CHANNELS   4
#define WC_BYTES_PER_FRAME   4  // sizeof(Sint16) * 2 canaux

extern int audio_ram_limit_mb;  // défini dans whitecat.h, configurable via CFG > core cfg

namespace audiere {

// ============================================================
// Utilitaires : détection par extension
// ============================================================
static bool wc_is_mp3_file(const char* fn) {
    const char* dot = strrchr(fn, '.');
    if (!dot) return false;
    const char* e = dot + 1;
    return (e[0]=='m'||e[0]=='M') && (e[1]=='p'||e[1]=='P') &&
           (e[2]=='3') && e[3]=='\0';
}
static bool wc_is_ogg_file(const char* fn) {
    const char* dot = strrchr(fn, '.');
    if (!dot) return false;
    const char* e = dot + 1;
    return (e[0]=='o'||e[0]=='O') && (e[1]=='g'||e[1]=='G') &&
           (e[2]=='g'||e[2]=='G') && e[3]=='\0';
}
static bool wc_is_flac_file(const char* fn) {
    const char* dot = strrchr(fn, '.');
    if (!dot) return false;
    const char* e = dot + 1;
    return (e[0]=='f'||e[0]=='F') && (e[1]=='l'||e[1]=='L') &&
           (e[2]=='a'||e[2]=='A') && (e[3]=='c'||e[3]=='C') && e[4]=='\0';
}

// ============================================================
// Parseur d'en-tête WAV — lit RIFF/WAVE, retourne spec PCM
// + offset/taille du chunk "data". Ne charge rien en RAM.
// Supporte PCM 8/16/32 bits et float 32 bits.
// Retourne false pour WAV 24 bits, compressé, ou fichier non-WAV.
// ============================================================
struct WCWavInfo {
    SDL_AudioSpec spec;
    Sint64        data_offset;
    Sint64        data_size;
};

static bool wc_parse_wav_header(SDL_RWops* rw, WCWavInfo* out) {
    char   tag[4];
    Uint32 u32;

    if (SDL_RWread(rw, tag, 1, 4) != 4 || memcmp(tag, "RIFF", 4) != 0) return false;
    SDL_RWread(rw, &u32, 4, 1);
    if (SDL_RWread(rw, tag, 1, 4) != 4 || memcmp(tag, "WAVE", 4) != 0) return false;

    SDL_zero(*out);
    bool got_fmt = false;

    for (int guard = 0; guard < 64; guard++) {
        char   cid[4];
        Uint32 csize;
        if (SDL_RWread(rw, cid,    1, 4) != 4) break;
        if (SDL_RWread(rw, &csize, 4, 1) != 1) break;
        csize = SDL_SwapLE32(csize);
        Sint64 chunk_end = SDL_RWtell(rw) + csize + (csize & 1);

        if (memcmp(cid, "fmt ", 4) == 0 && csize >= 16) {
            Uint16 audio_fmt, channels, bits;
            Uint32 sample_rate;
            Uint16 d16; Uint32 d32;
            SDL_RWread(rw, &audio_fmt,   2, 1); audio_fmt   = SDL_SwapLE16(audio_fmt);
            SDL_RWread(rw, &channels,    2, 1); channels    = SDL_SwapLE16(channels);
            SDL_RWread(rw, &sample_rate, 4, 1); sample_rate = SDL_SwapLE32(sample_rate);
            SDL_RWread(rw, &d32, 4, 1);
            SDL_RWread(rw, &d16, 2, 1);
            SDL_RWread(rw, &bits, 2, 1); bits = SDL_SwapLE16(bits);

            if (audio_fmt != 1 && audio_fmt != 3) {
                SDL_RWseek(rw, chunk_end, RW_SEEK_SET); continue;
            }
            if (audio_fmt == 1 && bits == 24) {   // 24 bits non supporté par SDL_AudioStream
                SDL_RWseek(rw, chunk_end, RW_SEEK_SET); continue;
            }
            out->spec.freq     = (int)sample_rate;
            out->spec.channels = (Uint8)channels;
            if (audio_fmt == 3) {
                out->spec.format = AUDIO_F32LSB;
            } else {
                switch (bits) {
                    case  8: out->spec.format = AUDIO_U8;     break;
                    case 16: out->spec.format = AUDIO_S16LSB; break;
                    case 32: out->spec.format = AUDIO_S32LSB; break;
                    default: SDL_RWseek(rw, chunk_end, RW_SEEK_SET); continue;
                }
            }
            got_fmt = true;

        } else if (memcmp(cid, "data", 4) == 0 && got_fmt) {
            out->data_offset = SDL_RWtell(rw);
            out->data_size   = csize;
            return true;
        }
        SDL_RWseek(rw, chunk_end, RW_SEEK_SET);
    }
    return false;
}

// ============================================================
// WCStreamState — état d'un player, accédé depuis le thread audio
// ET depuis le thread principal/timer.
// Toute écriture doit être protégée par SDL_LockAudio.
// ============================================================
#define WC_MP3_BUF_SIZE  65536   // read-ahead MP3 (bytes compressés)

struct WCStreamState {
    // ---- Mode WAV streaming (PCM brut depuis fichier) ----
    SDL_RWops*    rw;
    Sint64        data_start, data_size, cur_byte;
    SDL_AudioSpec src_spec;

    // ---- Mode WAV en RAM (pré-chargé — élimine les I/O dans le callback) ----
    Uint8*        wav_ram;       // données PCM brut chargées en mémoire
    Uint32        wav_ram_size;  // octets valides
    Uint32        wav_ram_pos;   // position de lecture courante

    // ---- Mode MP3 streaming (minimp3) ----
    SDL_RWops*    mp3_rw;
    mp3dec_t      mp3dec;
    Uint8         mp3_buf[WC_MP3_BUF_SIZE];
    int           mp3_buf_fill;
    bool          mp3_eof;
    Sint64        mp3_file_size;
    int           mp3_channels, mp3_hz;

    // ---- Mode OGG streaming (stb_vorbis depuis RAM) ----
    stb_vorbis*   ogg;
    Uint8*        ogg_buf;       // fichier entier en RAM (requis par stb_vorbis)
    int           ogg_channels, ogg_hz;

    // ---- Mode FLAC streaming (dr_flac depuis RAM) ----
    drflac*       flac_dec;
    Uint8*        flac_buf;      // fichier entier en RAM (requis par dr_flac)
    int           flac_channels, flac_hz;

    // ---- Mode Chunk (formats rares décodés via Mix_LoadWAV) ----
    Mix_Chunk*    chunk;
    Uint32        chunk_pos;

    // ---- Convertisseur de format (source → WC_AUDIO_FORMAT/CHANNELS/FREQUENCY) ----
    SDL_AudioStream* conv;

    // ---- État de lecture ----
    bool    playing, looping;
    float   vol, pan, pitch;
    Uint32  length_ms;
    Uint32  start_tick;
    Uint32  paused_position_ms; // position dans le fichier en ms (vitesse naturelle)
    int     sample_hz;  // sample rate du fichier chargé (pour conversion ms↔samples)
    int     fade_in_frames;    // fondu d'entrée anti-clic (décompte en frames output)
    int     fade_out_frames;   // fondu de sortie avant seek différé (0 = aucun)
    float   pending_seek_ms;   // cible du seek différé après fade-out (-1 = aucun)

    WCStreamState() { SDL_zero(*this); pitch = 1.0f; pending_seek_ms = -1.0f; }

    // Appelé dans le lock (destructeur) : libère tout immédiatement
    void cleanup_locked() {
        if (conv)     { SDL_FreeAudioStream(conv);  conv     = nullptr; }
        if (rw)       { SDL_RWclose(rw);            rw       = nullptr; }
        if (mp3_rw)   { SDL_RWclose(mp3_rw);        mp3_rw   = nullptr; }
        if (chunk)    { Mix_FreeChunk(chunk);        chunk    = nullptr; }
        if (wav_ram)  { SDL_free(wav_ram);           wav_ram  = nullptr; wav_ram_size = 0; wav_ram_pos = 0; }
        if (ogg)      { stb_vorbis_close(ogg);       ogg      = nullptr; }
        if (ogg_buf)  { SDL_free(ogg_buf);           ogg_buf  = nullptr; }
        if (flac_dec) { drflac_close(flac_dec);      flac_dec = nullptr; }
        if (flac_buf) { SDL_free(flac_buf);          flac_buf = nullptr; }
        playing = false;
        cur_byte = 0; chunk_pos = 0; data_start = 0; data_size = 0;
        mp3_buf_fill = 0; mp3_eof = false;
    }
    // Appelé dans le lock (chargement) : vole les vieux handles sans les libérer
    // → libération hors lock pour ne pas bloquer le callback audio.
    // Exception : ogg/flac fermés directement (rapide, sans I/O).
    void steal_locked(SDL_RWops*& o_rw, SDL_RWops*& o_mp3_rw,
                      SDL_AudioStream*& o_conv, Mix_Chunk*& o_chunk,
                      Uint8*& o_wav_ram) {
        o_rw     = rw;      rw      = nullptr;
        o_mp3_rw = mp3_rw;  mp3_rw  = nullptr;
        o_conv   = conv;    conv    = nullptr;
        o_chunk  = chunk;   chunk   = nullptr;
        o_wav_ram= wav_ram; wav_ram = nullptr; wav_ram_size = 0; wav_ram_pos = 0;
        if (ogg)      { stb_vorbis_close(ogg);  ogg      = nullptr; }
        if (ogg_buf)  { SDL_free(ogg_buf);       ogg_buf  = nullptr; }
        if (flac_dec) { drflac_close(flac_dec);  flac_dec = nullptr; }
        if (flac_buf) { SDL_free(flac_buf);      flac_buf = nullptr; }
        playing = false;
        cur_byte = 0; chunk_pos = 0; data_start = 0; data_size = 0;
        mp3_buf_fill = 0; mp3_eof = false;
        paused_position_ms = 0;
    }
};

static void wc_free_old_handles(SDL_RWops* o_rw, SDL_RWops* o_mp3_rw,
                                 SDL_AudioStream* o_conv, Mix_Chunk* o_chunk,
                                 Uint8* o_wav_ram) {
    if (o_conv)    SDL_FreeAudioStream(o_conv);
    if (o_rw)      SDL_RWclose(o_rw);
    if (o_mp3_rw)  SDL_RWclose(o_mp3_rw);
    if (o_chunk)   Mix_FreeChunk(o_chunk);
    if (o_wav_ram) SDL_free(o_wav_ram);
};

static WCStreamState wc_streams[WC_PLAYER_CHANNELS];

// ============================================================
// Music hook — thread audio SDL, ~43 fois/seconde.
// Mélange les 4 players dans le buffer de sortie.
// Format sortie : WC_AUDIO_FORMAT (S16 LE), stéréo, 44100 Hz.
// ============================================================
static inline Sint16 wc_clamp16(Sint32 v) {
    return (v < -32768) ? (Sint16)-32768 : (v > 32767) ? (Sint16)32767 : (Sint16)v;
}

// Remplit le convertisseur 'conv' depuis le WAV (RAM ou disque)
static void wc_fill_wav(WCStreamState& s, int need) {
    static Uint8 buf[8192];
    while (SDL_AudioStreamAvailable(s.conv) < need) {
        if (s.wav_ram) {
            // Chemin RAM — aucune I/O disque dans le callback
            Sint64 rem = (Sint64)s.wav_ram_size - s.wav_ram_pos;
            if (rem <= 0) {
                if (s.looping) { s.wav_ram_pos = 0; SDL_AudioStreamClear(s.conv); break; }
                else           { s.paused_position_ms = s.length_ms; s.playing = false; break; }
            }
            int to_put = (int)(rem < 8192 ? rem : 8192);
            SDL_AudioStreamPut(s.conv, s.wav_ram + s.wav_ram_pos, to_put);
            s.wav_ram_pos += to_put;
        } else {
            // Chemin disque (fallback : fichier trop grand ou malloc échoué)
            Sint64 rem = s.data_size - s.cur_byte;
            if (rem <= 0) {
                if (s.looping) {
                    SDL_RWseek(s.rw, s.data_start, RW_SEEK_SET);
                    s.cur_byte = 0;
                    SDL_AudioStreamClear(s.conv);
                    break;
                } else { s.paused_position_ms = s.length_ms; s.playing = false; break; }
            }
            int to_read = (int)(rem < 8192 ? rem : 8192);
            int got = (int)SDL_RWread(s.rw, buf, 1, to_read);
            if (got <= 0) { s.paused_position_ms = s.length_ms; s.playing = false; break; }
            s.cur_byte += got;
            SDL_AudioStreamPut(s.conv, buf, got);
        }
    }
}

#ifdef _WIN32
// Log de diagnostic audio — Win32 WriteFile (thread-safe, sans MSVCRT)
static void wc_audio_log(const char* msg) {
    HANDLE h = CreateFileA("wcat_audio.log", GENERIC_WRITE,
                           FILE_SHARE_READ | FILE_SHARE_WRITE,
                           NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    if (h == INVALID_HANDLE_VALUE) return;
    SetFilePointer(h, 0, NULL, FILE_END);
    DWORD written;
    WriteFile(h, msg, (DWORD)lstrlenA(msg), &written, NULL);
    CloseHandle(h);
}
static void wc_audio_logf(const char* fmt, ...) {
    char buf[512];
    va_list ap; va_start(ap, fmt);
    wvsprintfA(buf, fmt, ap);
    va_end(ap);
    wc_audio_log(buf);
}
#else
// Log de diagnostic audio — POSIX stderr
static void wc_audio_log(const char* msg) { fprintf(stderr, "%s", msg); }
static void wc_audio_logf(const char* fmt, ...) {
    va_list ap; va_start(ap, fmt);
    vfprintf(stderr, fmt, ap);
    va_end(ap);
}
#endif

// Remplit le convertisseur 'conv' depuis le stream MP3 (minimp3)
static void wc_fill_mp3(WCStreamState& s, int need) {
    static bool s_fill_entry_logged = false;
    if (!s_fill_entry_logged) {
        wc_audio_logf("[fill_mp3 entry] need=%d buf_fill=%d eof=%d\r\n",
                      need, s.mp3_buf_fill, (int)s.mp3_eof);
        s_fill_entry_logged = true;
    }
    // Nombre max de frames décodées par callback (~12 × 26ms = 312ms)
    // Évite de drainer tout le fichier si SDL_AudioStreamAvailable est défaillant.
    // Le buffer compressé est pré-chargé depuis play() (thread principal).
    int max_frames = 16;

    // Recharge depuis le fichier si buffer compressé bas (lecture fichier dans ce thread)
    if (s.mp3_buf_fill < 16384 && !s.mp3_eof) {
        int space = WC_MP3_BUF_SIZE - s.mp3_buf_fill;
        int got = (int)SDL_RWread(s.mp3_rw, s.mp3_buf + s.mp3_buf_fill, 1, space);
        if (got <= 0) s.mp3_eof = true;
        else          s.mp3_buf_fill += got;
    }

    while (max_frames-- > 0) {
        // Sortie anticipée : assez de données PCM dans le convertisseur
        if (SDL_AudioStreamAvailable(s.conv) >= need) break;
        // ---- Fin de buffer ----
        if (s.mp3_buf_fill < 4) {
            if (!s.mp3_eof) {
                int space = WC_MP3_BUF_SIZE - s.mp3_buf_fill;
                int got = (int)SDL_RWread(s.mp3_rw, s.mp3_buf + s.mp3_buf_fill, 1, space);
                if (got <= 0) s.mp3_eof = true;
                else          s.mp3_buf_fill += got;
            }
            if (s.mp3_buf_fill < 4) {
                if (s.looping) {
                    SDL_RWseek(s.mp3_rw, 0, RW_SEEK_SET);
                    s.mp3_buf_fill = 0; s.mp3_eof = false;
                    mp3dec_init(&s.mp3dec);
                    SDL_AudioStreamClear(s.conv);
                    int got = (int)SDL_RWread(s.mp3_rw, s.mp3_buf, 1, WC_MP3_BUF_SIZE);
                    if (got > 0) s.mp3_buf_fill = got;
                    } else {
                    s.paused_position_ms = s.length_ms;
                    s.playing = false;
                    wc_audio_log("[fill_mp3] EOF buf=0 eof=1\n");
                    break;
                }
            }
            if (!s.playing) break;
        }

        mp3dec_frame_info_t info;
        mp3d_sample_t       pcm[MINIMP3_MAX_SAMPLES_PER_FRAME];
        int samples = mp3dec_decode_frame(&s.mp3dec, s.mp3_buf, s.mp3_buf_fill, pcm, &info);

        if (info.frame_bytes > 0) {
            if (info.frame_bytes <= s.mp3_buf_fill) {
                // Cas normal : consomme les octets décodés/sautés
                s.mp3_buf_fill -= info.frame_bytes;
                if (s.mp3_buf_fill > 0)
                    memmove(s.mp3_buf, s.mp3_buf + info.frame_bytes, s.mp3_buf_fill);
            } else {
                // frame_bytes > mp3_buf_fill : tag ID3 plus grand que le buffer
                // (ex: pochette d'album > 64 KB). Saute dans le fichier.
                char logbuf[128];
                snprintf(logbuf, sizeof(logbuf),
                         "[fill_mp3] Grand tag: fb=%d buf=%d\n",
                         info.frame_bytes, s.mp3_buf_fill);
                wc_audio_log(logbuf);
                Sint64 extra = (Sint64)info.frame_bytes - s.mp3_buf_fill;
                SDL_RWseek(s.mp3_rw, extra, RW_SEEK_CUR);
                s.mp3_buf_fill = 0;
                s.mp3_eof = false;
                int got = (int)SDL_RWread(s.mp3_rw, s.mp3_buf, 1, WC_MP3_BUF_SIZE);
                if (got > 0) s.mp3_buf_fill = got;
                else         s.mp3_eof = true;
                continue;
            }
        } else {
            // frame_bytes == 0 : aucune frame trouvée
            if (s.mp3_eof) {
                s.paused_position_ms = s.length_ms;
                s.playing = false;
                wc_audio_log("[fill_mp3] Stop eof+frame=0\n");
                break;
            }
            int space = WC_MP3_BUF_SIZE - s.mp3_buf_fill;
            if (space <= 0) {
                // Buffer plein sans frame valide : avance d'un octet pour resynchroniser
                wc_audio_log("[fill_mp3] Resync buf plein\n");
                s.mp3_buf_fill--;
                if (s.mp3_buf_fill > 0)
                    memmove(s.mp3_buf, s.mp3_buf + 1, s.mp3_buf_fill);
            } else {
                int got = (int)SDL_RWread(s.mp3_rw, s.mp3_buf + s.mp3_buf_fill, 1, space);
                if (got <= 0) s.mp3_eof = true;
                else          s.mp3_buf_fill += got;
            }
            continue;
        }

        if (samples > 0) {
            int put_bytes = samples * s.mp3_channels * (int)sizeof(mp3d_sample_t);
            int put_ret   = SDL_AudioStreamPut(s.conv, pcm, put_bytes);
            int avail_now = SDL_AudioStreamAvailable(s.conv);
            static bool s_logged_put = false;
            if (!s_logged_put) {
                wc_audio_logf("[fill_mp3 put] ret=%d bytes=%d avail=%d ch=%d hz=%d\r\n",
                              put_ret, put_bytes, avail_now,
                              s.mp3_channels, s.mp3_hz);
                s_logged_put = true;
            }
        }

        // Recharge si le buffer descend sous le seuil
        if (s.mp3_buf_fill < 16384 && !s.mp3_eof) {
            int space = WC_MP3_BUF_SIZE - s.mp3_buf_fill;
            int got = (int)SDL_RWread(s.mp3_rw, s.mp3_buf + s.mp3_buf_fill, 1, space);
            if (got <= 0) s.mp3_eof = true;
            else          s.mp3_buf_fill += got;
        }
    }

}

// Remplit le convertisseur 'conv' depuis un chunk SDL_mixer (OGG/FLAC)
static void wc_fill_chunk(WCStreamState& s, int need) {
    static Uint8 buf[8192];
    while (SDL_AudioStreamAvailable(s.conv) < need) {
        Sint64 rem = (Sint64)s.chunk->alen - s.chunk_pos;
        if (rem <= 0) {
            if (s.looping) {
                s.chunk_pos = 0;
                SDL_AudioStreamClear(s.conv);
                break; // ne pas remplir depuis pos 0 dans ce callback
            } else { s.paused_position_ms = s.length_ms; s.playing = false; break; }
        }
        int to_read = (int)(rem < 8192 ? rem : 8192);
        SDL_memcpy(buf, s.chunk->abuf + s.chunk_pos, to_read);
        s.chunk_pos += to_read;
        SDL_AudioStreamPut(s.conv, buf, to_read);
    }
}

// Convertit du PCM S16 interleaved (ch canaux, sample_rate Hz) vers WC_AUDIO_FORMAT
// et encapsule dans un Mix_Chunk (allocated=1). Retourne nullptr si échec.
// Permet de bypasser Mix_LoadWAV et son SDL_LockAudio interne pour OGG/FLAC.
// Le PCM source (pcm) est géré par l'appelant ; le Mix_Chunk retourné est libéré par Mix_FreeChunk.
static Mix_Chunk* wc_s16_to_chunk(const short* pcm, int frames, int channels, int sample_rate) {
    if (!pcm || frames <= 0 || channels <= 0 || sample_rate <= 0) return nullptr;
    SDL_AudioStream* conv = SDL_NewAudioStream(AUDIO_S16LSB, (Uint8)channels, sample_rate,
                                               WC_AUDIO_FORMAT, WC_AUDIO_CHANNELS, WC_AUDIO_FREQUENCY);
    if (!conv) return nullptr;
    SDL_AudioStreamPut(conv, pcm, frames * channels * (int)sizeof(short));
    SDL_AudioStreamFlush(conv);
    int avail = SDL_AudioStreamAvailable(conv);
    if (avail <= 0) { SDL_FreeAudioStream(conv); return nullptr; }
    Uint8* buf = (Uint8*)SDL_malloc((size_t)avail);
    if (!buf) { SDL_FreeAudioStream(conv); return nullptr; }
    int got = SDL_AudioStreamGet(conv, buf, avail);
    SDL_FreeAudioStream(conv);
    if (got <= 0) { SDL_free(buf); return nullptr; }
    Mix_Chunk* chunk = (Mix_Chunk*)SDL_malloc(sizeof(Mix_Chunk));
    if (!chunk) { SDL_free(buf); return nullptr; }
    chunk->allocated = 1;
    chunk->abuf      = buf;
    chunk->alen      = (Uint32)got;
    chunk->volume    = MIX_MAX_VOLUME;
    return chunk;
}

// Seek depuis le thread audio (appelé dans le callback — pas de lock requis)
static void wc_do_seek(WCStreamState& s, float target_ms) {
    if (s.chunk) {
        Uint32 bp = (target_ms <= 0.0f) ? 0u :
            (Uint32)((double)target_ms / 1000.0 * WC_AUDIO_FREQUENCY * WC_BYTES_PER_FRAME);
        bp = (bp / WC_BYTES_PER_FRAME) * WC_BYTES_PER_FRAME;
        if (bp >= s.chunk->alen) bp = 0;
        s.chunk_pos = bp;
        if (s.conv) SDL_AudioStreamClear(s.conv);
    } else if (s.wav_ram && s.src_spec.freq && s.src_spec.channels) {
        int bpf = (SDL_AUDIO_BITSIZE(s.src_spec.format) / 8) * s.src_spec.channels;
        if (bpf > 0) {
            Sint64 bp = (target_ms <= 0.0f) ? 0 :
                (Sint64)((double)target_ms / 1000.0 * s.src_spec.freq * bpf);
            bp = (bp / bpf) * bpf;
            if (bp < 0) bp = 0;
            if (bp >= (Sint64)s.wav_ram_size) bp = 0;
            s.wav_ram_pos = (Uint32)bp;
            if (s.conv) SDL_AudioStreamClear(s.conv);
        }
    } else if (s.rw && s.src_spec.freq && s.src_spec.channels) {
        int bpf = (SDL_AUDIO_BITSIZE(s.src_spec.format) / 8) * s.src_spec.channels;
        if (bpf > 0) {
            Sint64 bp = (target_ms <= 0.0f) ? 0 :
                (Sint64)((double)target_ms / 1000.0 * s.src_spec.freq * bpf);
            bp = (bp / bpf) * bpf;
            if (bp < 0) bp = 0;
            if (bp >= s.data_size) bp = 0;
            SDL_RWseek(s.rw, s.data_start + bp, RW_SEEK_SET);
            s.cur_byte = bp;
            if (s.conv) SDL_AudioStreamClear(s.conv);
        }
    } else if (s.mp3_rw && s.length_ms > 0 && s.mp3_file_size > 0) {
        Sint64 bp = (target_ms <= 0.0f) ? 0 :
            (Sint64)((double)target_ms / s.length_ms * s.mp3_file_size);
        if (bp < 0) bp = 0;
        if (bp >= s.mp3_file_size) bp = 0;
        SDL_RWseek(s.mp3_rw, bp, RW_SEEK_SET);
        s.mp3_buf_fill = 0; s.mp3_eof = false;
        mp3dec_init(&s.mp3dec);
        if (s.conv) SDL_AudioStreamClear(s.conv);
    } else if (s.ogg && s.ogg_hz > 0) {
        unsigned int samp = (target_ms <= 0.0f) ? 0 :
            (unsigned int)((double)target_ms / 1000.0 * s.ogg_hz);
        stb_vorbis_seek(s.ogg, samp);
        if (s.conv) SDL_AudioStreamClear(s.conv);
    } else if (s.flac_dec && s.flac_hz > 0) {
        drflac_uint64 fr = (target_ms <= 0.0f) ? 0 :
            (drflac_uint64)((double)target_ms / 1000.0 * s.flac_hz);
        drflac_seek_to_pcm_frame(s.flac_dec, fr);
        if (s.conv) SDL_AudioStreamClear(s.conv);
    }
    float p = (s.pitch > 0.0f) ? s.pitch : 1.0f;
    float ms = (target_ms > 0.0f) ? target_ms : 0.0f;
    s.start_tick = SDL_GetTicks() - (Uint32)(ms / p);
    s.paused_position_ms = (Uint32)ms;
}

// Remplit le convertisseur depuis un stream OGG (stb_vorbis, fichier en RAM)
static void wc_fill_ogg(WCStreamState& s, int need) {
    static short pcm_buf[4096]; // 4096 samples interleaved (2048 frames stéréo)
    while (SDL_AudioStreamAvailable(s.conv) < need) {
        int n = stb_vorbis_get_samples_short_interleaved(
            s.ogg, s.ogg_channels, pcm_buf, 4096);
        if (n <= 0) {
            if (s.looping) { stb_vorbis_seek_start(s.ogg); SDL_AudioStreamClear(s.conv); break; }
            else           { s.paused_position_ms = s.length_ms; s.playing = false; break; }
        }
        SDL_AudioStreamPut(s.conv, pcm_buf, n * s.ogg_channels * (int)sizeof(short));
    }
}

// Remplit le convertisseur depuis un stream FLAC (dr_flac, fichier en RAM)
static void wc_fill_flac(WCStreamState& s, int need) {
    static short pcm_buf[4096]; // 4096 samples interleaved
    while (SDL_AudioStreamAvailable(s.conv) < need) {
        int frames_req = 4096 / (s.flac_channels > 0 ? s.flac_channels : 1);
        drflac_uint64 n = drflac_read_pcm_frames_s16(s.flac_dec, (drflac_uint64)frames_req, pcm_buf);
        if (n <= 0) {
            if (s.looping) { drflac_seek_to_pcm_frame(s.flac_dec, 0); SDL_AudioStreamClear(s.conv); break; }
            else           { s.paused_position_ms = s.length_ms; s.playing = false; break; }
        }
        SDL_AudioStreamPut(s.conv, pcm_buf, (int)n * s.flac_channels * (int)sizeof(short));
    }
}

static void wc_hook_music(void* /*udata*/, Uint8* stream, int len) {
    static bool s_hook_logged = false;
    if (!s_hook_logged) {
        wc_audio_logf("[wc_hook] len=%d\r\n", len);
        s_hook_logged = true;
    }
    SDL_memset(stream, 0, len);

    static Uint8 conv_buf[65536];

    for (int i = 0; i < WC_PLAYER_CHANNELS; i++) {
        WCStreamState& s = wc_streams[i];
        if (!s.playing || !s.conv) continue;

        // Remplit le convertisseur depuis la source
        if      (s.rw || s.wav_ram) wc_fill_wav  (s, len);
        else if (s.mp3_rw)          wc_fill_mp3  (s, len);
        else if (s.chunk)           wc_fill_chunk(s, len);
        else if (s.ogg)             wc_fill_ogg  (s, len);
        else if (s.flac_dec)        wc_fill_flac (s, len);

        if (!s.playing) continue;

        int avail  = SDL_AudioStreamAvailable(s.conv);
        if (avail <= 0) continue;
        int to_get = (avail < len) ? avail : len;
        if (to_get > (int)sizeof(conv_buf)) to_get = (int)sizeof(conv_buf);

        int got = SDL_AudioStreamGet(s.conv, conv_buf, to_get);
        if (got <= 0) continue;

        // Mélange dans stream avec volume + pan (S16 stéréo)
        // Courbe logarithmique (loi carrée) : perception humaine du volume
        float vol_log = s.vol * s.vol * s.vol;
        float vL = vol_log * (s.pan <= 0.0f ? 1.0f : 1.0f - s.pan);
        float vR = vol_log * (s.pan >= 0.0f ? 1.0f : 1.0f + s.pan);

        Sint16*       dst    = (Sint16*)stream;
        const Sint16* src    = (const Sint16*)conv_buf;
        int frames  = got / (int)sizeof(Sint16) / 2;
        int max_f   = len / (int)sizeof(Sint16) / 2;
        if (frames > max_f) frames = max_f;

        for (int f = 0; f < frames; f++) {
            // Seek différé : dès que le fade-out atteint zéro, seek + break immédiat
            // (évite de mélanger les frames post-fade depuis l'ancienne position)
            if (s.fade_out_frames == 0 && s.pending_seek_ms >= 0.0f) {
                wc_do_seek(s, s.pending_seek_ms);
                s.pending_seek_ms = -1.0f;
                s.fade_in_frames  = 88;
                break;
            }
            float fade = 1.0f;
            if (s.fade_in_frames > 0) {
                fade = 1.0f - (float)s.fade_in_frames / 88.0f;
                s.fade_in_frames--;
            }
            if (s.fade_out_frames > 0) {
                fade *= (float)s.fade_out_frames / 88.0f;
                s.fade_out_frames--;
            }
            dst[f*2]   = wc_clamp16(dst[f*2]   + (Sint32)(src[f*2]   * vL * fade));
            dst[f*2+1] = wc_clamp16(dst[f*2+1] + (Sint32)(src[f*2+1] * vR * fade));
        }
    }
}

// ============================================================
// WCAudioPlayer — wraps un WCStreamState
// ============================================================
class WCAudioPlayer {
    int idx;

    WCStreamState&       S()       { return wc_streams[idx]; }
    const WCStreamState& S() const { return wc_streams[idx]; }

    SDL_AudioStream* make_conv(SDL_AudioFormat fmt, int ch, int hz) {
        return SDL_NewAudioStream(fmt, (Uint8)ch, hz,
                                  WC_AUDIO_FORMAT, WC_AUDIO_CHANNELS, WC_AUDIO_FREQUENCY);
    }

    Uint32 length_from_wav_stream() const {
        const WCStreamState& s = S();
        if (!s.src_spec.freq || !s.src_spec.channels) return 0;
        int bpf = (SDL_AUDIO_BITSIZE(s.src_spec.format) / 8) * s.src_spec.channels;
        if (!bpf) return 0;
        return (Uint32)((Uint64)s.data_size * 1000ULL / ((Uint64)s.src_spec.freq * bpf));
    }

    Uint32 length_from_mp3_file() const {
        const WCStreamState& s = S();
        // Estimation CBR : taille_fichier * 8 / bitrate_kbps
        // (assez précis pour CBR, approximatif pour VBR)
        // Non calculable ici sans décoder tout le fichier.
        // Sera estimé à la première frame dans loadFile().
        return s.length_ms;
    }

    Uint32 length_from_chunk() const {
        const WCStreamState& s = S();
        if (!s.chunk) return 0;
        return (Uint32)((Uint64)s.chunk->alen * 1000ULL / (WC_AUDIO_FREQUENCY * WC_BYTES_PER_FRAME));
    }

    void save_prefs(float& v, float& p, bool& l) const {
        const WCStreamState& s = S();
        v = (s.vol > 0.0f) ? s.vol : 1.0f;
        p = s.pan;
        l = s.looping;
    }

    void restore_prefs(float v, float p, bool l) {
        WCStreamState& s = S();
        s.vol = v; s.pan = p; s.looping = l;
    }

public:
    explicit WCAudioPlayer(int i) : idx(i) {}

    ~WCAudioPlayer() {
        SDL_LockAudio();
        S().cleanup_locked();
        SDL_UnlockAudio();
    }

    bool loadFile(const char* filename) {
        float sv; float sp; bool sl;
        save_prefs(sv, sp, sl);

        // ---- 1. WAV : pré-chargement en RAM (élimine les I/O dans le callback) ----
        // Fallback disque si le fichier dépasse 200 MB ou si malloc échoue.
        static const Sint64 WC_WAV_RAM_MAX = 200LL * 1024 * 1024;
        SDL_RWops* rw = SDL_RWFromFile(filename, "rb");
        if (rw) {
            WCWavInfo info;
            if (wc_parse_wav_header(rw, &info)) {
                SDL_AudioStream* conv = make_conv(info.spec.format,
                                                  info.spec.channels, info.spec.freq);
                if (conv) {
                    // Tente la lecture en RAM avant d'acquérir le lock
                    Uint8* wav_ram = nullptr;
                    Uint32 wav_ram_size = 0;
                    if (info.data_size > 0 && info.data_size <= WC_WAV_RAM_MAX) {
                        wav_ram = (Uint8*)SDL_malloc((size_t)info.data_size);
                        if (wav_ram) {
                            SDL_RWseek(rw, info.data_offset, RW_SEEK_SET);
                            int got = (int)SDL_RWread(rw, wav_ram, 1, (size_t)info.data_size);
                            if (got > 0) {
                                wav_ram_size = (Uint32)got;
                                SDL_RWclose(rw); rw = nullptr; // handle fermé, plus besoin
                            } else {
                                SDL_free(wav_ram); wav_ram = nullptr;
                            }
                        }
                    }
                    // Installe le nouveau state sous lock (pointeur-swap minimal)
                    SDL_RWops* o_rw; SDL_RWops* o_mp3_rw; SDL_AudioStream* o_conv;
                    Mix_Chunk* o_chunk; Uint8* o_wav_ram;
                    SDL_LockAudio();
                    S().steal_locked(o_rw, o_mp3_rw, o_conv, o_chunk, o_wav_ram);
                    S().data_size  = info.data_size; // pour length_from_wav_stream()
                    S().src_spec   = info.spec;
                    S().conv       = conv;
                    if (wav_ram) {
                        S().wav_ram      = wav_ram;
                        S().wav_ram_size = wav_ram_size;
                        S().wav_ram_pos  = 0;
                        // S().rw reste nullptr (handle fermé avant le lock)
                    } else {
                        // Fallback disque (fichier > 200 MB ou malloc échoué)
                        S().rw         = rw;
                        S().data_start = info.data_offset;
                        S().cur_byte   = 0;
                        rw = nullptr; // ownership transféré à S()
                    }
                    S().length_ms  = length_from_wav_stream();
                    S().sample_hz  = info.spec.freq;
                    restore_prefs(sv, sp, sl);
                    SDL_UnlockAudio();
                    wc_free_old_handles(o_rw, o_mp3_rw, o_conv, o_chunk, o_wav_ram);
                    if (rw) SDL_RWclose(rw); // sécurité (ne devrait pas arriver)
                    return true;
                }
                SDL_FreeAudioStream(conv);
            }
            if (rw) SDL_RWclose(rw);
        }

        // ---- 2. Essai MP3 streaming (minimp3) ----
        if (wc_is_mp3_file(filename)) {
            SDL_RWops* mp3_rw = SDL_RWFromFile(filename, "rb");
            if (mp3_rw) {
                // Sonde les premières frames pour obtenir le format
                Uint8 probe[32768];
                int   probe_fill = (int)SDL_RWread(mp3_rw, probe, 1, sizeof(probe));
                mp3dec_t           dec; mp3dec_init(&dec);
                mp3dec_frame_info_t fi; mp3d_sample_t pcm[MINIMP3_MAX_SAMPLES_PER_FRAME];
                int bytes_scanned = 0;
                int channels = 0, hz = 0, bitrate = 0;

                // Scan les frames MP3 :
                // - passe les tags ID3 (fi.frame_bytes > 0, fi.hz == 0)
                // - passe le header Xing/VBR (fi.bitrate_kbps == 0, bitrate libre)
                // - s'arrête sur la première vraie frame audio avec bitrate > 0
                int xing_frame_start = -1;
                int scan_limit = 64;
                while (bytes_scanned < probe_fill && scan_limit-- > 0) {
                    int frame_start = bytes_scanned;
                    int samples = mp3dec_decode_frame(&dec,
                                      probe + bytes_scanned, probe_fill - bytes_scanned,
                                      pcm, &fi);
                    if (fi.frame_bytes > 0) bytes_scanned += fi.frame_bytes;
                    else break;
                    if (fi.hz > 0 && fi.channels > 0) {
                        if (channels == 0) { channels = fi.channels; hz = fi.hz; }
                        if (fi.bitrate_kbps > 0) { bitrate = fi.bitrate_kbps; break; }
                        // bitrate == 0 : frame Xing/free-format, mémorise sa position
                        if (xing_frame_start < 0) xing_frame_start = frame_start;
                    }
                }

                if (hz > 0 && channels > 0) {
                    SDL_AudioStream* conv = make_conv(AUDIO_S16LSB, channels, hz);
                    if (conv) {
                        Sint64 fsize = SDL_RWsize(mp3_rw);
                        Uint32 len_ms = 0;

                        // Durée via bitrate CBR (première vraie frame audio)
                        if (bitrate > 0 && fsize > 0)
                            len_ms = (Uint32)((Uint64)fsize * 8ULL / (Uint32)bitrate);

                        // Durée via header Xing (VBR exact : total_frames × 1152 / hz)
                        if (len_ms == 0 && xing_frame_start >= 0) {
                            int s0 = xing_frame_start + 4; // skip frame header
                            int s1 = xing_frame_start + 40;
                            if (s1 > probe_fill - 4) s1 = probe_fill - 4;
                            for (int si = s0; si < s1; si++) {
                                if ((probe[si]=='X'&&probe[si+1]=='i'&&probe[si+2]=='n'&&probe[si+3]=='g')||
                                    (probe[si]=='I'&&probe[si+1]=='n'&&probe[si+2]=='f'&&probe[si+3]=='o')) {
                                    if (si + 12 <= probe_fill) {
                                        Uint32 xf = ((Uint32)probe[si+4]<<24)|((Uint32)probe[si+5]<<16)|
                                                    ((Uint32)probe[si+6]<<8)| probe[si+7];
                                        if (xf & 1) { // total_frames présent
                                            Uint32 tf = ((Uint32)probe[si+8]<<24)|((Uint32)probe[si+9]<<16)|
                                                        ((Uint32)probe[si+10]<<8)| probe[si+11];
                                            if (tf > 0 && hz > 0)
                                                len_ms = (Uint32)((Uint64)tf * 1152ULL * 1000ULL / (Uint32)hz);
                                        }
                                    }
                                    break;
                                }
                            }
                        }

                        // Fallback : durée inconnue → 24h pour ne pas déclencher
                        // position >= length prématurément. La vraie fin est détectée
                        // par wc_fill_mp3 qui positionne s.playing = false à l'EOF.
                        if (len_ms == 0)
                            len_ms = 86400000;

                        // Log de diagnostic
                        wc_audio_logf("[loadFile MP3] file=%s hz=%d ch=%d bitrate=%d "
                                      "xing=%d len_ms=%u\n",
                                      filename, hz, channels, bitrate,
                                      xing_frame_start, (unsigned)len_ms);

                        SDL_RWseek(mp3_rw, 0, RW_SEEK_SET); // retour au début

                        SDL_RWops* o_rw; SDL_RWops* o_mp3_rw; SDL_AudioStream* o_conv;
                        Mix_Chunk* o_chunk; Uint8* o_wav_ram;
                        SDL_LockAudio();
                        S().steal_locked(o_rw, o_mp3_rw, o_conv, o_chunk, o_wav_ram);
                        S().mp3_rw       = mp3_rw;
                        mp3dec_init(&S().mp3dec);
                        S().mp3_buf_fill = 0;
                        S().mp3_eof      = false;
                        S().mp3_file_size= fsize;
                        S().mp3_channels = channels;
                        S().mp3_hz       = hz;
                        S().conv         = conv;
                        S().length_ms    = len_ms;
                        S().sample_hz    = hz;
                        restore_prefs(sv, sp, sl);
                        SDL_UnlockAudio();
                        wc_free_old_handles(o_rw, o_mp3_rw, o_conv, o_chunk, o_wav_ram);
                        return true;
                    }
                    SDL_FreeAudioStream(conv);
                }
                SDL_RWclose(mp3_rw);
            }
        }

        // Helper : installe un Mix_Chunk sous lock et retourne true.
        auto install_chunk = [&](Mix_Chunk* chunk) -> bool {
            SDL_AudioStream* conv = make_conv(WC_AUDIO_FORMAT, WC_AUDIO_CHANNELS, WC_AUDIO_FREQUENCY);
            if (!conv) { Mix_FreeChunk(chunk); return false; }
            SDL_RWops* o_rw; SDL_RWops* o_mp3_rw; SDL_AudioStream* o_conv;
            Mix_Chunk* o_chunk; Uint8* o_wav_ram;
            SDL_LockAudio();
            S().steal_locked(o_rw, o_mp3_rw, o_conv, o_chunk, o_wav_ram);
            S().chunk = chunk; S().chunk_pos = 0;
            S().conv      = conv;
            S().length_ms = length_from_chunk();
            S().sample_hz = WC_AUDIO_FREQUENCY;
            restore_prefs(sv, sp, sl);
            SDL_UnlockAudio();
            wc_free_old_handles(o_rw, o_mp3_rw, o_conv, o_chunk, o_wav_ram);
            return true;
        };

        // ---- 3. OGG (stb_vorbis streaming depuis RAM) ----
        // Lecture fichier en RAM (~10 ms), ouverture handle (parsing headers seulement,
        // instantané), installation immédiate. Décodage PCM frame par frame dans wc_fill_ogg.
        // Limite configurable (audio_ram_limit_mb, défaut 300 MB) : au-delà fallback Mix_LoadWAV.
        Sint64 WC_OGG_FLAC_RAM_MAX = (Sint64)audio_ram_limit_mb * 1024 * 1024;
        if (wc_is_ogg_file(filename)) {
            SDL_RWops* rw = SDL_RWFromFile(filename, "rb");
            if (rw) {
                Sint64 sz = SDL_RWsize(rw);
                Uint8* fbuf = (sz > 0 && sz <= WC_OGG_FLAC_RAM_MAX)
                              ? (Uint8*)SDL_malloc((size_t)sz) : nullptr;
                int nread = fbuf ? (int)SDL_RWread(rw, fbuf, 1, (size_t)sz) : 0;
                SDL_RWclose(rw);
                if (nread > 0) {
                    int ogg_err = 0;
                    stb_vorbis* vorbis = stb_vorbis_open_memory(fbuf, nread, &ogg_err, nullptr);
                    if (vorbis) {
                        stb_vorbis_info info = stb_vorbis_get_info(vorbis);
                        SDL_AudioStream* conv = make_conv(AUDIO_S16LSB, info.channels, info.sample_rate);
                        if (conv) {
                            unsigned int total_samp = stb_vorbis_stream_length_in_samples(vorbis);
                            Uint32 len_ms = (info.sample_rate > 0) ?
                                (Uint32)((Uint64)total_samp * 1000ULL / info.sample_rate) : 0;
                            SDL_RWops* o_rw; SDL_RWops* o_mp3_rw; SDL_AudioStream* o_conv;
                            Mix_Chunk* o_chunk; Uint8* o_wav_ram;
                            SDL_LockAudio();
                            S().steal_locked(o_rw, o_mp3_rw, o_conv, o_chunk, o_wav_ram);
                            S().ogg          = vorbis;
                            S().ogg_buf      = fbuf;
                            S().ogg_channels = info.channels;
                            S().ogg_hz       = info.sample_rate;
                            S().conv         = conv;
                            S().length_ms    = len_ms;
                            S().sample_hz    = info.sample_rate;
                            restore_prefs(sv, sp, sl);
                            SDL_UnlockAudio();
                            wc_free_old_handles(o_rw, o_mp3_rw, o_conv, o_chunk, o_wav_ram);
                            return true;
                        }
                        stb_vorbis_close(vorbis);
                    }
                }
                SDL_free(fbuf);
            }
        }

        // ---- 4. FLAC (dr_flac streaming depuis RAM) ----
        if (wc_is_flac_file(filename)) {
            SDL_RWops* rw = SDL_RWFromFile(filename, "rb");
            if (rw) {
                Sint64 sz = SDL_RWsize(rw);
                Uint8* fbuf = (sz > 0 && sz <= WC_OGG_FLAC_RAM_MAX)
                              ? (Uint8*)SDL_malloc((size_t)sz) : nullptr;
                int nread = fbuf ? (int)SDL_RWread(rw, fbuf, 1, (size_t)sz) : 0;
                SDL_RWclose(rw);
                if (nread > 0) {
                    drflac* flac = drflac_open_memory(fbuf, (size_t)nread, nullptr);
                    if (flac) {
                        SDL_AudioStream* conv = make_conv(AUDIO_S16LSB,
                            (int)flac->channels, (int)flac->sampleRate);
                        if (conv) {
                            Uint32 len_ms = (flac->sampleRate > 0) ?
                                (Uint32)((Uint64)flac->totalPCMFrameCount * 1000ULL / flac->sampleRate) : 0;
                            SDL_RWops* o_rw; SDL_RWops* o_mp3_rw; SDL_AudioStream* o_conv;
                            Mix_Chunk* o_chunk; Uint8* o_wav_ram;
                            SDL_LockAudio();
                            S().steal_locked(o_rw, o_mp3_rw, o_conv, o_chunk, o_wav_ram);
                            S().flac_dec      = flac;
                            S().flac_buf      = fbuf;
                            S().flac_channels = (int)flac->channels;
                            S().flac_hz       = (int)flac->sampleRate;
                            S().conv          = conv;
                            S().length_ms     = len_ms;
                            S().sample_hz     = (int)flac->sampleRate;
                            restore_prefs(sv, sp, sl);
                            SDL_UnlockAudio();
                            wc_free_old_handles(o_rw, o_mp3_rw, o_conv, o_chunk, o_wav_ram);
                            return true;
                        }
                        drflac_close(flac);
                    }
                }
                SDL_free(fbuf);
            }
        }

        // ---- 5. Fallback : Mix_LoadWAV (formats rares ou inconnus) ----
        // Note : Mix_LoadWAV tient SDL_LockAudio pendant SDL_ConvertAudio.
        // Acceptable ici car OGG/FLAC sont gérés avant.
        Mix_Chunk* chunk = Mix_LoadWAV(filename);
        if (!chunk) return false;
        return install_chunk(chunk);
    }

    // ---- API Audiere ----

    void play() {
        SDL_LockAudio();
        WCStreamState& s = S();
        Uint32 target_ms = s.paused_position_ms;
        if (s.wav_ram) {
            if (target_ms > 0 && s.src_spec.freq && s.src_spec.channels) {
                int bpf = (SDL_AUDIO_BITSIZE(s.src_spec.format) / 8) * s.src_spec.channels;
                if (bpf > 0) {
                    Sint64 bp = (Sint64)((double)target_ms / 1000.0 * s.src_spec.freq * bpf);
                    bp = (bp / bpf) * bpf;
                    if (bp < 0) bp = 0;
                    if (bp >= (Sint64)s.wav_ram_size) bp = 0;
                    s.wav_ram_pos = (Uint32)bp;
                }
            } else {
                s.wav_ram_pos = 0;
            }
        } else if (s.rw) {
            if (target_ms > 0 && s.src_spec.freq && s.src_spec.channels) {
                int bpf = (SDL_AUDIO_BITSIZE(s.src_spec.format) / 8) * s.src_spec.channels;
                if (bpf > 0) {
                    Sint64 bp = (Sint64)((double)target_ms / 1000.0 * s.src_spec.freq * bpf);
                    bp = (bp / bpf) * bpf; // alignement sur frame
                    if (bp < 0) bp = 0;
                    if (bp >= s.data_size) bp = 0;
                    SDL_RWseek(s.rw, s.data_start + bp, RW_SEEK_SET);
                    s.cur_byte = bp;
                }
            } else {
                SDL_RWseek(s.rw, s.data_start, RW_SEEK_SET);
                s.cur_byte = 0;
            }
        } else if (s.mp3_rw) {
            if (target_ms > 0 && s.length_ms > 0 && s.mp3_file_size > 0) {
                Sint64 bp = (Sint64)((double)target_ms / s.length_ms * s.mp3_file_size);
                if (bp < 0) bp = 0;
                if (bp >= s.mp3_file_size) bp = 0;
                SDL_RWseek(s.mp3_rw, bp, RW_SEEK_SET);
            } else {
                SDL_RWseek(s.mp3_rw, 0, RW_SEEK_SET);
            }
            s.mp3_buf_fill = 0;
            s.mp3_eof      = false;
            mp3dec_init(&s.mp3dec);
            // Pré-charge le buffer depuis le thread appelant (principal/timer),
            // car SDL_RWread depuis le thread audio (MSVCRT non initialisé) peut retourner 0.
            int got = (int)SDL_RWread(s.mp3_rw, s.mp3_buf, 1, WC_MP3_BUF_SIZE);
            if (got > 0) s.mp3_buf_fill = got;
            else         s.mp3_eof = true;
        } else if (s.chunk) {
            if (target_ms > 0) {
                Uint32 bp = (Uint32)((double)target_ms / 1000.0 * WC_AUDIO_FREQUENCY * WC_BYTES_PER_FRAME);
                bp = (bp / WC_BYTES_PER_FRAME) * WC_BYTES_PER_FRAME;
                if (bp >= s.chunk->alen) bp = 0;
                s.chunk_pos = bp;
            } else {
                s.chunk_pos = 0;
            }
        } else if (s.ogg && s.ogg_hz > 0) {
            unsigned int samp = (target_ms > 0) ?
                (unsigned int)((double)target_ms / 1000.0 * s.ogg_hz) : 0;
            stb_vorbis_seek(s.ogg, samp);
        } else if (s.flac_dec && s.flac_hz > 0) {
            drflac_uint64 fr = (target_ms > 0) ?
                (drflac_uint64)((double)target_ms / 1000.0 * s.flac_hz) : 0;
            drflac_seek_to_pcm_frame(s.flac_dec, fr);
        }
        if (s.conv) SDL_AudioStreamClear(s.conv);
        float p = (s.pitch > 0.0f) ? s.pitch : 1.0f;
        s.start_tick     = SDL_GetTicks() - (Uint32)(target_ms / p);
        s.fade_in_frames = 88; // 2ms à 44100 Hz
        s.playing        = true;
        SDL_UnlockAudio();
    }

    void stop() {
        SDL_LockAudio();
        WCStreamState& s = S();
        if (s.playing)
            s.paused_position_ms = (Uint32)((double)(SDL_GetTicks() - s.start_tick) * s.pitch);
        s.playing = false;
        SDL_UnlockAudio();
    }

    bool isPlaying() const { return S().playing; }

    void setVolume(float v) {
        v = (v < 0.0f) ? 0.0f : (v > 1.0f) ? 1.0f : v;
        SDL_LockAudio(); S().vol = v; SDL_UnlockAudio();
    }
    float getVolume() const { return S().vol; }

    void setRepeat(bool r) {
        SDL_LockAudio(); S().looping = r; SDL_UnlockAudio();
    }
    bool getRepeat() const { return S().looping; }

    void setPan(float p) {
        p = (p < -1.0f) ? -1.0f : (p > 1.0f) ? 1.0f : p;
        SDL_LockAudio(); S().pan = p; SDL_UnlockAudio();
    }
    float getPan() const { return S().pan; }

    // setPosition en samples (unité native du code original Audiere)
    void setPosition(float pos_samples) {
        SDL_LockAudio();
        WCStreamState& s = S();
        int hz = s.sample_hz > 0 ? s.sample_hz : 44100;
        float pos_ms = pos_samples * 1000.0f / hz;
        if (pos_ms <= 0.0f) {
            // Rembobinage
            if (s.wav_ram) {
                s.wav_ram_pos = 0;
            } else if (s.rw) {
                SDL_RWseek(s.rw, s.data_start, RW_SEEK_SET);
                s.cur_byte = 0;
            } else if (s.mp3_rw) {
                SDL_RWseek(s.mp3_rw, 0, RW_SEEK_SET);
                s.mp3_buf_fill = 0; s.mp3_eof = false;
                mp3dec_init(&s.mp3dec);
            } else if (s.chunk) {
                s.chunk_pos = 0;
            } else if (s.ogg) {
                stb_vorbis_seek_start(s.ogg);
            } else if (s.flac_dec) {
                drflac_seek_to_pcm_frame(s.flac_dec, 0);
            }
            if (s.conv) SDL_AudioStreamClear(s.conv);
            s.start_tick = SDL_GetTicks();
            s.paused_position_ms = 0;
        } else if (s.wav_ram && s.src_spec.freq && s.src_spec.channels) {
            // Seek précis dans WAV RAM
            int bpf = (SDL_AUDIO_BITSIZE(s.src_spec.format) / 8) * s.src_spec.channels;
            if (bpf > 0) {
                Sint64 bp = (Sint64)((double)pos_ms / 1000.0 * s.src_spec.freq * bpf);
                bp = (bp / bpf) * bpf;
                if (bp < 0) bp = 0;
                if (bp >= (Sint64)s.wav_ram_size) bp = 0;
                s.wav_ram_pos = (Uint32)bp;
                if (s.conv) SDL_AudioStreamClear(s.conv);
            }
            float p = (s.pitch > 0.0f) ? s.pitch : 1.0f;
            s.start_tick = SDL_GetTicks() - (Uint32)(pos_ms / p);
            s.paused_position_ms = (Uint32)pos_ms;
        } else if (s.rw && s.src_spec.freq && s.src_spec.channels) {
            // Seek précis dans WAV disque
            int bpf = (SDL_AUDIO_BITSIZE(s.src_spec.format) / 8) * s.src_spec.channels;
            if (bpf > 0) {
                Sint64 bp = (Sint64)((double)pos_ms / 1000.0 * s.src_spec.freq * bpf);
                bp = (bp / bpf) * bpf; // alignement sur frame
                if (bp < 0) bp = 0;
                if (bp >= s.data_size) bp = 0;
                SDL_RWseek(s.rw, s.data_start + bp, RW_SEEK_SET);
                s.cur_byte = bp;
                if (s.conv) SDL_AudioStreamClear(s.conv);
            }
            float p = (s.pitch > 0.0f) ? s.pitch : 1.0f;
            s.start_tick = SDL_GetTicks() - (Uint32)(pos_ms / p);
            s.paused_position_ms = (Uint32)pos_ms;
        } else if (s.mp3_rw && s.length_ms > 0 && s.mp3_file_size > 0) {
            // Seek approximatif dans MP3 (CBR uniquement)
            Sint64 bp = (Sint64)((double)pos_ms / s.length_ms * s.mp3_file_size);
            if (bp < 0) bp = 0;
            if (bp >= s.mp3_file_size) bp = 0;
            SDL_RWseek(s.mp3_rw, bp, RW_SEEK_SET);
            s.mp3_buf_fill = 0; s.mp3_eof = false;
            mp3dec_init(&s.mp3dec); // reset décodeur (perd sync, rattrapé rapidement)
            if (s.conv) SDL_AudioStreamClear(s.conv);
            float p = (s.pitch > 0.0f) ? s.pitch : 1.0f;
            s.start_tick = SDL_GetTicks() - (Uint32)(pos_ms / p);
            s.paused_position_ms = (Uint32)pos_ms;
        } else if (s.chunk) {
            Uint32 bp = (Uint32)((double)pos_ms / 1000.0 * WC_AUDIO_FREQUENCY * WC_BYTES_PER_FRAME);
            bp = (bp / WC_BYTES_PER_FRAME) * WC_BYTES_PER_FRAME;
            if (bp >= s.chunk->alen) bp = 0;
            s.chunk_pos = bp;
            if (s.conv) SDL_AudioStreamClear(s.conv);
            float p = (s.pitch > 0.0f) ? s.pitch : 1.0f;
            s.start_tick = SDL_GetTicks() - (Uint32)(pos_ms / p);
            s.paused_position_ms = (Uint32)pos_ms;
        } else if (s.ogg && s.ogg_hz > 0) {
            unsigned int samp = (unsigned int)((double)pos_ms / 1000.0 * s.ogg_hz);
            stb_vorbis_seek(s.ogg, samp);
            if (s.conv) SDL_AudioStreamClear(s.conv);
            float p = (s.pitch > 0.0f) ? s.pitch : 1.0f;
            s.start_tick = SDL_GetTicks() - (Uint32)(pos_ms / p);
            s.paused_position_ms = (Uint32)pos_ms;
        } else if (s.flac_dec && s.flac_hz > 0) {
            drflac_uint64 fr = (drflac_uint64)((double)pos_ms / 1000.0 * s.flac_hz);
            drflac_seek_to_pcm_frame(s.flac_dec, fr);
            if (s.conv) SDL_AudioStreamClear(s.conv);
            float p = (s.pitch > 0.0f) ? s.pitch : 1.0f;
            s.start_tick = SDL_GetTicks() - (Uint32)(pos_ms / p);
            s.paused_position_ms = (Uint32)pos_ms;
        }
        if (s.playing) s.fade_in_frames = 88;
        SDL_UnlockAudio();
    }

    // Seek différé avec crossfade pour les transitions de boucle
    void loopBackTo(float pos_samples) {
        SDL_LockAudio();
        WCStreamState& s = S();
        if (s.playing && s.pending_seek_ms < 0.0f) {
            float hz = (float)(s.sample_hz > 0 ? s.sample_hz : 44100);
            s.pending_seek_ms = pos_samples * 1000.0f / hz;
            s.fade_out_frames = 88;
        }
        SDL_UnlockAudio();
    }
    bool isPendingSeek() const { return S().pending_seek_ms >= 0.0f; }

    float getPosition() const {
        const WCStreamState& s = S();
        int hz = s.sample_hz > 0 ? s.sample_hz : 44100;
        float p = (s.pitch > 0.0f) ? s.pitch : 1.0f;
        if (!s.playing)
            return (float)((double)s.paused_position_ms * hz / 1000.0);
        return (float)((double)(SDL_GetTicks() - s.start_tick) * p * hz / 1000.0);
    }

    float getLength() const {
        const WCStreamState& s = S();
        int hz = s.sample_hz > 0 ? s.sample_hz : 44100;
        return (float)((Uint64)s.length_ms * hz / 1000);
    }

    int getSampleRate() const {
        int hz = S().sample_hz;
        return hz > 0 ? hz : 44100;
    }

    float getPitchShift() const { return S().pitch; }

    void setPitchShift(float ratio) {
        if (ratio < 0.05f) ratio = 0.05f;
        if (ratio > 4.0f)  ratio = 4.0f;
        SDL_LockAudio();
        WCStreamState& s = S();
        float old_pitch = s.pitch;
        if (old_pitch == ratio) { SDL_UnlockAudio(); return; }
        // Recalcule start_tick pour conserver la position fichier
        if (s.playing && old_pitch > 0.0f) {
            Uint32 wall_ms = SDL_GetTicks() - s.start_tick;
            float  file_ms = wall_ms * old_pitch;
            s.start_tick   = SDL_GetTicks() - (Uint32)(file_ms / ratio);
        }
        s.pitch = ratio;
        // Recrée le conv avec la nouvelle fréquence source (effet bande magnétique)
        SDL_AudioStream* new_conv = nullptr;
        if (s.wav_ram || s.rw) {
            new_conv = SDL_NewAudioStream(s.src_spec.format, s.src_spec.channels,
                                         (int)(s.src_spec.freq * ratio),
                                         WC_AUDIO_FORMAT, WC_AUDIO_CHANNELS, WC_AUDIO_FREQUENCY);
        } else if (s.mp3_rw) {
            new_conv = SDL_NewAudioStream(AUDIO_S16LSB, (Uint8)s.mp3_channels,
                                         (int)(s.mp3_hz * ratio),
                                         WC_AUDIO_FORMAT, WC_AUDIO_CHANNELS, WC_AUDIO_FREQUENCY);
        } else if (s.chunk) {
            new_conv = SDL_NewAudioStream(WC_AUDIO_FORMAT, WC_AUDIO_CHANNELS,
                                         (int)(WC_AUDIO_FREQUENCY * ratio),
                                         WC_AUDIO_FORMAT, WC_AUDIO_CHANNELS, WC_AUDIO_FREQUENCY);
        }
        if (new_conv) {
            if (s.conv) SDL_FreeAudioStream(s.conv);
            s.conv = new_conv;
        }
        SDL_UnlockAudio();
    }
};

// ============================================================
// WCAudioDevice — possède les 4 players, enregistre le hook
// ============================================================
class WCAudioDevice {
    bool           initialized;
    WCAudioPlayer* players[WC_PLAYER_CHANNELS];

public:
    WCAudioDevice() : initialized(false) {
        for (int i = 0; i < WC_PLAYER_CHANNELS; i++)
            players[i] = new WCAudioPlayer(i);
    }

    ~WCAudioDevice() {
        for (int i = 0; i < WC_PLAYER_CHANNELS; i++) delete players[i];
        if (initialized) {
            Mix_HookMusic(nullptr, nullptr);
            Mix_CloseAudio();
            Mix_Quit();
        }
    }

    bool init() {
        Mix_Init(MIX_INIT_OGG | MIX_INIT_FLAC); // MP3 géré par minimp3, pas besoin de MIX_INIT_MP3
        if (Mix_OpenAudio(WC_AUDIO_FREQUENCY, WC_AUDIO_FORMAT,
                          WC_AUDIO_CHANNELS, WC_AUDIO_CHUNKSIZE) < 0) {
            return false;
        }
        Mix_HookMusic(wc_hook_music, nullptr);
        initialized = true;
        return true;
    }

    const char* getName() const { return "SDL2_mixer"; }
    bool        isOk()    const { return initialized; }

    WCAudioPlayer* getPlayer(int i) {
        if (i < 0 || i >= WC_PLAYER_CHANNELS) return nullptr;
        return players[i];
    }
};

// ============================================================
// Typedefs de compatibilité Audiere
// ============================================================
typedef WCAudioDevice* AudioDevicePtr;
typedef WCAudioPlayer* OutputStreamPtr;

struct AudioDeviceDesc { std::string name, description; };

// ============================================================
// Fonctions globales (API Audiere)
// ============================================================
inline AudioDevicePtr OpenDevice(int /*index*/ = 0) {
    WCAudioDevice* dev = new WCAudioDevice();
    if (!dev->init()) { delete dev; return nullptr; }
    return dev;
}

static int wc_next_player_channel = 0;

// ch_hint >= 0 : canal imposé (reload sur le bon player)
// ch_hint <  0 : compteur global (premier chargement)
inline OutputStreamPtr OpenSound(AudioDevicePtr device, const char* filename,
                                  int ch_hint = -1) {
    if (!device || !filename) return nullptr;
    int ch;
    if (ch_hint >= 0 && ch_hint < WC_PLAYER_CHANNELS) {
        ch = ch_hint;
    } else {
        ch = wc_next_player_channel;
        if (ch >= WC_PLAYER_CHANNELS) ch = WC_PLAYER_CHANNELS - 1;
        wc_next_player_channel++;
    }
    WCAudioPlayer* player = device->getPlayer(ch);
    if (!player) return nullptr;
    if (!player->loadFile(filename)) return nullptr;
    return player;
}

inline void GetSupportedAudioDevices(std::vector<AudioDeviceDesc>& devices) {
    int n = SDL_GetNumAudioDevices(0);
    devices.clear();
    for (int i = 0; i < n; i++) {
        AudioDeviceDesc d;
        const char* nm = SDL_GetAudioDeviceName(i, 0);
        d.name = d.description = nm ? nm : "unknown";
        devices.push_back(d);
    }
}

} 

#endif // AUDIO_BACKEND_H
