# WhiteCat Lighting Board - Context for Claude Code
## Project Overview
WhiteCat is an open-source stage lighting console (console d'éclairage scénique) originally developed by Christoph Guillermet (2009-2016). Now maintained and modernized by Jacques Bouault - arpschuino.fr (2026).
- ~106 000 lines of C++ code
- Single-file architecture (all .cpp included via #include in main)
- Windows only for now, but targeting cross-platform
## Build System
- Compiler: GCC 5.1.0 (Code::Blocks MinGW)
- Build script: `build.bat` (run from project root)
- Output: `whitecatbuild/build/white_cat_for_mingw/Whitecat_Crossplatform.exe`
- Libraries: located in `C:\whitecat_crossplateform\whitecatlib\`
- Sources: `D:\nextCloud\ARPSCHUINO_PARTAGE\codes\whitecat_crossplateform\Src\`
## Completed Work
- ✅ Compiled with GCC 5.1.0 (was GCC 4.8.1)
- ✅ Recompiled OpenLayer from sources
- ✅ Fixed al_findnext crashes (replaced with Win32 FindFirstFile/FindNextFile)
- ✅ Fixed mondirectory initialization (GetModuleFileName)
- ✅ Fixed sprintf backslash bug in Load_Show()
- ✅ Migrated from MidiShare to RtMidi (midi_backend.h)
- ✅ Migrated from Code::Blocks to VSCode
## Current Architecture
- `Src/midi_backend.h` — MIDI abstraction layer (RtMidi)
- `Src/midi_CORE.cpp` — MIDI init/quit using midi_backend.h
- `Src/midi_13.cpp` — MIDI processing, now uses process_midi_input()
- `Src/MAIN_janv_2014_opensource_kbd.cpp` — Main loop
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
1. **SDL2 migration** — replace Allegro 4 + OpenLayer + Audiere
   - Create `graphics_backend.h` abstraction layer
   - Replace window/events (Allegro → SDL2)
   - Replace 2D rendering (OpenLayer → SDL2 + SDL2_ttf + SDL2_image)
   - Replace audio (Audiere → SDL2_mixer)
## Known Issues
- Process does not terminate cleanly after quit (Allegro 4 + WinMM threads)
- CPU usage 15-20% (Allegro rendering loop without vsync)
## Important Files
- `Src/whitecat.h` — global variables and declarations
- `Src/CORE_6.cpp` — core functions
- `Src/save_show_13.cpp` — save/load show
- `Src/midi_backend.h` — MIDI abstraction layer
- `build.bat` — build script
- `.vscode/tasks.json` — VSCode build task
- `.vscode/launch.json` — VSCode debug config