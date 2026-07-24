/*-------------------------------------------------------------------------------------------------------------
                                 |
          CWWWWWWWW              | Copyright (C) 2009-2016  Christoph Guillermet
       WWWWWWWWWWWWWWW           |               2026       Jacques Bouault - arpschuino.fr
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
 * \file graphics_backend.h
 * \brief Couche d'abstraction graphique - remplace Allegro 4 + OpenLayer par SDL2
 * \author Jacques Bouault - arpschuino.fr
 * \version 0.9.0
 * \date 2026
 *
 * Reproduit l'API Allegro 4 + OpenLayer (ol::) sur SDL2 + SDL2_ttf + SDL2_image.
 * Le reste du code WhiteCat peut continuer d'utiliser Vec2D, Rgba, Canvas::Fill(),
 * Canvas::Refresh(), Line().Draw(), TextRenderer, Bitmap, install_int_ex(), etc.
 * sans modification majeure.
 *
 * Replaces Allegro 4 + OpenLayer with SDL2 + SDL2_ttf + SDL2_image.
 * The rest of WhiteCat code continues to use Vec2D, Rgba, Canvas::Fill(),
 * Canvas::Refresh(), Line().Draw(), TextRenderer, Bitmap, install_int_ex(), etc.
 * with minimal changes.
 **/

#ifndef GRAPHICS_BACKEND_H
#define GRAPHICS_BACKEND_H

// Log dans le dossier TEMP (hors Nextcloud, pas de verrouillage de sync)
// Initialisé dans main() via GetTempPathA (après inclusion de windows.h)
static char wc_log_path[512] = "wc_debug.txt"; // fallback relatif
#define WC_LOG_FILE wc_log_path
#undef DLOG
#define DLOG(msg)                                                                                                      \
    {                                                                                                                  \
        FILE *_d = fopen(WC_LOG_FILE, "a");                                                                            \
        if (_d) {                                                                                                      \
            fprintf(_d, msg "\n");                                                                                     \
            fclose(_d);                                                                                                \
        }                                                                                                              \
    }

// ============================================================
// Bloquer tous les includes Allegro 4 + OpenLayer
// (les autres .cpp qui font #include <allegro.h> seront ignorés)
// Block all Allegro 4 + OpenLayer includes
// (other .cpp files doing #include <allegro.h> will be silently ignored)
// ============================================================
// wc_platform.h gère WIN32_LEAN_AND_MEAN, _WINSOCKAPI_, windows.h, winsock2.h
// et fournit les stubs POSIX (SOCKET, WSAStartup…) pour Linux/macOS.
#include "wc_platform.h"

#define ALLEGRO_H
#define WIN_ALLEGRO_H
#define OPENLAYER_HPP
// Sous-headers Allegro / Allegro sub-headers
#define ALLEGRO_3D_H
#define ALLEGRO_3DMATHS_H
#define ALLEGRO_COMPAT_H
#define ALLEGRO_BASE_H
#define ALLEGRO_COLOR_H
#define ALLEGRO_COMPILED_H
#define ALLEGRO_CONFIG_H
#define ALLEGRO_DATAFILE_H
#define ALLEGRO_DEBUG_H
#define ALLEGRO_DIGI_H
#define ALLEGRO_DRAW_H
#define ALLEGRO_FILE_H
#define ALLEGRO_FIX_H
#define ALLEGRO_FIXED_H
#define ALLEGRO_FLI_H
#define ALLEGRO_FMATH_H
#define ALLEGRO_FONT_H
#define ALLEGRO_GFX_H
#define ALLEGRO_GRAPHICS_H
#define ALLEGRO_GUI_H
#define ALLEGRO_JOYSTICK_H
#define ALLEGRO_KEYBOARD_H
#define ALLEGRO_LZSS_H
#define ALLEGRO_MATRIX_H
#define ALLEGRO_MIDI_H
#define ALLEGRO_MOUSE_H
#define ALLEGRO_PALETTE_H
#define ALLEGRO_QUAT_H
#define ALLEGRO_RLE_H
#define ALLEGRO_SOUND_H
#define ALLEGRO_STREAM_H
#define ALLEGRO_SYSTEM_H
#define ALLEGRO_TEXT_H
#define ALLEGRO_TIMER_H
#define ALLEGRO_UNICODE__H
// Guards loadpng / jpgalleg
#define LOADPNG_H
#define JPGALLEG_H

#include <SDL2/SDL.h>
// Fallbacks pour SDL < 2.24 (ex. Ubuntu 20.04 = SDL 2.0.10) : ces hints DPI
// (Windows uniquement) n'existent pas ; SDL_SetHint ignore un hint inconnu.
#ifndef SDL_HINT_WINDOWS_DPI_AWARENESS
#define SDL_HINT_WINDOWS_DPI_AWARENESS "SDL_WINDOWS_DPI_AWARENESS"
#endif
#ifndef SDL_HINT_WINDOWS_DPI_SCALING
#define SDL_HINT_WINDOWS_DPI_SCALING "SDL_WINDOWS_DPI_SCALING"
#endif
// SDL_HINT_APP_ID (SDL >= 2.0.18) : app_id Wayland, pour associer la fenêtre
// à whitecat.desktop (donc à l'icône). Fallback inerte sur SDL plus ancien.
#ifndef SDL_HINT_APP_ID
#define SDL_HINT_APP_ID "SDL_APP_ID"
#endif
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_syswm.h>
#include <SDL2/SDL_ttf.h>

#include <GL/gl.h>   // GL_LINES, GL_LINE_STIPPLE, etc. (utilise directement dans plot9.cpp)
#include <algorithm> // std::max, std::min, std::swap
#include <cmath>
#include <cstdio>
#include <cstring>
#include <queue>
#include <string>
#include <vector>
// windows.h + winsock2.h déjà inclus via wc_platform.h ci-dessus.

// index_quit est defini dans whitecat.h comme bool (meme unite de compilation)
extern bool index_quit;

// ============================================================
// Etat global SDL2
// Global SDL2 state
// ============================================================
#ifndef WC_SKIP_GLOBALS
SDL_Window *wc_sdl_window = nullptr;
SDL_Renderer *wc_sdl_renderer = nullptr;
// Texture persistante pour dirty-rects (Phase 6) :
// tous les rendus y sont redirigés via Canvas::Fill ; blit vers l'écran dans Canvas::Refresh.
SDL_Texture *wc_ui_texture = nullptr;
#else
extern SDL_Window *wc_sdl_window;
extern SDL_Renderer *wc_sdl_renderer;
extern SDL_Texture *wc_ui_texture;
#endif

// SCREEN_W / SCREEN_H : equivalents Allegro, mis a jour par Setup::SetupScreen
#ifndef WC_SKIP_GLOBALS
int SCREEN_W = 1280;
int SCREEN_H = 800;
#else
extern int SCREEN_W;
extern int SCREEN_H;
#endif


// ============================================================
// Macros Allegro no-op (interruptions, verrouillage memoire)
// Allegro no-op macros (interrupts, memory locking)
// ============================================================
#define LOCK_FUNCTION(f) ((void)0)
#define LOCK_VARIABLE(v) ((void)0)
#define END_OF_FUNCTION(f)
#define END_OF_MAIN()

// ============================================================
// Macros de conversion de temps Allegro
// Allegro time conversion macros (PIT frequency = 1193181 ticks/s)
// ============================================================
#define BPS_TO_TIMER(x) ((int)(1193181 / (x)))
#define BPM_TO_TIMER(x) ((int)(1193181 * 60 / (x)))
#define SECS_TO_TIMER(x) ((int)(1193181 * (x)))
#define MSEC_TO_TIMER(x) ((int)(1193181.0 * (x) / 1000.0))

// ============================================================
// Systeme de timers (install_int_ex → SDL_AddTimer)
// Timer system (install_int_ex → SDL_AddTimer)
// Les callbacks SDL tournent dans un thread separe.
// SDL timer callbacks run in a separate thread.
// Les fonctions ticker() incrementent des volatile int : OK sur x86.
// ticker() functions increment volatile ints : OK on x86.
// ============================================================
typedef void (*wc_timer_func_t)(void);

struct WC_TimerEntry {
    SDL_TimerID id;
    wc_timer_func_t func;
};

static std::vector<WC_TimerEntry> wc_timers;

struct WC_TimerParam {
    wc_timer_func_t func;
    const char *name;
};
volatile static const char *wc_current_timer = "none";
static Uint32 wc_sdl_timer_cb(Uint32 interval, void *param) {
    WC_TimerParam *p = (WC_TimerParam *)param;
    wc_current_timer = p->name;
    try {
        p->func();
    } catch (const std::exception &e) {
        FILE *f = fopen(WC_LOG_FILE, "a");
        if (f) {
            fprintf(f, "*** EXCEPTION in timer %s: %s\n", p->name, e.what());
            fclose(f);
        }
    } catch (...) {
        FILE *f = fopen(WC_LOG_FILE, "a");
        if (f) {
            fprintf(f, "*** UNKNOWN EXCEPTION in timer %s\n", p->name);
            fclose(f);
        }
    }
    wc_current_timer = "done";
    return interval;
}

inline void install_timer() { /* SDL_INIT_TIMER already done in Setup::SetupProgram */
}

inline void install_int_ex(wc_timer_func_t func, int allegro_ticks, const char *name = "?") {
    Uint32 ms = (Uint32)(allegro_ticks * 1000.0 / 1193181.0);
    if (ms < 1)
        ms = 1;
    WC_TimerParam *p = new WC_TimerParam{func, name};
    SDL_TimerID id = SDL_AddTimer(ms, wc_sdl_timer_cb, (void *)p);
    wc_timers.push_back({id, func});
}

inline void remove_int(wc_timer_func_t func) {
    for (auto it = wc_timers.begin(); it != wc_timers.end(); ++it) {
        if (it->func == func) {
            SDL_RemoveTimer(it->id);
            wc_timers.erase(it);
            return;
        }
    }
}

inline void remove_timer() {
    for (auto &t : wc_timers)
        SDL_RemoveTimer(t.id);
    wc_timers.clear();
}

// ============================================================
// Flags Allegro input
// ============================================================
#define KEYBOARD 0x01
#define MOUSE 0x02

// Flags evenements souris / Mouse event flags
#define MOUSE_FLAG_MOVE 0x001
#define MOUSE_FLAG_LEFT_DOWN 0x002
#define MOUSE_FLAG_LEFT_UP 0x004
#define MOUSE_FLAG_RIGHT_DOWN 0x008
#define MOUSE_FLAG_RIGHT_UP 0x010
#define MOUSE_FLAG_MIDDLE_DOWN 0x020
#define MOUSE_FLAG_MIDDLE_UP 0x040
#define MOUSE_FLAG_LEFT_DBLCLICK 0x080

// Modificateurs clavier / Keyboard modifiers
#define KB_SHIFT_FLAG 0x0001
#define KB_CAPSLOCK_FLAG 0x0002
#define KB_CTRL_FLAG 0x0004
#define KB_ALT_FLAG 0x0008
#define KB_NUMLOCK_FLAG 0x0020
#define KB_SCRLOCK_FLAG 0x0100

// Mode affichage / Display switch modes (stubs)
#define SWITCH_NONE 0
#define SWITCH_PAUSE 1
#define SWITCH_AMNESIA 2
#define SWITCH_BACKGROUND 3
#define SWITCH_BACKAMNESIA 4

// Mode fenetre / Window modes
#define WINDOWED 0
#define FULLSCREEN 1

// ============================================================
// Variables globales souris / Global mouse variables
// ============================================================
#ifndef WC_SKIP_GLOBALS
volatile int mouse_x = 0;
volatile int mouse_y = 0;
#else
extern volatile int mouse_x;
extern volatile int mouse_y;
#endif

#ifndef WC_SKIP_GLOBALS
int wc_mouse_range_x1 = 0;
int wc_mouse_range_y1 = 0;
int wc_mouse_range_x2 = 1279;
int wc_mouse_range_y2 = 799;
#else
extern int wc_mouse_range_x1;
extern int wc_mouse_range_y1;
extern int wc_mouse_range_x2;
extern int wc_mouse_range_y2;
#endif

typedef void (*wc_mouse_cb_t)(int flags);
static wc_mouse_cb_t mouse_callback = nullptr;

// ============================================================
// Variables globales clavier / Global keyboard variables
// ============================================================
#ifndef WC_SKIP_GLOBALS
int key_shifts = 0;
std::queue<int> wc_key_queue; // format Allegro : (scancode<<8)|ascii
#else
extern int key_shifts;
extern std::queue<int> wc_key_queue;
#endif

