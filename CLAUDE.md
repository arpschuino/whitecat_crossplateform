# WhiteCat Lighting Board - Context for Claude Code
## Project Overview
WhiteCat is an open-source stage lighting console (console d'éclairage scénique) originally developed by Christoph Guillermet (2009-2016). Now maintained and modernized by Jacques Bouault - arpschuino.fr (2026).
- ~106 000 lines of C++ code
- Single-file architecture (all .cpp included via #include in main)
- Windows only for now, but targeting cross-platform
## Build System
- Compiler: GCC 5.1.0 (MinGW portable dans `tools/MinGW/` ou détecté automatiquement)
- Build script: `build.bat` (chemins 100% relatifs via `%~dp0`, portable)
- Output: `whitecatbuild/build/white_cat_for_mingw/Whitecat_Crossplatform.exe`
- Libraries: `whitecatlib/` — dans le repo, synchronisé via Nextcloud
- Build depuis VSCode : **Ctrl+Shift+B**
## Completed Work
- ✅ Compiled with GCC 5.1.0 (was GCC 4.8.1)
- ✅ Recompiled OpenLayer from sources
- ✅ Fixed al_findnext crashes (replaced with Win32 FindFirstFile/FindNextFile)
- ✅ Fixed mondirectory initialization (GetModuleFileName)
- ✅ Fixed sprintf backslash bug in Load_Show()
- ✅ Migrated from MidiShare to RtMidi (midi_backend.h)
- ✅ Migrated from Code::Blocks to VSCode
- ✅ SDL2 migration complete (Allegro + OpenLayer + Audiere remplacés)
- ✅ Process terminates cleanly after quit
- ✅ Build system fully portable (relative paths, MinGW in tools/)
- ✅ whitecatlib moved into repo (Nextcloud-synced)
- ✅ Phase 4 TU extraction (6 TUs extraits, PCH Makefile, WC_SKIP_GLOBALS pattern)
- ✅ Ticker intelligent — cap 3 niveaux (60/25/idle fps), détection LFO/chasers/GO/dampers
- ✅ wc_cache/wc_cache_mutex globaux (WC_SKIP_GLOBALS) — rendu texte stable dans tous les TUs
- ✅ Phase 6 dirty rects — wc_ui_texture persistante, 33%→18% CPU
- ✅ Phase 7A — abstraction Win32 (wc_platform.h, network.cpp, CFG_screen.cpp)
- ✅ Phase 7B — Makefile.linux (GCC, SDL2, ALSA, libftdi1) — pas encore testé
- ✅ Phase 7C — POSIX porting fichiers (opendir/readdir dans core.cpp, saves_menu.cpp, plot_core9.cpp ; wc_fopen_utf8 #ifdef _WIN32)
- ✅ Fix thru/Tab — range endpoint incorrect en mode vues (ClassicalChannelView=0) dans keyboard_functions2.cpp
- ✅ Phase 7D — ioctlsocket → ioctl POSIX dans wc_platform.h
- ✅ Phase 7E — backend DMX POSIX sans FTD2XX (dmx_backend_linux.h : Enttec Open pthread + Enttec Pro serial)
- ✅ Phase 7F — OpenCV guards #ifdef _WIN32 + stubs Linux/macOS (video_tracking_core.cpp/.h)
## Current Architecture
- `Src/midi_backend.h` — MIDI abstraction layer (RtMidi)
- `Src/midi_CORE.cpp` — MIDI init/quit using midi_backend.h
- `Src/midi_13.cpp` — MIDI processing, now uses process_midi_input()
- `Src/MAIN.cpp` — Main loop
## Dependencies
| Library | Version | Status |
|---------|---------|--------|
| SDL2 + SDL2_mixer + SDL2_ttf + SDL2_image | 2.x | ✅ Active |
| RtMidi | latest | ✅ Active |
| OpenCV | 2.4.8 | ⚠️ Windows only — à wrapper #ifdef |
| FTD2XX | FTDI | ⚠️ Windows only — à remplacer libftdi1 (Linux) |
| libftdi1 | — | 🔜 Linux/macOS (Makefile.linux prêt) |
| MidiShare | 1.91 | ❌ Removed |
| Allegro / OpenLayer / Audiere | — | ❌ Removed |
## Next Steps — Phase 7 portage Linux/Pi/macOS (branche 0.9.1)
Reprendre en **Phase 7G** :
- **7D** ✅ `wc_platform.h` — `ioctlsocket` → `ioctl` POSIX + `sys/ioctl.h`
- **7E** ✅ `Src/dmx_functions.cpp` + `Src/dmx_backend_linux.h` — backend POSIX sans FTD2XX
  (Enttec Open : serial 250000 baud + pthread ; Enttec Pro : serial 57600 baud ; Sunlite : stubs)
- **7F** ✅ OpenCV — `#ifdef _WIN32` autour de tout le tracking vidéo ; stubs Linux/macOS
- **7G** ← PROCHAINE : Test compilation Linux x86 (WSL2/Ubuntu) — chercher les erreurs restantes
- **7H** Test Raspberry Pi 3 (compilation native, VideoCore IV)
- **7D-réseau** `Src/network_MAC_adress_3.cpp` — wrapper `#ifdef _WIN32` autour de `Iphlpapi.h` (à faire si 7G révèle des erreurs de link)

## Key Context — Cross-platform
- `Src/wc_platform.h` — abstraction OS : headers POSIX, types Winsock compat, `wc_get_exe_dir()`, `wc_get_temp_dir()`, `WC_DIRSEP`/`WC_DIRSEP_C`
- `ClassicalChannelView` : 0 = mode vues (thru filtre par vues actives), 1 = mode classique
- `wc_key_queue` est `static` dans graphics_backend.h → keyboard_routines2.cpp doit rester `#include` dans MAIN.cpp (jamais TU séparée)
- **WC_SKIP_GLOBALS** : tout nouveau global dans graphics_backend.h doit utiliser ce patron (extern dans TUs, définition dans MAIN.cpp) — sinon symbole dupliqué silencieux
## Known Issues
- CPU idle : 0,4–0,8 % (ticker intelligent en place — cap 3 niveaux 60/25/100 ms)
- **WC_SKIP_GLOBALS** : tout nouveau global dans `graphics_backend.h` doit suivre ce patron — sinon symbole dupliqué par TU (bug silencieux, difficile à diagnostiquer)
- **PCH** : si `graphics_backend.h` est modifié, `make clean pch` ou supprimer manuellement `Src/wc_tus.h.gch` avant rebuild
- **Warnings cachés** : `-w` dans `build.bat` (ligne 44) supprime tous les warnings GCC — à retirer et trier quand le code sera plus propre
- **Dette technique** : architecture single-file + indentation irrégulière + pas de séparation .h/.cpp — refonte long terme prévue
## État des fenêtres (SDL2 migration)
- ✅ **W_ECHO** — réactivé
- ✅ **W_TRACKINGVIDEO** — réactivé
- ✅ **W_DRAW** — réactivé
- ✅ **W_PLOT** — réactivé
- ✅ **W_AUDIO** — réactivé
- 🗑 **W_MOVER** — définitivement retiré
- 🗑 **W_iCAT** — définitivement retiré (code supprimé)
- 🗑 **W_MY_WINDOW** — définitivement retiré
- ❌ **Restauration état fenêtres** — `window_opened[]` remis à 0 après Load_Show dans MAIN (ligne ~987). À réactiver.
## Important Files
- `Src/whitecat.h` — global variables and declarations
- `Src/CORE_6.cpp` — core functions
- `Src/save_show_13.cpp` — save/load show
- `Src/midi_backend.h` — MIDI abstraction layer
- `build.bat` — build script
- `.vscode/tasks.json` — VSCode build task
- `.vscode/launch.json` — VSCode debug config