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

// ============================================================
// Format de sortie — doit correspondre aux paramètres de Mix_OpenAudio()
// ============================================================
#define WC_AUDIO_FREQUENCY   44100
#define WC_AUDIO_CHANNELS    2
#define WC_AUDIO_CHUNKSIZE   4096
#define WC_AUDIO_FORMAT      AUDIO_S16LSB   // S16 little-endian stéréo

#define WC_PLAYER_CHANNELS   4
#define WC_BYTES_PER_FRAME   4  // sizeof(Sint16) * 2 canaux

namespace audiere {

// ============================================================
// Utilitaire : détection MP3 par extension
// ============================================================
static bool wc_is_mp3_file(const char* fn) {
    const char* dot = strrchr(fn, '.');
    if (!dot) return false;
    const char* e = dot + 1;
    return (e[0]=='m'||e[0]=='M') && (e[1]=='p'||e[1]=='P') &&
           (e[2]=='3') && e[3]=='\0';
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

    // ---- Mode MP3 streaming (minimp3) ----
    SDL_RWops*    mp3_rw;
    mp3dec_t      mp3dec;
    Uint8         mp3_buf[WC_MP3_BUF_SIZE];
    int           mp3_buf_fill;
    bool          mp3_eof;
    Sint64        mp3_file_size;
    int           mp3_channels, mp3_hz;

    // ---- Mode Chunk (OGG/FLAC décodé en RAM par SDL_mixer) ----
    Mix_Chunk*    chunk;
    Uint32        chunk_pos;

    // ---- Convertisseur de format (source → WC_AUDIO_FORMAT/CHANNELS/FREQUENCY) ----
    SDL_AudioStream* conv;

    // ---- État de lecture ----
    bool    playing, looping;
    float   vol, pan;
    Uint32  length_ms;
    Uint32  start_tick;
    int     sample_hz;  // sample rate du fichier chargé (pour conversion ms↔samples)

    WCStreamState() { SDL_zero(*this); }

    void cleanup_locked() {
        if (conv)   { SDL_FreeAudioStream(conv); conv   = nullptr; }
        if (rw)     { SDL_RWclose(rw);           rw     = nullptr; }
        if (mp3_rw) { SDL_RWclose(mp3_rw);       mp3_rw = nullptr; }
        if (chunk)  { Mix_FreeChunk(chunk);       chunk  = nullptr; }
        playing = false;
        cur_byte = 0; chunk_pos = 0;
        mp3_buf_fill = 0; mp3_eof = false;
    }
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

// Remplit le convertisseur 'conv' depuis le fichier WAV streamé
static void wc_fill_wav(WCStreamState& s, int need) {
    static Uint8 buf[8192];
    while (SDL_AudioStreamAvailable(s.conv) < need) {
        Sint64 rem = s.data_size - s.cur_byte;
        if (rem <= 0) {
            if (s.looping) {
                SDL_RWseek(s.rw, s.data_start, RW_SEEK_SET);
                s.cur_byte = 0; rem = s.data_size;
                SDL_AudioStreamClear(s.conv);
            } else { s.playing = false; break; }
        }
        int to_read = (int)(rem < 8192 ? rem : 8192);
        int got = (int)SDL_RWread(s.rw, buf, 1, to_read);
        if (got <= 0) { s.playing = false; break; }
        s.cur_byte += got;
        SDL_AudioStreamPut(s.conv, buf, got);
    }
}

// Log de diagnostic audio (debug — chemin absolu via %TEMP%)
// Log via Win32 WriteFile — fonctionne depuis n'importe quel thread (pas MSVCRT)
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
    wvsprintfA(buf, fmt, ap); // Win32, sans MSVCRT
    va_end(ap);
    wc_audio_log(buf);
}

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
                s.chunk_pos = 0; rem = s.chunk->alen;
                SDL_AudioStreamClear(s.conv);
            } else { s.playing = false; break; }
        }
        int to_read = (int)(rem < 8192 ? rem : 8192);
        SDL_memcpy(buf, s.chunk->abuf + s.chunk_pos, to_read);
        s.chunk_pos += to_read;
        SDL_AudioStreamPut(s.conv, buf, to_read);
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
        if      (s.rw)     wc_fill_wav  (s, len);
        else if (s.mp3_rw) wc_fill_mp3  (s, len);
        else if (s.chunk)  wc_fill_chunk(s, len);

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
            dst[f*2]   = wc_clamp16(dst[f*2]   + (Sint32)(src[f*2]   * vL));
            dst[f*2+1] = wc_clamp16(dst[f*2+1] + (Sint32)(src[f*2+1] * vR));
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

        // ---- 1. Essai WAV streaming (PCM brut) ----
        SDL_RWops* rw = SDL_RWFromFile(filename, "rb");
        if (rw) {
            WCWavInfo info;
            if (wc_parse_wav_header(rw, &info)) {
                SDL_AudioStream* conv = make_conv(info.spec.format,
                                                  info.spec.channels, info.spec.freq);
                if (conv) {
                    SDL_LockAudio();
                    S().cleanup_locked();
                    S().rw         = rw;
                    S().data_start = info.data_offset;
                    S().data_size  = info.data_size;
                    S().cur_byte   = 0;
                    S().src_spec   = info.spec;
                    S().conv       = conv;
                    S().length_ms  = length_from_wav_stream();
                    S().sample_hz  = info.spec.freq;
                    restore_prefs(sv, sp, sl);
                    SDL_UnlockAudio();
                    return true;
                }
                SDL_FreeAudioStream(conv);
            }
            SDL_RWclose(rw);
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

                        SDL_LockAudio();
                        S().cleanup_locked();
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
                        return true;
                    }
                    SDL_FreeAudioStream(conv);
                }
                SDL_RWclose(mp3_rw);
            }
        }

        // ---- 3. Fallback : Mix_LoadWAV pour OGG/FLAC/autres ----
        Mix_Chunk* chunk = Mix_LoadWAV(filename);
        if (!chunk) return false;

        // Mix_LoadWAV décode vers le format du mixer (WC_AUDIO_FORMAT, stéréo, 44100)
        SDL_AudioStream* conv = make_conv(WC_AUDIO_FORMAT, WC_AUDIO_CHANNELS, WC_AUDIO_FREQUENCY);
        if (!conv) { Mix_FreeChunk(chunk); return false; }

        SDL_LockAudio();
        S().cleanup_locked();
        S().chunk     = chunk;
        S().chunk_pos = 0;
        S().conv      = conv;
        S().length_ms = length_from_chunk();
        S().sample_hz = WC_AUDIO_FREQUENCY;
        restore_prefs(sv, sp, sl);
        SDL_UnlockAudio();
        return true;
    }

    // ---- API Audiere ----

    void play() {
        SDL_LockAudio();
        WCStreamState& s = S();
        if (s.rw) {
            SDL_RWseek(s.rw, s.data_start, RW_SEEK_SET);
            s.cur_byte = 0;
        } else if (s.mp3_rw) {
            SDL_RWseek(s.mp3_rw, 0, RW_SEEK_SET);
            s.mp3_buf_fill = 0;
            s.mp3_eof      = false;
            mp3dec_init(&s.mp3dec);
            // Pré-charge le buffer depuis le thread appelant (principal/timer),
            // car SDL_RWread depuis le thread audio (MSVCRT non initialisé) peut retourner 0.
            int got = (int)SDL_RWread(s.mp3_rw, s.mp3_buf, 1, WC_MP3_BUF_SIZE);
            if (got > 0) s.mp3_buf_fill = got;
            else         s.mp3_eof = true;
        } else if (s.chunk) {
            s.chunk_pos = 0;
        }
        if (s.conv) SDL_AudioStreamClear(s.conv);
        s.start_tick = SDL_GetTicks();
        s.playing    = true;
        SDL_UnlockAudio();
    }

    void stop() {
        SDL_LockAudio();
        S().playing = false;
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
            if (s.rw) {
                SDL_RWseek(s.rw, s.data_start, RW_SEEK_SET);
                s.cur_byte = 0;
            } else if (s.mp3_rw) {
                SDL_RWseek(s.mp3_rw, 0, RW_SEEK_SET);
                s.mp3_buf_fill = 0; s.mp3_eof = false;
                mp3dec_init(&s.mp3dec);
            } else if (s.chunk) {
                s.chunk_pos = 0;
            }
            if (s.conv) SDL_AudioStreamClear(s.conv);
            s.start_tick = SDL_GetTicks();
        } else if (s.rw && s.src_spec.freq && s.src_spec.channels) {
            // Seek précis dans WAV
            int bpf = (SDL_AUDIO_BITSIZE(s.src_spec.format) / 8) * s.src_spec.channels;
            if (bpf > 0) {
                Sint64 bp = (Sint64)((double)pos_ms / 1000.0 * s.src_spec.freq * bpf);
                if (bp < 0) bp = 0;
                if (bp >= s.data_size) bp = 0;
                SDL_RWseek(s.rw, s.data_start + bp, RW_SEEK_SET);
                s.cur_byte = bp;
                if (s.conv) SDL_AudioStreamClear(s.conv);
            }
        } else if (s.mp3_rw && s.length_ms > 0 && s.mp3_file_size > 0) {
            // Seek approximatif dans MP3 (CBR uniquement)
            Sint64 bp = (Sint64)((double)pos_ms / s.length_ms * s.mp3_file_size);
            if (bp < 0) bp = 0;
            if (bp >= s.mp3_file_size) bp = 0;
            SDL_RWseek(s.mp3_rw, bp, RW_SEEK_SET);
            s.mp3_buf_fill = 0; s.mp3_eof = false;
            mp3dec_init(&s.mp3dec); // reset décodeur (perd sync, rattrapé rapidement)
            if (s.conv) SDL_AudioStreamClear(s.conv);
        }
        SDL_UnlockAudio();
    }

    float getPosition() const {
        const WCStreamState& s = S();
        if (!s.playing) return 0.0f;
        int hz = s.sample_hz > 0 ? s.sample_hz : 44100;
        return (float)((double)(SDL_GetTicks() - s.start_tick) * hz / 1000.0);
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

    float getPitchShift() const { return 1.0f; }
    void  setPitchShift(float)  {}
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
                                  bool /*preloaded*/ = false, int ch_hint = -1) {
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

} // namespace audiere

#endif // AUDIO_BACKEND_H