// Buffer SDL_TEXTINPUT : caractères produits par le système (accents, ponctuation, layout-aware)
// Utilisé uniquement en mode nom (F5, index_type==1). static : partagé dans le TU MAIN.
static std::string wc_textinput_buf;
static std::string wc_confirm_textinput_buf;
extern bool index_type;           // défini dans ui_indexes.cpp
extern bool index_confirm_name_active; // défini dans ui_indexes.cpp

// Scancodes Allegro courants / Common Allegro scancodes
#define KEY_A SDL_SCANCODE_A
#define KEY_B SDL_SCANCODE_B
#define KEY_C SDL_SCANCODE_C
#define KEY_D SDL_SCANCODE_D
#define KEY_E SDL_SCANCODE_E
#define KEY_F SDL_SCANCODE_F
#define KEY_G SDL_SCANCODE_G
#define KEY_H SDL_SCANCODE_H
#define KEY_I SDL_SCANCODE_I
#define KEY_J SDL_SCANCODE_J
#define KEY_K SDL_SCANCODE_K
#define KEY_L SDL_SCANCODE_L
#define KEY_M SDL_SCANCODE_M
#define KEY_N SDL_SCANCODE_N
#define KEY_O SDL_SCANCODE_O
#define KEY_P SDL_SCANCODE_P
#define KEY_Q SDL_SCANCODE_Q
#define KEY_R SDL_SCANCODE_R
#define KEY_S SDL_SCANCODE_S
#define KEY_T SDL_SCANCODE_T
#define KEY_U SDL_SCANCODE_U
#define KEY_V SDL_SCANCODE_V
#define KEY_W SDL_SCANCODE_W
#define KEY_X SDL_SCANCODE_X
#define KEY_Y SDL_SCANCODE_Y
#define KEY_Z SDL_SCANCODE_Z
#define KEY_0 SDL_SCANCODE_0
#define KEY_1 SDL_SCANCODE_1
#define KEY_2 SDL_SCANCODE_2
#define KEY_3 SDL_SCANCODE_3
#define KEY_4 SDL_SCANCODE_4
#define KEY_5 SDL_SCANCODE_5
#define KEY_6 SDL_SCANCODE_6
#define KEY_7 SDL_SCANCODE_7
#define KEY_8 SDL_SCANCODE_8
#define KEY_9 SDL_SCANCODE_9
#define KEY_F1 SDL_SCANCODE_F1
#define KEY_F2 SDL_SCANCODE_F2
#define KEY_F3 SDL_SCANCODE_F3
#define KEY_F4 SDL_SCANCODE_F4
#define KEY_F5 SDL_SCANCODE_F5
#define KEY_F6 SDL_SCANCODE_F6
#define KEY_F7 SDL_SCANCODE_F7
#define KEY_F8 SDL_SCANCODE_F8
#define KEY_F9 SDL_SCANCODE_F9
#define KEY_F10 SDL_SCANCODE_F10
#define KEY_F11 SDL_SCANCODE_F11
#define KEY_F12 SDL_SCANCODE_F12
#define KEY_ESC SDL_SCANCODE_ESCAPE
#define KEY_ENTER SDL_SCANCODE_RETURN
#define KEY_SPACE SDL_SCANCODE_SPACE
#define KEY_BACKSPACE SDL_SCANCODE_BACKSPACE
#define KEY_TAB SDL_SCANCODE_TAB
#define KEY_DEL SDL_SCANCODE_DELETE
#define KEY_INSERT SDL_SCANCODE_INSERT
#define KEY_HOME SDL_SCANCODE_HOME
#define KEY_END SDL_SCANCODE_END
#define KEY_PGUP SDL_SCANCODE_PAGEUP
#define KEY_PGDN SDL_SCANCODE_PAGEDOWN
#define KEY_UP SDL_SCANCODE_UP
#define KEY_DOWN SDL_SCANCODE_DOWN
#define KEY_LEFT SDL_SCANCODE_LEFT
#define KEY_RIGHT SDL_SCANCODE_RIGHT
#define KEY_LSHIFT SDL_SCANCODE_LSHIFT
#define KEY_RSHIFT SDL_SCANCODE_RSHIFT
#define KEY_LCONTROL SDL_SCANCODE_LCTRL
#define KEY_RCONTROL SDL_SCANCODE_RCTRL
#define KEY_ALT SDL_SCANCODE_LALT
#define KEY_ALTGR SDL_SCANCODE_RALT
#define KEY_MINUS SDL_SCANCODE_MINUS
#define KEY_PLUS SDL_SCANCODE_EQUALS
#define KEY_SLASH SDL_SCANCODE_SLASH
#define KEY_NUMPAD0 SDL_SCANCODE_KP_0
#define KEY_NUMPAD1 SDL_SCANCODE_KP_1
#define KEY_NUMPAD2 SDL_SCANCODE_KP_2
#define KEY_NUMPAD3 SDL_SCANCODE_KP_3
#define KEY_NUMPAD4 SDL_SCANCODE_KP_4
#define KEY_NUMPAD5 SDL_SCANCODE_KP_5
#define KEY_NUMPAD6 SDL_SCANCODE_KP_6
#define KEY_NUMPAD7 SDL_SCANCODE_KP_7
#define KEY_NUMPAD8 SDL_SCANCODE_KP_8
#define KEY_NUMPAD9 SDL_SCANCODE_KP_9
#define KEY_ENTER_PAD SDL_SCANCODE_KP_ENTER

// ============================================================
// Fonctions clavier / Keyboard functions
// ============================================================
inline bool keypressed() {
    return !wc_key_queue.empty();
}

inline int readkey() {
    if (wc_key_queue.empty())
        return 0;
    int k = wc_key_queue.front();
    wc_key_queue.pop();
    return k;
}

inline void install_keyboard() {
}
inline void install_mouse() {
}

// ============================================================
// Types de base Allegro manquants / Missing Allegro base types
// ============================================================
typedef unsigned char byte;
typedef long fixed; // Allegro fixed-point 16.16

// Maths virgule fixe Allegro / Allegro fixed-point math
inline fixed itofix(int x) {
    return (fixed)(x << 16);
}
inline fixed ftofix(double x) {
    return (fixed)(x * 65536.0);
}
inline int fixtoi(fixed x) {
    return (int)(x >> 16);
}
inline fixed fixmul(fixed a, fixed b) {
    return (fixed)(((long long)a * b) >> 16);
}
inline fixed fixsqrt(fixed x) {
    return (fixed)(sqrt((double)x / 65536.0) * 65536.0);
}
// Angles binaires : 65536 = 360 degrees / Binary angles: 65536 = 360 degrees
inline fixed fixcos(fixed x) {
    return (fixed)(cos((double)x * 2.0 * 3.14159265358979 / 65536.0) * 65536.0);
}
inline fixed fixsin(fixed x) {
    return (fixed)(sin((double)x * 2.0 * 3.14159265358979 / 65536.0) * 65536.0);
}
inline fixed fixatan2(fixed y, fixed x) {
    double angle = atan2((double)y, (double)x);
    return (fixed)(angle / (2.0 * 3.14159265358979) * 65536.0);
}

// calc_spline : courbe de Bezier Allegro
// points[8] = {x0,y0, cx1,cy1, cx2,cy2, x3,y3}
// npts = nombre de points de sortie
inline void calc_spline(const int points[8], int npts, int *x_out, int *y_out) {
    double x0 = points[0], y0 = points[1];
    double x1 = points[2], y1 = points[3];
    double x2 = points[4], y2 = points[5];
    double x3 = points[6], y3 = points[7];
    for (int i = 0; i < npts; i++) {
        double t = (double)i / (npts - 1);
        double t2 = t * t, t3 = t2 * t;
        double mt = 1.0 - t, mt2 = mt * mt, mt3 = mt2 * mt;
        x_out[i] = (int)(mt3 * x0 + 3 * mt2 * t * x1 + 3 * mt * t2 * x2 + t3 * x3);
        y_out[i] = (int)(mt3 * y0 + 3 * mt2 * t * y1 + 3 * mt * t2 * y2 + t3 * y3);
    }
}

// Utilitaires fichiers Allegro / Allegro file utilities
inline void get_executable_name(char *buf, int size) {
#ifdef _WIN32
    GetModuleFileNameA(NULL, buf, (DWORD)size);
#else
    ssize_t n = readlink("/proc/self/exe", buf, (size_t)(size - 1));
    if (n > 0) buf[n] = '\0'; else { buf[0] = '.'; buf[1] = '\0'; }
#endif
}
inline const char *get_filename(const char *path) {
    const char *p = path;
    const char *last = path;
    while (*p) {
        if (*p == '/' || *p == '\\')
            last = p + 1;
        p++;
    }
    return last;
}
inline void replace_filename(char *dest, const char *path, const char *newname, int size) {
    const char *fn = get_filename(path);
    int base_len = (int)(fn - path);
    if (base_len >= size)
        base_len = size - 1;
    strncpy(dest, path, base_len);
    dest[base_len] = '\0';
    strncat(dest, newname, size - base_len - 1);
}

// chdir : inclus via wc_platform.h (direct.h + _chdir sur Windows, unistd.h sur POSIX)

// Constantes Allegro texte / Allegro text alignment constants
#define ALLEGRO_ALIGN_LEFT 0
#define ALLEGRO_ALIGN_CENTRE 1
#define ALLEGRO_ALIGN_CENTER 1
#define ALLEGRO_ALIGN_RIGHT 2
#define CENTER ALLEGRO_ALIGN_CENTER
#define JUSTIFY 3

// Constantes Allegro joystick / Allegro joystick constants
#define JOY_TYPE_AUTODETECT -1
#define JOY_TYPE_NONE 0

// Constantes Allegro 3D / Allegro 3D polygon type
#define POLYTYPE_FLAT 0
#define POLYTYPE_GCOL 1
#define POLYTYPE_GRGB 2
#define POLYTYPE_ATEX 3
#define POLYTYPE_PTEX 4
#define POLYTYPE_ATEX_MASK 5
#define POLYTYPE_PTEX_MASK 6
#define POLYTYPE_ATEX_LIT 7
#define POLYTYPE_PTEX_LIT 8
#define POLYTYPE_ATEX_MASK_LIT 9
#define POLYTYPE_PTEX_MASK_LIT 10

// Touches Allegro manquantes / Missing Allegro key codes
#define KEY_0_PAD SDL_SCANCODE_KP_0
#define KEY_1_PAD SDL_SCANCODE_KP_1
#define KEY_2_PAD SDL_SCANCODE_KP_2
#define KEY_3_PAD SDL_SCANCODE_KP_3
#define KEY_4_PAD SDL_SCANCODE_KP_4
#define KEY_5_PAD SDL_SCANCODE_KP_5
#define KEY_6_PAD SDL_SCANCODE_KP_6
#define KEY_7_PAD SDL_SCANCODE_KP_7
#define KEY_8_PAD SDL_SCANCODE_KP_8
#define KEY_9_PAD SDL_SCANCODE_KP_9
#define KEY_DEL_PAD SDL_SCANCODE_KP_PERIOD
#define KEY_PLUS_PAD SDL_SCANCODE_KP_PLUS
#define KEY_MINUS_PAD SDL_SCANCODE_KP_MINUS
#define KEY_EQUALS SDL_SCANCODE_EQUALS
#define KEY_COMMA SDL_SCANCODE_COMMA
#define KEY_SEMICOLON SDL_SCANCODE_SEMICOLON
#define KEY_TILDE SDL_SCANCODE_GRAVE
#define KEY_PRTSCR SDL_SCANCODE_PRINTSCREEN
#define KEY_PAUSE SDL_SCANCODE_PAUSE
#define KEY_LWIN SDL_SCANCODE_LGUI
#define KEY_RWIN SDL_SCANCODE_RGUI
#define KEY_MENU SDL_SCANCODE_APPLICATION
#define KEY_STOP SDL_SCANCODE_STOP

// Struct vertex Allegro 3D (utilise dans graphics_rebuild1.cpp et trichro_core2.cpp)
struct V3D_f {
    float x, y, z;
    float u, v;
    int c; // couleur / color
};

// Forward declaration (definie plus bas / defined below)
inline void triangle(SDL_Surface *bmp, int x1, int y1, int x2, int y2, int x3, int y3, int color);

