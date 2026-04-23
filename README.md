# WhiteCat — Lighting Console

**WhiteCat** is a free, open-source stage lighting console for Windows, originally developed by [Christoph Guillermet](https://github.com/ChristophGuillermet) (2009–2016). The project has been taken over and modernized in 2026 by Jacques Bouault — [arpschuino.fr](http://arpschuino.fr).

**WhiteCat** est une console d'éclairage scénique libre et open-source pour Windows, développée à l'origine par [Christoph Guillermet](https://github.com/ChristophGuillermet) (2009–2016). Le projet a été repris et modernisé en 2026 par Jacques Bouault — [arpschuino.fr](http://arpschuino.fr).

---

## Features / Fonctionnalités

- DMX output: ArtNet, Enttec Open DMX, Enttec Pro, Sunlite (simultaneous / simultanées)
- MIDI input/output: multi-port, hotplug (RtMidi)
- Audio playback: WAV, MP3, OGG, FLAC (SDL2_mixer + minimp3)
- Sequencer / cue list
- Trichromie (RGB color mixing)
- Echo (physics-based control)
- Arduino & HF prototype control
- Video tracking module
- Light plot (draw module)
- Patch, XY mover, channel macros, banger

---

## Version 0.9 — What's new / Nouveautés

- **Graphics**: full migration from Allegro 4 + OpenLayer to **SDL2**
- **Audio**: full migration from Audiere to **SDL2_mixer + minimp3** (MP3, WAV, OGG, FLAC)
- **MIDI**: migration from MidiShare (obsolete) to **RtMidi** — multi-port, hotplug
- **DMX**: multiple interfaces active simultaneously; DMX King UltraDMX2 Pro fixed
- **Build**: portable build system (`build.bat`), GCC 5.1.0 (MinGW in `tools/`)
- **IDE**: VSCode integration (build `Ctrl+Shift+B`, debug `F5`)
- Reduced CPU usage (adaptive idle mode, 30 fps cap)
- Cleaned up obsolete source files and old libraries

---

## Build — Windows

### Requirements / Prérequis

- MinGW GCC 5.1.0 — included in `tools/MinGW/` (not tracked by git, download separately)
- SDL2, SDL2_mixer, SDL2_ttf, SDL2_image — included in `whitecatlib/`
- RtMidi — included in `whitecatlib/lib/sources_of_libs/rtmidi/`

### Build

```bat
build.bat
```

Output: `whitecatbuild/build/white_cat_for_mingw/Whitecat_Crossplatform.exe`

### VSCode

Open the project folder in VSCode:
- **Build**: `Ctrl+Shift+B`
- **Debug**: `F5`

---

## Dependencies / Dépendances

| Library | Version | Role |
|---------|---------|------|
| SDL2 | 2.x | Graphics, input, window |
| SDL2_mixer | 2.x | Audio playback |
| SDL2_ttf | 2.x | Text rendering |
| SDL2_image | 2.x | Image loading |
| RtMidi | latest | MIDI I/O |
| OpenCV | 2.4.8 | Video tracking |
| libharu | 2.0.8 | PDF export |
| zlib | 1.2.8 | Compression |

---

## Repository structure / Structure du dépôt

```
whitecat_crossplateform/
├── Src/               — C++ source files
├── whitecatlib/       — Libraries (SDL2, RtMidi, OpenCV…)
├── whitecatbuild/     — Build output and runtime resources
├── tools/             — Portable MinGW compiler (not in git)
└── build.bat          — Build script
```

---

## Licence

White Cat is free software originally developed by Christoph Guillermet.  
You can redistribute it and/or modify it under the terms of the **GNU General Public License v2** (or any later version).

White Cat est un logiciel libre développé à l'origine par Christoph Guillermet.  
Vous pouvez le redistribuer et/ou le modifier selon les termes de la **Licence Publique Générale GNU v2** (ou toute version ultérieure).

See `COPYING` or <http://www.gnu.org/licenses/> for details.
