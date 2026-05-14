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
## Current Architecture
- `Src/midi_backend.h` — MIDI abstraction layer (RtMidi)
- `Src/midi_CORE.cpp` — MIDI init/quit using midi_backend.h
- `Src/midi_13.cpp` — MIDI processing, now uses process_midi_input()
- `Src/MAIN.cpp` — Main loop
## Dependencies
| Library | Version | Status |
|---------|---------|--------|
| Allegro | 4.4.2 | TO REPLACE with SDL2 |
| OpenLayer | 2.1 | TO REPLACE with SDL2 |
| Audiere | 1.9.4 | TO REPLACE with SDL2_mixer |
| RtMidi | latest | ✅ Active |
| OpenCV | 2.4.8 | to update later |
| MidiShare | 1.91 | ❌ Removed |
## Next Steps
- Continuer le développement fonctionnel (SDL2 en place)
- Remettre la restauration des fenêtres ouvertes au démarrage (window_opened[] remis à zéro après Load_Show pour éviter les crashs — à réactiver quand le rendu de chaque fenêtre sera validé)
- Réactiver les fenêtres désactivées une par une après stabilisation
- OpenCV 2.4.8 à mettre à jour (non bloquant)
## Known Issues
- CPU usage à surveiller (vsync fallback + cap 60fps en place)
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