// triangle3d_f : Gouraud-shaded triangle on SDL_Surface
// Replaces Allegro POLYTYPE_GCOL — interpolates colors between 3 vertices
// using barycentric coordinates (one scanline per row, linear interpolation).
inline void triangle3d_f(SDL_Surface *bmp, int /*type*/, void * /*texture*/, V3D_f *v1, V3D_f *v2, V3D_f *v3) {
    if (!bmp || !v1 || !v2 || !v3)
        return;

    // Extract screen-space coords and packed colors
    float ax = v1->x, ay = v1->y;
    Uint32 ca = (Uint32)v1->c;
    float bx = v2->x, by = v2->y;
    Uint32 cb = (Uint32)v2->c;
    float cx = v3->x, cy = v3->y;
    Uint32 cc = (Uint32)v3->c;

    // Per-vertex color components (R<<16 G<<8 B in packed int)
    float ra = (float)((ca >> 16) & 0xFF), ga = (float)((ca >> 8) & 0xFF), ba2 = (float)(ca & 0xFF);
    float rb = (float)((cb >> 16) & 0xFF), gb = (float)((cb >> 8) & 0xFF), bb2 = (float)(cb & 0xFF);
    float rc = (float)((cc >> 16) & 0xFF), gc = (float)((cc >> 8) & 0xFF), bc2 = (float)(cc & 0xFF);

    // Bounding box clamped to surface
    int minx = (int)fminf(fminf(ax, bx), cx);
    int maxx = (int)fmaxf(fmaxf(ax, bx), cx);
    int miny = (int)fminf(fminf(ay, by), cy);
    int maxy = (int)fmaxf(fmaxf(ay, by), cy);
    if (minx < 0)
        minx = 0;
    if (maxx >= bmp->w)
        maxx = bmp->w - 1;
    if (miny < 0)
        miny = 0;
    if (maxy >= bmp->h)
        maxy = bmp->h - 1;

    // Triangle area (cross product) for barycentric denominator
    float denom = (by - cy) * (ax - cx) + (cx - bx) * (ay - cy);
    if (fabsf(denom) < 0.5f)
        return; // degenerate

    SDL_LockSurface(bmp);
    Uint32 *pixels = (Uint32 *)bmp->pixels;
    int pitch = bmp->pitch / 4;

    for (int py = miny; py <= maxy; py++) {
        for (int px = minx; px <= maxx; px++) {
            // Barycentric coords
            float w1 = ((by - cy) * (px - cx) + (cx - bx) * (py - cy)) / denom;
            float w2 = ((cy - ay) * (px - cx) + (ax - cx) * (py - cy)) / denom;
            float w3 = 1.0f - w1 - w2;
            if (w1 < 0.0f || w2 < 0.0f || w3 < 0.0f)
                continue;
            // Interpolate color
            int r = (int)(w1 * ra + w2 * rb + w3 * rc + 0.5f);
            int g = (int)(w1 * ga + w2 * gb + w3 * gc + 0.5f);
            int b = (int)(w1 * ba2 + w2 * bb2 + w3 * bc2 + 0.5f);
            if (r > 255)
                r = 255;
            if (g > 255)
                g = 255;
            if (b > 255)
                b = 255;
            pixels[py * pitch + px] = 0xFF000000u | ((Uint32)r << 16) | ((Uint32)g << 8) | (Uint32)b;
        }
    }
    SDL_UnlockSurface(bmp);
}

// Mouse Z (molette) / Mouse wheel
#ifndef WC_SKIP_GLOBALS
volatile int mouse_z = 0;
#else
extern volatile int mouse_z;
#endif
static int wc_mouse_z_value = 0;

// position_mouse_z : certains codes l'appellent comme fonction ET d'autres assignent dessus
// Some code calls it as position_mouse_z(z), others do position_mouse_z = z
struct WC_MouseZType {
    int value;
    WC_MouseZType() : value(0) {
    }
    void operator()(int z) {
        value = z;
        mouse_z = z;
    } // appel / call
    WC_MouseZType &operator=(int z) {
        value = z;
        mouse_z = z;
        return *this;
    } // assignation
    WC_MouseZType &operator+=(int z) {
        value += z;
        mouse_z += z;
        return *this;
    }
    operator int() const {
        return value;
    }
};
#ifndef WC_SKIP_GLOBALS
WC_MouseZType position_mouse_z;
#else
extern WC_MouseZType position_mouse_z;
#endif

// mouse_b : etat boutons souris format Allegro (bits 0=gauche 1=droit 2=milieu)
#ifndef WC_SKIP_GLOBALS
volatile int mouse_b = 0;
#else
extern volatile int mouse_b;
#endif

// ============================================================
// Buffer bitmap CPU (remplace Allegro BITMAP)
// On utilise SDL_Surface* directement — Win32 a deja un type BITMAP (wingdi.h)
// Use SDL_Surface* directly — Win32 already has a BITMAP type (wingdi.h)
// ============================================================
inline SDL_Surface *create_bitmap(int w, int h) {
    return SDL_CreateRGBSurface(0, w, h, 32, 0x00FF0000, 0x0000FF00, 0x000000FF, 0xFF000000);
}

inline void clear_bitmap(SDL_Surface *bmp) {
    if (bmp)
        SDL_FillRect(bmp, nullptr, 0);
}

inline void destroy_bitmap(SDL_Surface *bmp) {
    if (bmp)
        SDL_FreeSurface(bmp);
}

inline void blit(SDL_Surface *src, SDL_Surface *dst, int sx, int sy, int dx, int dy, int w, int h) {
    SDL_Rect srect = {sx, sy, w, h};
    SDL_Rect drect = {dx, dy, w, h};
    SDL_BlitSurface(src, &srect, dst, &drect);
}

// makecol : couleur packed 32-bit, alpha=0xFF (opaque)
inline int makecol(int r, int g, int b) {
    return 0xFF000000 | (r << 16) | (g << 8) | b;
}

// triangle sur SDL_Surface (calcul trichro)
// Raster triangle fill sur SDL_Surface - implementation simple scanline
inline void triangle(SDL_Surface *bmp, int x1, int y1, int x2, int y2, int x3, int y3, int color) {
    if (!bmp)
        return;
    // Tri vertices by y
    if (y1 > y2) {
        std::swap(x1, x2);
        std::swap(y1, y2);
    }
    if (y1 > y3) {
        std::swap(x1, x3);
        std::swap(y1, y3);
    }
    if (y2 > y3) {
        std::swap(x2, x3);
        std::swap(y2, y3);
    }

    Uint32 c = (Uint32)color;
    SDL_LockSurface(bmp);
    Uint32 *pixels = (Uint32 *)bmp->pixels;
    int pitch = bmp->pitch / 4;
    int bw = bmp->w, bh = bmp->h;

    auto drawHLine = [&](int y, int xa, int xb) {
        if (y < 0 || y >= bh)
            return;
        if (xa > xb)
            std::swap(xa, xb);
        if (xa < 0)
            xa = 0;
        if (xb >= bw)
            xb = bw - 1;
        Uint32 *row = pixels + y * pitch;
        for (int x = xa; x <= xb; x++)
            row[x] = c;
    };

    // Top to middle
    for (int y = y1; y <= y2; y++) {
        int xa = (y2 != y1) ? x1 + (x2 - x1) * (y - y1) / (y2 - y1) : x1;
        int xb = (y3 != y1) ? x1 + (x3 - x1) * (y - y1) / (y3 - y1) : x1;
        drawHLine(y, xa, xb);
    }
    // Middle to bottom
    for (int y = y2; y <= y3; y++) {
        int xa = (y3 != y2) ? x2 + (x3 - x2) * (y - y2) / (y3 - y2) : x2;
        int xb = (y3 != y1) ? x1 + (x3 - x1) * (y - y1) / (y3 - y1) : x1;
        drawHLine(y, xa, xb);
    }
    SDL_UnlockSurface(bmp);
}

// ============================================================
// Fonctions Allegro stubs
// ============================================================
inline int desktop_color_depth() {
    return 32;
}
inline void jpgalleg_init() {
}
inline void install_joystick(int) {
}
inline void calibrate_joystick_name(int) {
}
inline void poll_joystick() {
}
// set_display_switch_mode est defini plus bas avec retour int
inline void allegro_exit() {
    SDL_Quit();
}

inline void allegro_message(const char *msg) {
    SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "WhiteCat", msg, wc_sdl_window);
}

inline void set_mouse_range(int x1, int y1, int x2, int y2) {
    wc_mouse_range_x1 = x1;
    wc_mouse_range_y1 = y1;
    wc_mouse_range_x2 = x2;
    wc_mouse_range_y2 = y2;
}

#ifdef _WIN32
inline HWND win_get_window() {
    if (!wc_sdl_window)
        return NULL;
    SDL_SysWMinfo info;
    SDL_VERSION(&info.version);
    if (SDL_GetWindowWMInfo(wc_sdl_window, &info))
        return info.info.win.window;
    return NULL;
}
#endif

inline void rest(int ms) {
    if (ms > 0)
        SDL_Delay((Uint32)ms);
}

// Extraction composantes couleur / Color component extraction
inline int getr(int c) {
    return (c >> 16) & 0xFF;
}
inline int getg(int c) {
    return (c >> 8) & 0xFF;
}
inline int getb(int c) {
    return c & 0xFF;
}

// Pixel sur SDL_Surface / Pixel on SDL_Surface
inline int getpixel(SDL_Surface *bmp, int x, int y) {
    if (!bmp || x < 0 || y < 0 || x >= bmp->w || y >= bmp->h)
        return 0;
    SDL_LockSurface(bmp);
    Uint32 px = ((Uint32 *)((Uint8 *)bmp->pixels + y * bmp->pitch))[x];
    SDL_UnlockSurface(bmp);
    return (int)px;
}
inline void putpixel(SDL_Surface *bmp, int x, int y, int color) {
    if (!bmp || x < 0 || y < 0 || x >= bmp->w || y >= bmp->h)
        return;
    SDL_LockSurface(bmp);
    ((Uint32 *)((Uint8 *)bmp->pixels + y * bmp->pitch))[x] = (Uint32)color;
    SDL_UnlockSurface(bmp);
}

// circlefill sur SDL_Surface / circlefill on SDL_Surface
inline void circlefill(SDL_Surface *bmp, int cx, int cy, int r, int color) {
    if (!bmp)
        return;
    SDL_LockSurface(bmp);
    Uint32 *pixels = (Uint32 *)bmp->pixels;
    int pitch = bmp->pitch / 4;
    int bw = bmp->w, bh = bmp->h;
    Uint32 c = (Uint32)color;
    for (int dy = -r; dy <= r; dy++) {
        int dx = (int)sqrtf((float)(r * r - dy * dy));
        int ys = cy + dy;
        if (ys < 0 || ys >= bh)
            continue;
        int xs = cx - dx < 0 ? 0 : cx - dx;
        int xe = cx + dx >= bw ? bw - 1 : cx + dx;
        Uint32 *row = pixels + ys * pitch;
        for (int x = xs; x <= xe; x++)
            row[x] = c;
    }
    SDL_UnlockSurface(bmp);
}

// polygon sur SDL_Surface / polygon on SDL_Surface (outline only)
inline void polygon(SDL_Surface *bmp, int npts, const int *points, int color) {
    for (int i = 0; i < npts; i++) {
        int j = (i + 1) % npts;
        // Basic Bresenham line on SDL_Surface
        int x0 = points[i * 2], y0 = points[i * 2 + 1];
        int x1 = points[j * 2], y1 = points[j * 2 + 1];
        int dx = abs(x1 - x0), dy = abs(y1 - y0);
        int sx = x0 < x1 ? 1 : -1, sy = y0 < y1 ? 1 : -1, err = dx - dy;
        while (true) {
            putpixel(bmp, x0, y0, color);
            if (x0 == x1 && y0 == y1)
                break;
            int e2 = 2 * err;
            if (e2 > -dy) {
                err -= dy;
                x0 += sx;
            }
            if (e2 < dx) {
                err += dx;
                y0 += sy;
            }
        }
    }
}

// spline sur SDL_Surface / spline on SDL_Surface
inline void spline(SDL_Surface *bmp, const int points[8], int color) {
    const int N = 64;
    int px[N], py[N];
    calc_spline(points, N, px, py);
    for (int i = 0; i + 1 < N; i++) {
        // Bresenham on SDL_Surface
        int x0 = px[i], y0 = py[i], x1 = px[i + 1], y1 = py[i + 1];
        int dx = abs(x1 - x0), dy = abs(y1 - y0);
        int sx = x0 < x1 ? 1 : -1, sy = y0 < y1 ? 1 : -1, err = dx - dy;
        while (true) {
            putpixel(bmp, x0, y0, color);
            if (x0 == x1 && y0 == y1)
                break;
            int e2 = 2 * err;
            if (e2 > -dy) {
                err -= dy;
                x0 += sx;
            }
            if (e2 < dx) {
                err += dx;
                y0 += sy;
            }
        }
    }
}

// screen : surface Allegro globale — nulle en SDL2 (triangle3d_f gere le cas null)
static SDL_Surface *screen = nullptr;

// Helper interne forward decl (definie plus bas dans le fichier)
// Internal helper forward decl (defined later in file)
static inline void wc_set_render_color(SDL_Renderer *r, float rf, float gf, float bf, float af);

