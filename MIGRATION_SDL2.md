# Migration SDL2 — WhiteCat 0.8.6.3
*Session du 2026-03-28*

## Objectif
Remplacer Allegro 4 + OpenLayer + Audiere par SDL2 + SDL2_ttf + SDL2_image + SDL2_mixer.

## Libs SDL2 installées
Dossier : `C:\whitecat_crossplateform\whitecatlib\lib\windows\sdl2\`
- SDL2 2.30.8 (i686-w64-mingw32 — 32-bit MinGW)
- SDL2_ttf 2.24.0
- SDL2_image 2.8.8
- SDL2_mixer 2.8.1

## Fichiers créés

### `Src/graphics_backend.h`
Remplace `allegro.h` + `winalleg.h` + `OpenLayer.hpp`.

Implémente :
- `namespace ol::Vec2D` — vecteur 2D (float x, y)
- `namespace ol::Rgba` — couleur RGBA float 0.0-1.0 + `.WithAlpha()`
- `namespace ol::Canvas::Fill(Rgba)` / `Canvas::Refresh()` — rendu frame SDL2
- `namespace ol::Line(Vec2D,Vec2D,thickness).Draw(Rgba)` — ligne avec épaisseur
- `namespace ol::Rectangle(Vec2D,Vec2D).Draw/Fill(Rgba)` — rectangle
- `namespace ol::Circle(Vec2D,radius).Draw/Fill(Rgba)` — cercle Bresenham
- `namespace ol::Triangle(Vec2D,Vec2D,Vec2D).Draw/Fill(Rgba)` — triangle scanline
- `namespace ol::Bitmap` — chargement PNG/JPG via SDL2_image
- `namespace ol::TextRenderer` — polices TTF via SDL2_ttf (Load/Print)
- `namespace ol::Setup::SetupProgram/SetupScreen` — init SDL2 window+renderer
- `namespace ol::Settings::SetWindowBorder/SetAntialiasing` — config fenêtre
- `typedef SDL_Surface BITMAP` + `create_bitmap/clear_bitmap/destroy_bitmap/blit`
- `triangle(BITMAP*, ...)` — scanline fill sur SDL_Surface (pour trichro buffer)
- `install_int_ex(func, ticks)` → `SDL_AddTimer` (thread séparé)
- `remove_int/remove_timer` — nettoyage timers
- `BPS_TO_TIMER/BPM_TO_TIMER/SECS_TO_TIMER/MSEC_TO_TIMER` — macros de conversion
- `LOCK_FUNCTION/LOCK_VARIABLE/END_OF_FUNCTION/END_OF_MAIN` — no-ops
- `mouse_callback`, `mouse_x/y`, `set_mouse_range` — souris compatible Allegro
- `keypressed()`, `readkey()`, `key_shifts`, `KB_CTRL_FLAG` — clavier compatible
- `rest(ms)` → `SDL_Delay`, `allegro_message()` → `SDL_ShowSimpleMessageBox`
- `win_get_window()` → HWND via `SDL_GetWindowWMInfo`
- Stubs : `install_joystick`, `poll_joystick`, `jpgalleg_init`, etc.
- `wc_process_events()` — pompe SDL_PollEvent, appelée dans `Canvas::Refresh()`

### `Src/audio_backend.h`
Remplace `audiere.h` (namespace `audiere`).

Implémente :
- `class WCAudioPlayer` — wraps Mix_Chunk + canal SDL2_mixer
  - `play/stop/isPlaying/setVolume/getVolume`
  - `setRepeat` (looping)
  - `setPosition/getPosition` (ms, estimé via SDL_GetTicks)
  - `getLength` (calculé depuis chunk->alen)
  - `setPan/getPan` via `Mix_SetPanning`
  - `getPitchShift` → retourne 1.0 (non supporté SDL2_mixer)
- `class WCAudioDevice` — wraps Mix_OpenAudio, 4 canaux dédiés
- `typedef WCAudioPlayer* OutputStreamPtr`
- `typedef WCAudioDevice* AudioDevicePtr`
- `OpenDevice(0)` — ouvre le device audio SDL2_mixer
- `OpenSound(device, filename, preloaded)` — charge WAV/OGG/MP3/FLAC

## Fichiers modifiés

### `Src/MAIN_janv_2014_opensource_kbd.cpp`
Remplacé :
```cpp
#include <allegro.h>
#include <winalleg.h>
#include <OpenLayer.hpp>
#include <audiere.h>
#include <loadpng.h>
#include <jpgalleg.h>
```
Par :
```cpp
#include "graphics_backend.h"
#include "audio_backend.h"
```

### `build.bat`
- Supprimé : `-I` et `-L` Allegro/OpenLayer/Audiere
- Ajouté : `-I%SDL2%\include`, `-L%SDL2%\lib`
- Ajouté : `-lSDL2 -lSDL2main -lSDL2_ttf -lSDL2_image -lSDL2_mixer`
- Reformatté avec `^` pour la lisibilité

## Travail restant

### Fichiers encore à migrer (incluent encore Allegro directement)
- `Src/keyboard_routines2.cpp` — `#include <allegro.h>`
- `Src/channels_9_core.cpp` — `#include <allegro.h>`
- `Src/tracker_main14nov.cpp` — `#include <allegro.h>` + `#include <OpenLayer.hpp>`

### Erreurs de compilation attendues
- Conflits de types entre Allegro et SDL2 dans les fichiers ci-dessus
- Fonctions Allegro non wrappées (à identifier au fur et à mesure)
- `SCREEN_W`/`SCREEN_H` possiblement redéfinis par Allegro dans les autres fichiers

### DLLs à copier dans le dossier de l'exe (runtime)
```
SDL2.dll, SDL2_ttf.dll, SDL2_image.dll, SDL2_mixer.dll
→ whitecatbuild/build/white_cat_for_mingw/
```

### Limitations connues audio_backend.h
- `setPosition(pos > 0)` non supporté (SDL2_mixer sans accès PCM direct)
- `getPitchShift` toujours 1.0
- Formats audio : selon build SDL2_mixer (WAV natif, OGG/MP3/FLAC optionnels)