// triangle() sans BITMAP : dessine sur le renderer SDL (usage direct dans les .cpp)
// triangle() without BITMAP: draws on SDL renderer
inline void triangle(int x1, int y1, int x2, int y2, int x3, int y3, int color) {
    if (!wc_sdl_renderer)
        return;
    float r = ((color >> 16) & 0xFF) / 255.0f;
    float g = ((color >> 8) & 0xFF) / 255.0f;
    float b = (color & 0xFF) / 255.0f;
    wc_set_render_color(wc_sdl_renderer, r, g, b, 1.0f);
    SDL_RenderDrawLine(wc_sdl_renderer, x1, y1, x2, y2);
    SDL_RenderDrawLine(wc_sdl_renderer, x2, y2, x3, y3);
    SDL_RenderDrawLine(wc_sdl_renderer, x3, y3, x1, y1);
}

// Tableau etat clavier / Keyboard state array
// Utilise un struct avec operator[] pour eviter #define key qui casse e.key (SDL_Event)
// Uses a struct with operator[] to avoid #define key which breaks e.key (SDL_Event)
struct WC_KeyArray {
    bool operator[](int scancode) const {
        const Uint8 *state = SDL_GetKeyboardState(nullptr);
        if (scancode >= 0 && scancode < SDL_NUM_SCANCODES)
            return state[scancode] != 0;
        return false;
    }
};
#ifndef WC_SKIP_GLOBALS
WC_KeyArray key;
#else
extern WC_KeyArray key;
#endif

inline void poll_keyboard() {
    SDL_PumpEvents(); // SDL2 met a jour l'etat clavier automatiquement
}

inline void clear_keybuf() {
    while (!wc_key_queue.empty())
        wc_key_queue.pop();
}

inline void simulate_keypress(int keycode) {
    wc_key_queue.push(keycode);
}

// Conversion HSV/RGB
inline void rgb_to_hsv(int r, int g, int b, float *h, float *s, float *v) {
    float rf = r / 255.0f, gf = g / 255.0f, bf = b / 255.0f;
    float mx = std::max({rf, gf, bf}), mn = std::min({rf, gf, bf}), d = mx - mn;
    *v = mx;
    *s = (mx > 0) ? d / mx : 0;
    if (d == 0) {
        *h = 0;
        return;
    }
    if (mx == rf)
        *h = 60.0f * fmod((gf - bf) / d, 6.0f);
    else if (mx == gf)
        *h = 60.0f * ((bf - rf) / d + 2.0f);
    else
        *h = 60.0f * ((rf - gf) / d + 4.0f);
    if (*h < 0)
        *h += 360.0f;
}
inline void hsv_to_rgb(float h, float s, float v, int *r, int *g, int *b) {
    float c = v * s, x = c * (1.0f - fabsf(fmod(h / 60.0f, 2.0f) - 1.0f)), m = v - c;
    float rf = 0, gf = 0, bf = 0;
    int i = (int)(h / 60.0f) % 6;
    switch (i) {
    case 0:
        rf = c;
        gf = x;
        bf = 0;
        break;
    case 1:
        rf = x;
        gf = c;
        bf = 0;
        break;
    case 2:
        rf = 0;
        gf = c;
        bf = x;
        break;
    case 3:
        rf = 0;
        gf = x;
        bf = c;
        break;
    case 4:
        rf = x;
        gf = 0;
        bf = c;
        break;
    case 5:
        rf = c;
        gf = 0;
        bf = x;
        break;
    }
    *r = (int)((rf + m) * 255);
    *g = (int)((gf + m) * 255);
    *b = (int)((bf + m) * 255);
}

// set_display_switch_mode : retourne int (certains codes testent la valeur de retour)
// Note: la version 'void' anterieure est remplacee ici
// Note: the earlier 'void' version is replaced here
static inline int set_display_switch_mode(int) {
    return 0;
}

// Constantes alignement texte / Text alignment constants
#ifndef RIGHT
#define RIGHT 2
#endif
#ifndef LEFT
#define LEFT 0
#endif

// ============================================================
// Traitement des evenements SDL (appele dans Canvas::Refresh)
// SDL event processing (called inside Canvas::Refresh)
// ============================================================
static Uint32 wc_last_input_ms = 0;        // timestamp du dernier evenement souris/clavier/midi
static bool wc_dirty = true;               // faut-il redessiner ce frame ?
static bool wc_bg_dirty = true;           // fond (circuits, scroller, infos) à redessiner
static bool wc_win_dirty = true;          // fenêtres ouvertes à redessiner (false sur hover pur)
static bool wc_frame_was_updated = false;  // un dessin a eu lieu ce cycle
static bool wc_automation_active = false;  // wc_request_refresh() appelé depuis le dernier Canvas::Refresh
// Posé sur chaque SDL_MOUSEBUTTONDOWN gauche, consommé par la boucle principale.
// Garantit que check_graphics_mouse_handling() s'exécute même si DOWN+UP ont
// été traités dans le même wc_process_events() (mouse_button retombe à 0 trop tôt).
static bool wc_click_pending = false;
static bool wc_popup_input_hint = false; // posé sur keydown/click — lu par ticker() pour popup_alert_retrigger
// Posé par ticker() quand une popup (hors W_SEQUENCIEL) est visible.
// Lu par Canvas::Refresh() stable mode pour réduire le timeout à 40ms
// et poser wc_dirty=true sur timeout → blinkers animés sans USEREVENT.
static bool wc_blink_needed = false;

// Appelé par le backend MIDI pour maintenir le mode actif pendant les mouvements MIDI.
// wc_dirty est static-par-TU : le mettre à true ici (midi_CORE.cpp) ne suffit pas
// à réveiller le rendu dans MAIN.cpp. On pousse un SDL_USEREVENT qui :
//   1) réveille SDL_WaitEventTimeout si la boucle est en mode idle,
//   2) est traité dans wc_handle_event() (contexte MAIN.cpp) qui met wc_dirty=true
//      dans la bonne copie.
// SDL_PushEvent est thread-safe (RtMidi callback tourne dans un thread séparé).
inline void wc_notify_midi_activity() {
    wc_last_input_ms = SDL_GetTicks();
    wc_dirty = true;
    SDL_Event e;
    SDL_zero(e);
    e.type = SDL_USEREVENT;
    SDL_PushEvent(&e);
}

// Appelé par les automations (LFO, crossfade, chasers...) pour maintenir le rendu actif.
// N'update PAS wc_last_input_ms : seule l'entrée utilisateur réelle garde le cap 60fps.
// L'automation seule utilise le cap 25fps (40ms), plus doux pour le GPU.
// Pousse un SDL_USEREVENT throttlé pour réveiller SDL_WaitEventTimeout si le thread
// principal est en mode idle (sinon première frame LFO retardée jusqu'à 1000ms sur Linux).
inline void wc_request_refresh() {
    wc_dirty = true;
    wc_automation_active = true;
    wc_bg_dirty = true;
    wc_win_dirty = true;
    static volatile Uint32 wc_last_wake_push = 0;
    Uint32 now = SDL_GetTicks();
    if (now - wc_last_wake_push >= 38) {   // ~26 wakeups/s max — aligne sur le cap 25fps
        wc_last_wake_push = now;
        SDL_Event we; SDL_zero(we); we.type = SDL_USEREVENT;
        SDL_PushEvent(&we);
    }
}

// Diagnostic freeze : s'active quand W_FADERS s'ouvre, log les etapes cles
static volatile int wc_freeze_debug = 0;
#define WC_FDEBUG(msg)                                                                                                 \
    do {                                                                                                               \
        static int _cnt = 0;                                                                                           \
        if (wc_freeze_debug && _cnt < 3) {                                                                             \
            FILE *_f = fopen(WC_LOG_FILE, "a");                                                                        \
            if (_f) {                                                                                                  \
                fprintf(_f, "FDEBUG[%d]: " msg "\n", _cnt);                                                            \
                fflush(_f);                                                                                            \
                fclose(_f);                                                                                            \
            }                                                                                                          \
            _cnt++;                                                                                                    \
        }                                                                                                              \
    } while (0)

static void wc_handle_event(const SDL_Event &e) {
    switch (e.type) {

    case SDL_QUIT:
        index_quit = 1;
        break;

    case SDL_MOUSEMOTION: {
        wc_last_input_ms = SDL_GetTicks();
        // Throttle hover renders à ~15fps : curseur fluide (OS), circuits à jour toutes les 67ms.
        // Click/touche/MIDI posent wc_dirty=true directement → rendu immédiat sans throttle.
        { static Uint32 s_last_motion_ms = 0;
          Uint32 _now = SDL_GetTicks();
          if (_now - s_last_motion_ms >= 67) { wc_dirty = true; wc_win_dirty = true; s_last_motion_ms = _now; } }
        int mx = e.motion.x;
        int my = e.motion.y;
        if (mx < wc_mouse_range_x1)
            mx = wc_mouse_range_x1;
        if (mx > wc_mouse_range_x2)
            mx = wc_mouse_range_x2;
        if (my < wc_mouse_range_y1)
            my = wc_mouse_range_y1;
        if (my > wc_mouse_range_y2)
            my = wc_mouse_range_y2;
        mouse_x = mx;
        mouse_y = my;
        if (mouse_callback)
            mouse_callback(MOUSE_FLAG_MOVE);
        break;
    }

    case SDL_MOUSEBUTTONDOWN:
        wc_last_input_ms = SDL_GetTicks();
        wc_dirty = true;
        wc_bg_dirty = true;
        wc_win_dirty = true;
        wc_popup_input_hint = true;
        mouse_x = e.button.x;
        mouse_y = e.button.y;
        if (e.button.button == SDL_BUTTON_LEFT)
            mouse_b |= 1;
        if (e.button.button == SDL_BUTTON_RIGHT)
            mouse_b |= 2;
        if (e.button.button == SDL_BUTTON_MIDDLE)
            mouse_b |= 4;
        if (e.button.button == SDL_BUTTON_LEFT)
            wc_click_pending = true;
        if (e.button.button == SDL_BUTTON_LEFT && mouse_callback) {
            mouse_callback(MOUSE_FLAG_LEFT_DOWN);
            if (e.button.clicks == 2)
                mouse_callback(MOUSE_FLAG_LEFT_DBLCLICK);
        }
        if (e.button.button == SDL_BUTTON_RIGHT && mouse_callback)
            mouse_callback(MOUSE_FLAG_RIGHT_DOWN);
        if (e.button.button == SDL_BUTTON_MIDDLE && mouse_callback)
            mouse_callback(MOUSE_FLAG_MIDDLE_DOWN);
        break;

    case SDL_MOUSEBUTTONUP:
        wc_dirty = true;
        wc_bg_dirty = true;
        wc_win_dirty = true;
        mouse_x = e.button.x;
        mouse_y = e.button.y;
        if (e.button.button == SDL_BUTTON_LEFT)
            mouse_b &= ~1;
        if (e.button.button == SDL_BUTTON_RIGHT)
            mouse_b &= ~2;
        if (e.button.button == SDL_BUTTON_MIDDLE)
            mouse_b &= ~4;
        if (e.button.button == SDL_BUTTON_LEFT && mouse_callback)
            mouse_callback(MOUSE_FLAG_LEFT_UP);
        if (e.button.button == SDL_BUTTON_RIGHT && mouse_callback)
            mouse_callback(MOUSE_FLAG_RIGHT_UP);
        if (e.button.button == SDL_BUTTON_MIDDLE && mouse_callback)
            mouse_callback(MOUSE_FLAG_MIDDLE_UP);
        break;

    case SDL_MOUSEWHEEL:
        // [fix fin] la molette n'agit que si WhiteCat a le FOCUS clavier. Sinon (curseur qui survole
        // la fenetre alors qu'une AUTRE appli est active) SDL delivre quand meme l'event -> les niveaux
        // bougeaient a l'insu de l'utilisateur, avec le Ctrl (pas fin) de l'autre appli -> comportement
        // erratique. On ignore la molette hors focus : mouse_z ne bouge pas (aucune action, aucun saut).
        if (SDL_GetKeyboardFocus() != wc_sdl_window) break;
        wc_last_input_ms = SDL_GetTicks();
        wc_dirty = true;
        wc_bg_dirty = true;
        wc_win_dirty = true;
        mouse_z += e.wheel.y;
        position_mouse_z += e.wheel.y;
        break;

    case SDL_KEYDOWN: {
        if (e.key.repeat) {
            // Autoriser la répétition pour les flèches (scroll, navigation) uniquement
            SDL_Keycode _s = e.key.keysym.sym;
            bool _is_arrow = (_s == SDLK_UP || _s == SDLK_DOWN ||
                              _s == SDLK_LEFT || _s == SDLK_RIGHT ||
                              _s == SDLK_BACKSPACE);
            if (!_is_arrow) break;
        }
        wc_last_input_ms = SDL_GetTicks();
        wc_dirty = true;
        wc_bg_dirty = true;
        wc_win_dirty = true;
        wc_popup_input_hint = true;
        SDL_Keymod mod = SDL_GetModState();
        key_shifts = 0;
        if (mod & KMOD_SHIFT)
            key_shifts |= KB_SHIFT_FLAG;
        if (mod & KMOD_CTRL)
            key_shifts |= KB_CTRL_FLAG;
        if (mod & KMOD_ALT)
            key_shifts |= KB_ALT_FLAG;
        if (mod & KMOD_CAPS)
            key_shifts |= KB_CAPSLOCK_FLAG;
        if (mod & KMOD_NUM)
            key_shifts |= KB_NUMLOCK_FLAG;
#if SDL_VERSION_ATLEAST(2, 0, 18)
        if (mod & KMOD_SCROLL) // KMOD_SCROLL ajouté en SDL 2.0.18 (absent sur Ubuntu 20.04 / SDL 2.0.10)
            key_shifts |= KB_SCRLOCK_FLAG;
#endif

        int scancode = (int)e.key.keysym.scancode;
        int ascii = 0;
        SDL_Keycode sym = e.key.keysym.sym;
        if (sym >= 32 && sym < 127)
            ascii = (int)sym;
        // SDL2 renvoie toujours la minuscule dans sym ; appliquer Shift/CapsLock
        if (sym >= SDLK_a && sym <= SDLK_z) {
            bool shift_held = (mod & KMOD_SHIFT) != 0;
            bool caps_on   = (mod & KMOD_CAPS)  != 0;
            if (shift_held ^ caps_on)
                ascii -= 32; // 'a'→'A'
        }
        if ((mod & KMOD_CTRL) && sym >= SDLK_a && sym <= SDLK_z)
            ascii = (int)(sym - SDLK_a + 1);

        // Ne pas queuer les modificateurs : leur état est dans key_shifts via SDL_GetModState().
        // Sans ce filtre, tenir Shift inonde la queue de key-repeats et retarde les lettres.
        bool is_modifier = (sym == SDLK_LSHIFT || sym == SDLK_RSHIFT ||
                            sym == SDLK_LCTRL  || sym == SDLK_RCTRL  ||
                            sym == SDLK_LALT   || sym == SDLK_RALT   ||
                            sym == SDLK_LGUI   || sym == SDLK_RGUI   ||
                            sym == SDLK_CAPSLOCK || sym == SDLK_NUMLOCKCLEAR ||
                            sym == SDLK_SCROLLLOCK);
        // En mode nom (F5) : les caractères visibles arrivent via SDL_TEXTINPUT (accents, layout...).
        // On ne queuje ici que les touches spéciales : Backspace/Enter/Esc (sym<32) et Fn/flèches
        // (sym & SDLK_SCANCODE_MASK). Les lettres, chiffres, ponctuation → SDL_TEXTINPUT.
        // [fix F5] Les touches chiffres du pave numerique (KP_1..KP_0, KP_.) portent le bit
        // SDLK_SCANCODE_MASK dans leur sym -> la condition !(sym & MASK) ne les excluait PAS de la
        // queue. En mode nom F5 elles etaient alors ajoutees a la fois par la queue (case KEY_x_PAD)
        // ET par SDL_TEXTINPUT -> chaque chiffre compte double ("01" -> "0101"). On les traite comme
        // les chiffres de la rangee du haut : hors queue (SDL_TEXTINPUT s'en charge).
        bool is_numpad_char = (scancode >= SDL_SCANCODE_KP_1 && scancode <= SDL_SCANCODE_KP_PERIOD);
        bool skip_for_textinput = index_type &&
                                  !(sym < 32) &&
                                  (!(sym & SDLK_SCANCODE_MASK) || is_numpad_char);
        if (!is_modifier && !skip_for_textinput)
            wc_key_queue.push((scancode << 8) | ascii);
        break;
    }

    case SDL_TEXTINPUT:
        if (index_confirm_name_active) {
            wc_confirm_textinput_buf += e.text.text;
            wc_dirty = true;
            wc_win_dirty = true;
        } else if (index_type) {
            wc_textinput_buf += e.text.text;
            wc_dirty = true;
            wc_win_dirty = true;
        }
        break;

    case SDL_WINDOWEVENT:
        if (e.window.event == SDL_WINDOWEVENT_RESIZED) {
            SCREEN_W = e.window.data1;
            SCREEN_H = e.window.data2;
            if (wc_ui_texture && wc_sdl_renderer) {
                SDL_DestroyTexture(wc_ui_texture);
                wc_ui_texture = SDL_CreateTexture(wc_sdl_renderer, SDL_PIXELFORMAT_ARGB8888,
                                                   SDL_TEXTUREACCESS_TARGET, SCREEN_W, SCREEN_H);
            }
        }
        // [fix fin] perte de focus : on remet les modificateurs a zero. Sinon un Ctrl (ou Shift/Alt)
        // presse/relache pendant que la fenetre est inactive n'est pas vu par SDL -> etat "colle" ->
        // SDL_GetModState() perime -> molette pas-fin (Ctrl) erratique au retour dans la fenetre.
        if (e.window.event == SDL_WINDOWEVENT_FOCUS_LOST) {
            SDL_SetModState(KMOD_NONE);
        }
        wc_dirty = true;
        wc_bg_dirty = true;
        wc_win_dirty = true;
        break;

    case SDL_USEREVENT:
        // Réveillé par wc_notify_midi_activity() (thread MIDI) ou wc_request_refresh()
        // (thread ticker — LFO, chasers, crossfade). wc_dirty est static-par-TU :
        // positionné ici, dans le contexte de MAIN.cpp, pour que la boucle voie le changement.
        wc_dirty = true;
        wc_bg_dirty = true;
        wc_win_dirty = true;
        break;

    default:
        break;
    }
}

static void wc_process_events() {
    SDL_Event e;
    while (SDL_PollEvent(&e)) {
        wc_handle_event(e);
        // S'arrêter après MOUSEBUTTONDOWN : la boucle principale doit voir
        // mouse_button==1 AVANT que MOUSEBUTTONUP ne soit traité.
        // Sans ça, DOWN+UP dans la même queue sont traités ensemble et
        // mouse_button repasse à 0 avant le check if(mouse_button==1).
        if (e.type == SDL_MOUSEBUTTONDOWN)
            break;
    }
}

// ============================================================
// Helper interne : appliquer une couleur au renderer
// Internal helper: set renderer draw color
// ============================================================
static inline void wc_set_render_color(SDL_Renderer *r, float rf, float gf, float bf, float af) {
    SDL_BlendMode bm = (af < 0.999f) ? SDL_BLENDMODE_BLEND : SDL_BLENDMODE_NONE;
    SDL_SetRenderDrawBlendMode(r, bm);
    SDL_SetRenderDrawColor(r, (Uint8)(rf * 255.0f), (Uint8)(gf * 255.0f), (Uint8)(bf * 255.0f), (Uint8)(af * 255.0f));
}

// ============================================================
// Namespace ol (OpenLayer compatibility)
// ============================================================
namespace ol {

// ----------------------------------------------------------------
// Vec2D
// ----------------------------------------------------------------
struct Vec2D {
    float x, y;
    Vec2D() : x(0), y(0) {
    }
    Vec2D(float x, float y) : x(x), y(y) {
    }
    Vec2D operator+(const Vec2D &o) const {
        return Vec2D(x + o.x, y + o.y);
    }
    Vec2D operator-(const Vec2D &o) const {
        return Vec2D(x - o.x, y - o.y);
    }
    Vec2D operator*(float s) const {
        return Vec2D(x * s, y * s);
    }
};

// ----------------------------------------------------------------
// Rgba
// ----------------------------------------------------------------
struct Rgba {
    float r, g, b, a;

    Rgba() : r(0), g(0), b(0), a(1) {
    }
    Rgba(float r, float g, float b) : r(r), g(g), b(b), a(1) {
    }
    Rgba(float r, float g, float b, float a) : r(r), g(g), b(b), a(a) {
    }

    Rgba WithAlpha(float alpha) const {
        return Rgba(r, g, b, alpha);
    }

    SDL_Color toSDL() const {
        return {(Uint8)(r * 255.0f), (Uint8)(g * 255.0f), (Uint8)(b * 255.0f), (Uint8)(a * 255.0f)};
    }

    // Constantes statiques / Static color constants
    static Rgba WHITE;
    static Rgba BLACK;
    static Rgba RED;
    static Rgba GREEN;
    static Rgba BLUE;
    static Rgba YELLOW;
};

// Definitions (une seule TU definit WC_SKIP_GLOBALS=non-defini, les autres sautent)
#ifndef WC_SKIP_GLOBALS
Rgba Rgba::WHITE(1.0f, 1.0f, 1.0f, 1.0f);
Rgba Rgba::BLACK(0.0f, 0.0f, 0.0f, 1.0f);
Rgba Rgba::RED(1.0f, 0.0f, 0.0f, 1.0f);
Rgba Rgba::GREEN(0.0f, 1.0f, 0.0f, 1.0f);
Rgba Rgba::BLUE(0.0f, 0.0f, 1.0f, 1.0f);
Rgba Rgba::YELLOW(1.0f, 1.0f, 0.0f, 1.0f);
#endif

// ----------------------------------------------------------------
// Helper interne namespace
// ----------------------------------------------------------------
static inline void _setcolor(const Rgba &c) {
    wc_set_render_color(wc_sdl_renderer, c.r, c.g, c.b, c.a);
}

// ----------------------------------------------------------------
// Point (Allegro/OpenLayer) — peut dessiner sur le renderer
// ----------------------------------------------------------------
class Point {
  public:
    float x, y;
    Point() : x(0), y(0) {
    }
    Point(int x, int y) : x((float)x), y((float)y) {
    }
    Point(float x, float y) : x(x), y(y) {
    }
    Point(Vec2D v) : x(v.x), y(v.y) {
    }

    void Draw(const Rgba &color) const {
        if (!wc_sdl_renderer)
            return;
        _setcolor(color);
        SDL_RenderDrawPoint(wc_sdl_renderer, (int)x, (int)y);
    }

    // OpenLayer fast drawing stubs
    static void StartFastDrawing() {
    }
    static void FinishFastDrawing() {
    }
    void DrawFast(const Rgba &color) const {
        Draw(color);
    }
};

// ----------------------------------------------------------------
// Line
// ----------------------------------------------------------------
class Line {
    Vec2D start, end;
    float thickness;

  public:
    Line(Vec2D s, Vec2D e, float t = 1.0f) : start(s), end(e), thickness(t) {
    }
    // Constructeur avec coordonnees separees / Constructor with separate int coords
    Line(int x1, int y1, int x2, int y2, float t = 1.0f)
        : start((float)x1, (float)y1), end((float)x2, (float)y2), thickness(t) {
    }
    Line(float x1, float y1, float x2, float y2, float t = 1.0f) : start(x1, y1), end(x2, y2), thickness(t) {
    }

    void SetLineWidth(float w) {
        thickness = w;
    }
    void DrawOutline(const Rgba &color) const {
        Draw(color);
    }
    // Draw avec deux couleurs (degradé) — utilise la premiere couleur
    void Draw(const Rgba &c1, const Rgba & /*c2*/) const {
        Draw(c1);
    }

    void Draw(const Rgba &color) const {
        if (!wc_sdl_renderer)
            return;
        _setcolor(color);
        int t = (int)(thickness + 0.5f);
        if (t <= 1) {
            SDL_RenderDrawLine(wc_sdl_renderer, (int)start.x, (int)start.y, (int)end.x, (int)end.y);
        } else {
            // Lignes paralleles decalees perpendiculairement
            float dx = end.x - start.x;
            float dy = end.y - start.y;
            float len = sqrtf(dx * dx + dy * dy);
            if (len > 0.0f) {
                float nx = -dy / len;
                float ny = dx / len;
                for (int i = -(t / 2); i <= t / 2; i++) {
                    SDL_RenderDrawLine(wc_sdl_renderer, (int)(start.x + nx * i), (int)(start.y + ny * i),
                                       (int)(end.x + nx * i), (int)(end.y + ny * i));
                }
            }
        }
    }
};

// ----------------------------------------------------------------
// Rectangle
// ----------------------------------------------------------------
class Rectangle {
    Vec2D pos, size;
    float roundness; // rayon des coins / corner radius
    float line_width;

  public:
    Rectangle(Vec2D pos, Vec2D size) : pos(pos), size(size), roundness(0.0f), line_width(1.0f) {
    }

    void SetRoundness(float r) {
        roundness = r;
    }
    void SetLineWidth(float w) {
        line_width = w;
    }
    void MoveTo(float x, float y) {
        pos.x = x;
        pos.y = y;
    }
    void MoveTo(Vec2D p) {
        pos = p;
    }

    // OpenLayer: Draw() = filled, DrawOutline() = contour seulement
    void Draw(const Rgba &color) const {
        if (!wc_sdl_renderer)
            return;
        _setcolor(color);
        int x0 = (int)pos.x, y0 = (int)pos.y;
        int w = (int)size.x, h = (int)size.y;
        int r = (int)(roundness + 0.5f);
        if (r <= 0) {
            SDL_Rect rect = {x0, y0, w, h};
            SDL_RenderFillRect(wc_sdl_renderer, &rect);
            return;
        }
        r = std::min(r, std::min(w / 2, h / 2));
        // Corps principal : 3 rects (haut, milieu, bas) sans les coins
        SDL_Rect body[3] = {
            {x0 + r, y0, w - 2 * r, r},
            {x0, y0 + r, w, h - 2 * r},
            {x0 + r, y0 + h - r, w - 2 * r, r},
        };
        SDL_RenderFillRects(wc_sdl_renderer, body, 3);
        // Coins arrondis : seulement r scanlines par coin, batchees en FillRects
        SDL_Rect corners[256];
        int nc = 0;
        for (int i = 0; i < r && nc + 4 <= 256; i++) {
            int dy = r - i;
            int dx = (int)(sqrtf((float)(r * r - dy * dy)) + 0.5f);
            if (dx <= 0)
                continue;
            int rowT = y0 + i;
            int rowB = y0 + h - 1 - i;
            corners[nc++] = {x0 + r - dx, rowT, dx, 1}; // haut-gauche
            corners[nc++] = {x0 + w - r, rowT, dx, 1};  // haut-droit
            corners[nc++] = {x0 + r - dx, rowB, dx, 1}; // bas-gauche
            corners[nc++] = {x0 + w - r, rowB, dx, 1};  // bas-droit
        }
        if (nc > 0)
            SDL_RenderFillRects(wc_sdl_renderer, corners, nc);
    }

    void Fill(const Rgba &color) const {
        Draw(color);
    }

    void DrawOutline(const Rgba &color) const {
        if (!wc_sdl_renderer)
            return;
        _setcolor(color);
        int x0 = (int)pos.x, y0 = (int)pos.y;
        int w = (int)size.x, h = (int)size.y;
        if (w <= 0 || h <= 0)
            return;
        int r = (int)(roundness + 0.5f);
        int lw = std::max(1, (int)(line_width + 0.5f));
        r = std::min(r, std::min(w / 2, h / 2));
        int ri = std::max(0, r - lw); // rayon intérieur du coin arrondi

        // Bordure = bande pleine entre arc extérieur (r) et intérieur (ri)
        // Pas d'arcs concentriques : une seule passe, sans trous aux coins
        SDL_Rect all[256];
        int n = 0;

        // Bandes droites (haut, bas, gauche, droite)
        int lwh = std::min(lw, h), lww = std::min(lw, w);
        if (w > 2 * r) {
            all[n++] = {x0 + r, y0, w - 2 * r, lwh};           // haut
            all[n++] = {x0 + r, y0 + h - lwh, w - 2 * r, lwh}; // bas
        }
        if (h > 2 * r) {
            all[n++] = {x0, y0 + r, lww, h - 2 * r};           // gauche
            all[n++] = {x0 + w - lww, y0 + r, lww, h - 2 * r}; // droite
        }

        // Bandes de coins : chaque ligne = bande entre arc extérieur et intérieur
        for (int i = 1; i < r && n + 4 <= 256; i++) {
            int dy = r - i;
            int odx = (int)(sqrtf((float)(r * r - dy * dy)) + 0.5f);
            int idx = (ri > 0 && dy <= ri) ? (int)(sqrtf((float)(ri * ri - dy * dy)) + 0.5f) : 0;
            int bw = odx - idx;
            if (bw <= 0)
                continue;
            int rowT = y0 + i;
            int rowB = y0 + h - 1 - i;
            all[n++] = {x0 + r - odx, rowT, bw, 1};     // coin gauche haut
            all[n++] = {x0 + w - r + idx, rowT, bw, 1}; // coin droit haut
            if (rowB != rowT) {
                all[n++] = {x0 + r - odx, rowB, bw, 1};     // coin gauche bas
                all[n++] = {x0 + w - r + idx, rowB, bw, 1}; // coin droit bas
            }
        }

        if (n > 0)
            SDL_RenderFillRects(wc_sdl_renderer, all, n);
    }
};

// Alias OpenLayer / OpenLayer alias
typedef Rectangle Rect;

// ----------------------------------------------------------------
// Circle
// ----------------------------------------------------------------
class Circle {
    Vec2D center;
    float radius;
    float line_width;
    float rot_angle;// rotation courante en radians (RotateBy) / current rotation in radians

  public:
    Circle(Vec2D c, float r) : center(c), radius(r), line_width(1.0f), rot_angle(0.0f) {
    }
    Circle(int cx, int cy, int r) : center((float)cx, (float)cy), radius((float)r), line_width(1.0f), rot_angle(0.0f) {
    }
    // 4 args (cx, cy, inner_r, outer_r) — dessine anneau / draws ring
    Circle(int cx, int cy, double r1, double r2)
        : center((float)cx, (float)cy), radius((float)r2), line_width((float)(r2 - r1)), rot_angle(0.0f) {
    }

    void SetLineWidth(float w) {
        line_width = w;
    }
    void SetRadius(float r) {
        radius = r;
    }
    void MoveTo(float x, float y) {
        center.x = x;
        center.y = y;
    }
    void MoveTo(Vec2D p) {
        center = p;
    }
    void MoveBy(float dx, float dy) {
        center.x += dx;
        center.y += dy;
    }
    void MoveBy(Vec2D d) {
        center.x += d.x;
        center.y += d.y;
    }
    void RotateBy(float da) {
        rot_angle += da;
    }
    // Coeur : part de tarte pleine (fill) + contour optionnel (outline).
    // start = angle de depart, sweep = amplitude (radians), + rotation courante.
    // Core: filled pie slice (fill) + optional outline. start = start angle,
    // sweep = extent (radians), plus current rotation.
    void _drawSliceCore(const Rgba &fill, const Rgba *outline, double start, double sweep) const {
        if (!wc_sdl_renderer)
            return;
        double a0 = start + rot_angle;
        // segments de l'arc / arc segments
        int n = (int)(fabsf((float)sweep) * (radius + 2.0f) / 4.0f);
        if (n < 6)
            n = 6;
        if (n > 96)
            n = 96;
        float cx = center.x, cy = center.y, r = radius;
        // Sommets du secteur : centre + points d'arc. Y ecran inverse (cy - sin)
        // pour rester coherent avec OpenLayer et le sens de rotation de Poly.
        // Sector vertices: center + arc points. Screen Y flipped (cy - sin).
        SDL_Point vx[100];
        int vn = 0;
        vx[vn].x = (int)cx;
        vx[vn].y = (int)cy;
        vn++;
        for (int i = 0; i <= n; i++) {
            double a = a0 + sweep * (double)i / (double)n;
            vx[vn].x = (int)(cx + r * cosf((float)a));
            vx[vn].y = (int)(cy - r * sinf((float)a));
            vn++;
        }
        // Remplissage scanline du polygone (sans trous) / gapless scanline fill
        _setcolor(fill);
        int ymin = vx[0].y, ymax = vx[0].y;
        for (int i = 1; i < vn; i++) {
            if (vx[i].y < ymin)
                ymin = vx[i].y;
            if (vx[i].y > ymax)
                ymax = vx[i].y;
        }
        for (int y = ymin; y <= ymax; y++) {
            int xs[128];
            int xc = 0;
            for (int i = 0; i < vn; i++) {
                SDL_Point a = vx[i], b = vx[(i + 1) % vn];
                if ((a.y <= y && b.y > y) || (b.y <= y && a.y > y)) {
                    int x = a.x + (int)((long)(y - a.y) * (b.x - a.x) / (b.y - a.y));
                    if (xc < 128)
                        xs[xc++] = x;
                }
            }
            for (int i = 0; i < xc - 1; i++)
                for (int j = i + 1; j < xc; j++)
                    if (xs[j] < xs[i]) {
                        int t = xs[i];
                        xs[i] = xs[j];
                        xs[j] = t;
                    }
            for (int i = 0; i + 1 < xc; i += 2)
                SDL_RenderDrawLine(wc_sdl_renderer, xs[i], y, xs[i + 1], y);
        }
        // Contour = toutes les aretes du polygone (2 rayons + arc)
        // Outline = every polygon edge (2 radii + arc)
        if (outline) {
            _setcolor(*outline);
            for (int i = 0; i < vn; i++) {
                SDL_Point a = vx[i], b = vx[(i + 1) % vn];
                SDL_RenderDrawLine(wc_sdl_renderer, a.x, a.y, b.x, b.y);
            }
        }
    }
    // DrawSlice : plusieurs signatures possibles / multiple possible signatures
    void DrawSlice(const Rgba &color, double a1, double a2) const {
        _drawSliceCore(color, nullptr, a1, a2);
    }
    void DrawSlice(const Rgba &color, int a1, double a2) const {
        _drawSliceCore(color, nullptr, (double)a1, a2);
    }
    void DrawSlice(const Rgba &color, int a1, float a2) const {
        _drawSliceCore(color, nullptr, (double)a1, (double)a2);
    }
    void DrawSlice(const Rgba &c1, const Rgba &c2, int a1, float a2) const {
        _drawSliceCore(c1, &c2, (double)a1, (double)a2);
    }

    // OpenLayer: Draw() = filled circle, DrawOutline() = contour seulement
    void Draw(const Rgba &color) const {
        if (!wc_sdl_renderer)
            return;
        _setcolor(color);
        int cx = (int)center.x, cy = (int)center.y, r = (int)(radius + 0.5f);
        for (int dy = -r; dy <= r; dy++) {
            int dx = (int)sqrtf((float)(r * r - dy * dy));
            SDL_RenderDrawLine(wc_sdl_renderer, cx - dx, cy + dy, cx + dx, cy + dy);
        }
    }

    void Fill(const Rgba &color) const {
        Draw(color);
    }

    void DrawOutline(const Rgba &color) const {
        if (!wc_sdl_renderer)
            return;
        _setcolor(color);
        int cx = (int)center.x, cy = (int)center.y;
        int lw = std::max(1, (int)(line_width + 0.5f));
        int r0 = std::max(1, (int)(radius + 0.5f) - lw / 2);
        int r1 = r0 + lw - 1;
        for (int r = r0; r <= r1; r++) {
            SDL_Point pts[512];
            int n = 0;
            int x = 0, y = r, d = 1 - r;
            while (x <= y && n + 8 <= 512) {
                pts[n++] = {cx + x, cy + y};
                pts[n++] = {cx - x, cy + y};
                pts[n++] = {cx + x, cy - y};
                pts[n++] = {cx - x, cy - y};
                pts[n++] = {cx + y, cy + x};
                pts[n++] = {cx - y, cy + x};
                pts[n++] = {cx + y, cy - x};
                pts[n++] = {cx - y, cy - x};
                if (d < 0) {
                    d += 2 * x + 3;
                } else {
                    d += 2 * (x - y) + 5;
                    y--;
                }
                x++;
            }
            if (n > 0)
                SDL_RenderDrawPoints(wc_sdl_renderer, pts, n);
        }
    }
};

// ----------------------------------------------------------------
// Triangle (polygone 3 cotes sur renderer)
// ----------------------------------------------------------------
class Triangle {
    Vec2D p1, p2, p3;

  public:
    Triangle(Vec2D a, Vec2D b, Vec2D c) : p1(a), p2(b), p3(c) {
    }

    void Draw(const Rgba &color) const {
        if (!wc_sdl_renderer)
            return;
        _setcolor(color);
        SDL_RenderDrawLine(wc_sdl_renderer, (int)p1.x, (int)p1.y, (int)p2.x, (int)p2.y);
        SDL_RenderDrawLine(wc_sdl_renderer, (int)p2.x, (int)p2.y, (int)p3.x, (int)p3.y);
        SDL_RenderDrawLine(wc_sdl_renderer, (int)p3.x, (int)p3.y, (int)p1.x, (int)p1.y);
    }

    void Fill(const Rgba &color) const {
        if (!wc_sdl_renderer)
            return;
        _setcolor(color);
        // Scanline fill
        int x1 = (int)p1.x, y1 = (int)p1.y;
        int x2 = (int)p2.x, y2 = (int)p2.y;
        int x3 = (int)p3.x, y3 = (int)p3.y;
        if (y1 > y2) {
            std::swap(x1, x2);
            std::swap(y1, y2);
        }
        if (y1 > y3) {
            std::swap(x1, x3);
            std::swap(y1, y3);
        }
        if (y2 > y3) {
            std::swap(x2, x3);
            std::swap(y2, y3);
        }
        for (int y = y1; y <= y3; y++) {
            int xa = (y3 != y1) ? x1 + (x3 - x1) * (y - y1) / (y3 - y1) : x1;
            int xb;
            if (y <= y2)
                xb = (y2 != y1) ? x1 + (x2 - x1) * (y - y1) / (y2 - y1) : x1;
            else
                xb = (y3 != y2) ? x2 + (x3 - x2) * (y - y2) / (y3 - y2) : x2;
            if (xa > xb)
                std::swap(xa, xb);
            SDL_RenderDrawLine(wc_sdl_renderer, xa, y, xb, y);
        }
    }
};

// ----------------------------------------------------------------
// Poly (polygone a N sommets / N-vertex polygon)
// ----------------------------------------------------------------
class Poly {
    std::vector<Vec2D> vertices;
    Vec2D pivot;
    float angle;

  public:
    Poly() : angle(0.0f) {
    }
    void AddVertex(float x, float y) {
        vertices.push_back(Vec2D(x, y));
    }
    void AddVertex(Vec2D v) {
        vertices.push_back(v);
    }
    void Add(float x, float y) {
        vertices.push_back(Vec2D(x, y));
    }
    void Add(Vec2D v) {
        vertices.push_back(v);
    }
    void Clear() {
        vertices.clear();
    }
    void SetPivot(Vec2D p) {
        pivot = p;
    }
    void SetPivot(float x, float y) {
        pivot = Vec2D(x, y);
    }
    void RotateBy(float da) {
        angle += da;
    }
    // Applique la rotation courante (radians) autour du pivot / applies current rotation
    Vec2D _xform(const Vec2D &v) const {
        if (angle == 0.0f)
            return v;
        float s = sinf(angle), c = cosf(angle);
        float dx = v.x - pivot.x, dy = v.y - pivot.y;
        return Vec2D(pivot.x + dx * c - dy * s, pivot.y + dx * s + dy * c);
    }
    void DrawOutline(const Rgba &color) const {
        Draw(color);
    }

    void Draw(const Rgba &color) const {
        if (!wc_sdl_renderer || vertices.size() < 2)
            return;
        _setcolor(color);
        for (size_t i = 0; i < vertices.size(); i++) {
            Vec2D a = _xform(vertices[i]);
            Vec2D b = _xform(vertices[(i + 1) % vertices.size()]);
            SDL_RenderDrawLine(wc_sdl_renderer, (int)a.x, (int)a.y, (int)b.x, (int)b.y);
        }
    }

    void Fill(const Rgba &color) const {
        if (!wc_sdl_renderer || vertices.size() < 3)
            return;
        _setcolor(color);
        // Simple fan triangulation from first vertex
        for (size_t i = 1; i + 1 < vertices.size(); i++) {
            // Scanline fill each triangle
            Vec2D v0 = _xform(vertices[0]);
            Vec2D vi = _xform(vertices[i]);
            Vec2D vi1 = _xform(vertices[i + 1]);
            int x1 = (int)v0.x, y1 = (int)v0.y;
            int x2 = (int)vi.x, y2 = (int)vi.y;
            int x3 = (int)vi1.x, y3 = (int)vi1.y;
            if (y1 > y2) {
                std::swap(x1, x2);
                std::swap(y1, y2);
            }
            if (y1 > y3) {
                std::swap(x1, x3);
                std::swap(y1, y3);
            }
            if (y2 > y3) {
                std::swap(x2, x3);
                std::swap(y2, y3);
            }
            for (int y = y1; y <= y3; y++) {
                int xa = (y3 != y1) ? x1 + (x3 - x1) * (y - y1) / (y3 - y1) : x1;
                int xb = (y <= y2) ? ((y2 != y1) ? x1 + (x2 - x1) * (y - y1) / (y2 - y1) : x1)
                                   : ((y3 != y2) ? x2 + (x3 - x2) * (y - y2) / (y3 - y2) : x2);
                if (xa > xb)
                    std::swap(xa, xb);
                SDL_RenderDrawLine(wc_sdl_renderer, xa, y, xb, y);
            }
        }
    }
};

// ----------------------------------------------------------------
// Bitmap (images PNG/JPG → SDL_Texture)
// ----------------------------------------------------------------
class Bitmap {
    SDL_Texture *tex;
    int img_w, img_h;

  public:
    Bitmap() : tex(nullptr), img_w(0), img_h(0) {
    }

    // Constructeur avec dimensions (bitmap vide / empty bitmap)
    Bitmap(int w, int h) : tex(nullptr), img_w(w), img_h(h) {
        if (wc_sdl_renderer) {
            tex = SDL_CreateTexture(wc_sdl_renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, w, h);
        }
    }

    ~Bitmap() {
        if (tex)
            SDL_DestroyTexture(tex);
    }

    bool Load(const char *filename) {
        if (tex) {
            SDL_DestroyTexture(tex);
            tex = nullptr;
        }
        SDL_Surface *surf = IMG_Load(filename);
        if (!surf)
            return false;
        img_w = surf->w;
        img_h = surf->h;
        tex = SDL_CreateTextureFromSurface(wc_sdl_renderer, surf);
        SDL_FreeSurface(surf);
        return (tex != nullptr);
    }

    operator bool() const {
        return tex != nullptr;
    }

    void Draw(float x, float y) const {
        if (!tex || !wc_sdl_renderer)
            return;
        SDL_Rect dst = {(int)x, (int)y, img_w, img_h};
        SDL_RenderCopy(wc_sdl_renderer, tex, nullptr, &dst);
    }

    void Draw(float x, float y, float w, float h) const {
        if (!tex || !wc_sdl_renderer)
            return;
        SDL_Rect dst = {(int)x, (int)y, (int)w, (int)h};
        SDL_RenderCopy(wc_sdl_renderer, tex, nullptr, &dst);
    }

    int Width() const {
        return img_w;
    }
    int Height() const {
        return img_h;
    }

    bool IsValid() const {
        return tex != nullptr;
    }
    void Blit(float x, float y) const {
        Draw(x, y);
    }
    void Blit(float x, float y, float w, float h) const {
        Draw(x, y, w, h);
    }
    void BlitTransformed(float x, float y, float /*angle*/, float sx = 1, float sy = 1) const {
        Draw(x, y, img_w * sx, img_h * sy);
    }
    void BlitTransformed(int x, int y, int /*ax*/, int /*ay*/, double /*angle*/, float sx = 1) const {
        Draw((float)x, (float)y, img_w * sx, img_h * sx);
    }
    void CopyFromScreen() {
    }
    void CopyFromScreen(int /*x*/, int /*y*/) {
    }
    bool Save(const char * /*filename*/) const {
        return false;
    }
};

// ----------------------------------------------------------------
// TextRenderer (polices TTF → SDL2_ttf)
// Cache 4-way set-associatif avec LRU (SDL_GetTicks).
// 2048 slots = 512 sets × 4 ways.
// Sur un miss : on évince le slot le moins récemment utilisé dans le set
// (ou un slot vide en priorité) — élimine les évictions de collision.
// ----------------------------------------------------------------
#define WC_CACHE_WAYS 4
#define WC_CACHE_SETS 512
#define WC_CACHE_SIZE (WC_CACHE_SETS * WC_CACHE_WAYS)
struct WC_CacheSlot {
    TTF_Font *font; // null = slot vide
    Uint32 col;
    char text[128];
    SDL_Texture *tex;
    int w, h, ascent;
    Uint32 last_used; // SDL_GetTicks() au dernier accès (LRU)
};
#ifndef WC_SKIP_GLOBALS
WC_CacheSlot *wc_cache = nullptr;
SDL_mutex *wc_cache_mutex = nullptr;
#else
extern WC_CacheSlot *wc_cache;
extern SDL_mutex *wc_cache_mutex;
#endif

static unsigned wc_cache_hash(TTF_Font *font, Uint32 col, const char *text) {
    unsigned h = (unsigned)(uintptr_t)font * 2654435761u ^ col * 2246822519u;
    for (int i = 0; i < 127 && text[i]; i++)
        h = h * 31 + (unsigned char)text[i];
    return h;
}

static void wc_purge_font(TTF_Font *f) {
    if (!f || !wc_cache)
        return;
    if (wc_cache_mutex)
        SDL_LockMutex(wc_cache_mutex);
    for (int i = 0; i < WC_CACHE_SIZE; i++) {
        if (wc_cache[i].font == f) {
            if (wc_cache[i].tex && SDL_WasInit(SDL_INIT_VIDEO))
                SDL_DestroyTexture(wc_cache[i].tex);
            wc_cache[i].font = nullptr;
            wc_cache[i].tex = nullptr;
        }
    }
    if (wc_cache_mutex)
        SDL_UnlockMutex(wc_cache_mutex);
}

class TextRenderer {
    TTF_Font *font;
    SDL_Color col;
    bool ok;
    int font_ascent; // mis en cache au Load(), pas recalculé à chaque Print
  public:
    TextRenderer() : font(nullptr), ok(false), font_ascent(0) {
        col = {255, 255, 255, 255};
    }

    ~TextRenderer() {
        if (font) {
            wc_purge_font(font);
            TTF_CloseFont(font);
        }
    }

    bool Load(const char *filename, int /*width*/, int height, const Rgba &color) {
        if (font) {
            wc_purge_font(font);
            TTF_CloseFont(font);
            font = nullptr;
        }
        font = TTF_OpenFont(filename, height);
        ok = (font != nullptr);
        col = color.toSDL();
        font_ascent = font ? TTF_FontAscent(font) : 0;
        return ok;
    }

    int TextWidth(const char *text) const {
        if (!font || !text || !text[0]) return 0;
        int w = 0, h = 0;
        TTF_SizeUTF8(font, text, &w, &h);
        return w;
    }

    void Print(const char *text, int x, int y) const {
        if (!font || !text || text[0] == '\0' || !wc_sdl_renderer || !wc_cache)
            return;
        char trunc[128];
        strncpy(trunc, text, 127);
        trunc[127] = '\0';
        Uint32 key_col = (Uint32)col.r | ((Uint32)col.g << 8) | ((Uint32)col.b << 16) | ((Uint32)col.a << 24);
        unsigned set = wc_cache_hash(font, key_col, trunc) % WC_CACHE_SETS;
        WC_CacheSlot *base = wc_cache + set * WC_CACHE_WAYS;

        // Recherche dans les 4 ways du set
        // Pas de mutex : rendu uniquement depuis le thread principal.
        WC_CacheSlot *hit = nullptr;
        for (int w = 0; w < WC_CACHE_WAYS; w++) {
            WC_CacheSlot &s = base[w];
            if (s.font == font && s.col == key_col && s.tex && strncmp(s.text, trunc, 128) == 0) {
                hit = &s;
                break;
            }
        }

        if (!hit) {
            // Miss — trouver slot vide ou LRU dans le set
            WC_CacheSlot *victim = nullptr;
            for (int w = 0; w < WC_CACHE_WAYS; w++) {
                WC_CacheSlot &s = base[w];
                if (!s.font) { victim = &s; break; }
                if (!victim || s.last_used < victim->last_used) victim = &s;
            }
            if (victim->tex) SDL_DestroyTexture(victim->tex);
            victim->tex = nullptr;
            victim->font = nullptr;
            SDL_Surface *surf = TTF_RenderUTF8_Blended(font, trunc, col);
            if (!surf) return;
            SDL_Texture *tex = SDL_CreateTextureFromSurface(wc_sdl_renderer, surf);
            SDL_FreeSurface(surf);
            if (!tex) return;
            victim->font = font;
            victim->col  = key_col;
            strncpy(victim->text, trunc, 128);
            victim->tex  = tex;
            SDL_QueryTexture(tex, nullptr, nullptr, &victim->w, &victim->h);
            victim->ascent    = font_ascent;
            victim->last_used = SDL_GetTicks();
            hit = victim;
        }

        hit->last_used = SDL_GetTicks();
        SDL_Rect dst = {x, y - hit->ascent, hit->w, hit->h};
        SDL_RenderCopy(wc_sdl_renderer, hit->tex, nullptr, &dst);
    }

    // Surcharges / Overloads
    void Print(int value, int x, int y) const {
        char buf[32];
        snprintf(buf, sizeof(buf), "%d", value);
        Print(buf, x, y);
    }
    void Print(float value, int x, int y) const {
        char buf[32];
        snprintf(buf, sizeof(buf), "%.2f", value);
        Print(buf, x, y);
    }
    void Print(const std::string &s, int x, int y) const {
        Print(s.c_str(), x, y);
    }
    // Surcharges avec alignement / alignment overloads (4th arg = align, 5th = max_width)
    void Print(const char *text, int x, int y, int /*align*/) const {
        Print(text, x, y);
    }
    void Print(const std::string &s, int x, int y, int align) const {
        Print(s.c_str(), x, y, align);
    }
    void Print(const char *text, int x, int y, int align, int /*maxw*/) const {
        Print(text, x, y, align);
    }
    // Metrics
    int FirstLineHeight() const {
        return font ? TTF_FontHeight(font) : 0;
    }
    int FirstLineHeight(const std::string &) const {
        return FirstLineHeight();
    }
    int FirstLineWidth(const char *text) const {
        if (!font || !text)
            return 0;
        int w = 0, h = 0;
        TTF_SizeText(font, text, &w, &h);
        return w;
    }
    int FirstLineWidth(const std::string &s) const {
        return FirstLineWidth(s.c_str());
    }

    // Mise a jour de la couleur sans recharger la police
    void SetColor(const Rgba &color) {
        col = color.toSDL();
    }

    operator bool() const {
        return ok;
    }
};

// ----------------------------------------------------------------
// Canvas (rendu frame)
// ----------------------------------------------------------------
namespace Canvas {

inline void Fill(const Rgba &color) {
    if (!wc_sdl_renderer)
        return;
    if (wc_ui_texture) {
        // Phase 6 : tout le rendu va sur la texture persistante.
        // Effacement complet seulement si le fond a changé (circuits, scroller, infos).
        // Sur MOUSEMOTION pur, wc_bg_dirty=false → on réutilise le fond en cache.
        SDL_SetRenderTarget(wc_sdl_renderer, wc_ui_texture);
        if (wc_bg_dirty) {
            wc_set_render_color(wc_sdl_renderer, color.r, color.g, color.b, color.a);
            SDL_RenderClear(wc_sdl_renderer);
        }
    } else {
        wc_set_render_color(wc_sdl_renderer, color.r, color.g, color.b, color.a);
        SDL_RenderClear(wc_sdl_renderer);
    }
    wc_frame_was_updated = true;
}

inline void Refresh() {
    if (!wc_sdl_renderer)
        return;

    // Présente exactement 1× après chaque draw — le double buffering SDL2 fait que
    // le 2e present sans redraw affiche l'ANCIEN back buffer → clignotement visible.
    // On tire profit du SDL_WaitEventTimeout en mode idle pour rester réactif sans GPU.
    static int presents_since_draw = 1;
    if (wc_frame_was_updated) {
        presents_since_draw = 0;
        wc_frame_was_updated = false;
    }

    if (presents_since_draw < 1) {
        // Mode actif : pompe les événements sans bloquer, puis présente
        WC_FDEBUG("Refresh-before-process_events");
        wc_process_events();
        WC_FDEBUG("Refresh-before-RenderPresent");
        // Phase 6 : blit la texture persistante vers le back-buffer écran.
        // Résout le problème de double-buffering SDL (le 2e present sans redraw
        // afficherait l'ancien back-buffer ; ici la texture est toujours à jour).
        if (wc_ui_texture) {
            SDL_SetRenderTarget(wc_sdl_renderer, nullptr);
            SDL_RenderCopy(wc_sdl_renderer, wc_ui_texture, nullptr, nullptr);
        }
        SDL_RenderPresent(wc_sdl_renderer);
        WC_FDEBUG("Refresh-after-RenderPresent");
        presents_since_draw++;

        // Cap FPS à 3 niveaux :
        //   utilisateur actif (< 500ms) → 30fps (33ms) — 20fps (50ms) sur Pi3
        //   automation seule            → 25fps (40ms) — 10fps (100ms) sur Pi3
        //   idle (aucune des deux)      → 100ms — 250ms sur Pi3
        static Uint32 last_frame = 0;
        Uint32 now = SDL_GetTicks();
        Uint32 cap_ms;
#ifdef WC_PI3
        if (now - wc_last_input_ms < 500)
            cap_ms = 50;  // 20fps sur Pi3
        else if (wc_automation_active)
            cap_ms = 100; // 10fps sur Pi3
        else
            cap_ms = 250;
#else
        if (now - wc_last_input_ms < 500)
            cap_ms = 33; // ~30fps actif (Phase 6a : draw~10ms → cap désormais effectif)
        else if (wc_automation_active)
            cap_ms = 40;
        else
            cap_ms = 100;
#endif
        wc_automation_active = false;
        Uint32 elapsed = now - last_frame;
        // Remplace SDL_Delay par SDL_WaitEventTimeout : réveille immédiatement sur
        // tout événement (clavier, souris, MIDI) au lieu de dormir aveuglément.
        // Garantit une réactivité clavier < 1ms quelle que soit la charge MIDI.
        if (elapsed < cap_ms) {
            SDL_Event wake_e;
            if (SDL_WaitEventTimeout(&wake_e, cap_ms - elapsed)) {
                wc_handle_event(wake_e);
                if (wake_e.type != SDL_MOUSEBUTTONDOWN)
                    wc_process_events(); // vide la queue restante sans bloquer
            }
        }
        last_frame = SDL_GetTicks();
    } else {
        // Mode stable : attend un événement ou 100ms max.
        // Si c'est MOUSEBUTTONDOWN : traiter UN SEUL event pour que la boucle principale
        // voie mouse_button==1 avant MOUSEBUTTONUP (sinon le clic serait manqué).
        // Si c'est un autre event (MOUSEMOTION, etc.) : vider la queue jusqu'au prochain
        // MOUSEBUTTONDOWN pour qu'un KEYDOWN en attente ne soit pas retardé d'une itération.
        // wc_blink_needed : posé par ticker() quand une fenêtre popup (hors séquenceur)
        // est ouverte ou que F-Shift/F-Ctrl est actif. Réduit le timeout à 40ms (25fps)
        // et force wc_dirty=true sur timeout pour que les éléments alpha_blinker
        // continuent d'animer sans USEREVENT ni wc_request_refresh().
        SDL_Event e;
        // 40ms (25fps) si popup/blinker actif, 1000ms sinon :
        // garantit horloge et led artnet visibles même souris immobile.
        // Sur Pi3 : 100ms / 1000ms pour réduire la charge.
#ifdef WC_PI3
        Uint32 wait_ms = wc_blink_needed ? 100u : 1000u;
#else
        Uint32 wait_ms = wc_blink_needed ? 40u : 1000u;
#endif
        bool _got_event = SDL_WaitEventTimeout(&e, wait_ms);
        if (!_got_event) {
            wc_dirty = true;
            wc_bg_dirty = true;
            wc_win_dirty = true;
        }
        if (_got_event) {
            wc_handle_event(e);
            if (e.type != SDL_MOUSEBUTTONDOWN) {
                SDL_Event e2;
                while (SDL_PollEvent(&e2)) {
                    wc_handle_event(e2);
                    if (e2.type == SDL_MOUSEBUTTONDOWN)
                        break;
                }
            }
        }
    }
}

// Clipping region / Zone de clip
inline void SetClipping(int x, int y, int w, int h) {
    SDL_Rect clip = {x, y, w, h};
    SDL_RenderSetClipRect(wc_sdl_renderer, &clip);
}
inline void SetClipping(Vec2D pos, Vec2D size) {
    SDL_Rect clip = {(int)pos.x, (int)pos.y, (int)size.x, (int)size.y};
    SDL_RenderSetClipRect(wc_sdl_renderer, &clip);
}
inline void SetClipping(Rectangle r) {
    // Rectangle type not fully visible here, use SDL directly
    (void)r; // handled by Vec2D overload at call site
}
inline void DisableClipping() {
    SDL_RenderSetClipRect(wc_sdl_renderer, nullptr);
}

} // namespace Canvas

// ----------------------------------------------------------------
// Utilitaires OpenLayer / OpenLayer utilities
// ----------------------------------------------------------------

// ToString : equivalent std::to_string
template <typename T> inline std::string ToString(T value) {
    return std::to_string(value);
}
inline std::string ToString(float value) {
    char buf[32];
    snprintf(buf, sizeof(buf), "%.4g", value);
    return std::string(buf);
}

// ----------------------------------------------------------------
// Settings
// ----------------------------------------------------------------
namespace Settings {

inline void SetWindowBorder(bool enabled) {
    if (wc_sdl_window)
        SDL_SetWindowBordered(wc_sdl_window, enabled ? SDL_TRUE : SDL_FALSE);
}

inline void SetAntialiasing(bool) {
    // SDL2 n'a pas d'antialiasing global / SDL2 has no global antialiasing switch
    // SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, enabled ? "1" : "0");
}

} // namespace Settings

// ----------------------------------------------------------------
// Setup (initialisation fenetre et input)
// Setup (window and input initialization)
// ----------------------------------------------------------------
namespace Setup {

inline void SetupProgram(int /*flags*/) {
    // app_id Wayland : labwc associe la fenêtre à whitecat.desktop (et donc à
    // l'icône installée). À définir AVANT SDL_Init. Sans effet sous Windows.
    SDL_SetHint(SDL_HINT_APP_ID, "whitecat");
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_AUDIO) != 0) {
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "WhiteCat", SDL_GetError(), nullptr);
        exit(-1);
    }
    if (TTF_Init() != 0) {
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "WhiteCat - TTF", TTF_GetError(), nullptr);
        exit(-1);
    }
    int img_flags = IMG_INIT_PNG | IMG_INIT_JPG;
    if ((IMG_Init(img_flags) & img_flags) != img_flags) {
        // Non fatal : certains formats peuvent manquer
    }
}

inline void SetupScreen(int w, int h, int mode, int /*color_depth*/) {
    SDL_SetHint(SDL_HINT_RENDER_DRIVER, "opengl"); // moins de travail DWM qu'avec D3D11

    // [resize] fenetre redimensionnable a la souris. L'UI garde ses coordonnees fixes (largeur_ecran)
    // -> agrandir donne "plus de vide" (fond), retrecir rogne l'UI. La texture est recreee a la taille
    // fenetre (WINDOWEVENT_RESIZED) et le fond est efface en plein (wc_bg_dirty) -> zone vide propre.
    Uint32 flags = SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE;
    if (mode == FULLSCREEN)
        flags |= SDL_WINDOW_FULLSCREEN_DESKTOP;

    wc_sdl_window = SDL_CreateWindow("WhiteCat", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, w, h, flags);
    if (!wc_sdl_window) {
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "WhiteCat", SDL_GetError(), nullptr);
        exit(-1);
    }

    // Icône de la fenêtre (effective sous X11 et Windows ; sous Wayland l'icône
    // vient du .desktop via l'app_id). gfx/logo.png = tête de chat (RGBA).
    {
        SDL_Surface *_icon = IMG_Load("gfx/logo.png");
        if (_icon) {
            SDL_SetWindowIcon(wc_sdl_window, _icon);
            SDL_FreeSurface(_icon);
        }
    }

    SCREEN_W = w;
    SCREEN_H = h;

    // Try hardware + vsync first
    wc_sdl_renderer = SDL_CreateRenderer(wc_sdl_window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (!wc_sdl_renderer) {
        // Try hardware without vsync (vsync enforced manually in Refresh)
        wc_sdl_renderer = SDL_CreateRenderer(wc_sdl_window, -1, SDL_RENDERER_ACCELERATED);
    }
    if (!wc_sdl_renderer) {
        // Last resort: software renderer
        wc_sdl_renderer = SDL_CreateRenderer(wc_sdl_window, -1, SDL_RENDERER_SOFTWARE);
    }
    if (!wc_sdl_renderer) {
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "WhiteCat", SDL_GetError(), nullptr);
        exit(-1);
    }

    SDL_SetRenderDrawBlendMode(wc_sdl_renderer, SDL_BLENDMODE_BLEND);

    // Phase 6 : texture persistante plein-écran pour les dirty-rects.
    wc_ui_texture = SDL_CreateTexture(wc_sdl_renderer, SDL_PIXELFORMAT_ARGB8888,
                                      SDL_TEXTUREACCESS_TARGET, w, h);

    wc_mouse_range_x2 = w - 1;
    wc_mouse_range_y2 = h - 1;
}

} // namespace Setup

} // namespace ol

// Importer les noms ol dans le namespace global (remplace "using namespace ol")
// Pas de "using namespace ol" ici : le code existant a deja "using namespace ol;"
// Pas besoin de re-exporter.

#endif // GRAPHICS_BACKEND_H
