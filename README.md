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

## Documentation

The user documentation (HTML) is included in the repository in the [`doc/`](doc/) folder.

Open [`doc/introduction.html`](doc/introduction.html) locally in a browser — the home page links to all the documentation (French/English).

---

## Build — Windows

The compiler, the libraries and the runtime files are not in git: they come in a **build kit**
(~80 MB) downloaded by `setup_windows.bat`.
*Le compilateur, les bibliothèques et les fichiers d'exécution ne sont pas dans git : ils sont
fournis par un **kit de build** (~80 Mo) que télécharge `setup_windows.bat`.*

```bat
git clone https://github.com/arpschuino/whitecat_crossplateform.git
cd whitecat_crossplateform
setup_windows.bat
build.bat
```

Output / Sortie : `whitecatbuild/build/white_cat_for_mingw/Whitecat_Crossplatform.exe`

The kit installs / Le kit installe :
- `tools/MinGW/` — TDM-GCC 5.1.0 (32 bit)
- `whitecatlib/` — SDL2 2.30.8, SDL2_ttf 2.24.0, SDL2_image 2.8.8, SDL2_mixer 2.8.1, RtMidi 6.0.0,
  OpenCV 2.4.8, libharu, zlib, FTDI D2XX (reduced copy of
  [ChristophGuillermet/whitecatlib](https://github.com/ChristophGuillermet/whitecatlib) + SDL2 + RtMidi)
- `whitecatbuild/build/white_cat_for_mingw/` — DLLs, fonts, resources, default configuration

`setup_windows.bat` never overwrites an existing file (your `user/` and `saves/` are kept).
Offline: download the kit from the
[Releases](https://github.com/arpschuino/whitecat_crossplateform/releases) page, put the zip next to
`setup_windows.bat` and run it.
*`setup_windows.bat` n'écrase jamais un fichier existant. Hors ligne : téléchargez le kit depuis la
page Releases, posez le zip à côté de `setup_windows.bat` et lancez-le.*

## Build — Linux (x86_64, Raspberry Pi)

```bash
sudo apt install build-essential libsdl2-dev libsdl2-ttf-dev libsdl2-image-dev libsdl2-mixer-dev \
                 libhpdf-dev libasound2-dev libftdi1-dev zlib1g-dev
make -f Makefile.linux -j4
```

Output / Sortie : `whitecatbuild/build/linux/Whitecat_Crossplatform`.
Runtime files (`Fonts/`, `gfx/`, `ressources/`, `user/`…): copy them next to the executable from
the Linux archive of the latest [release](https://github.com/arpschuino/whitecat_crossplateform/releases).
*Fichiers d'exécution : copiez-les à côté de l'exécutable depuis l'archive Linux de la dernière release.*

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
├── whitecatlib/       — Libraries: SDL2, RtMidi, OpenCV… (build kit, not in git)
├── whitecatbuild/     — Build output + runtime resources (build kit, not in git)
├── tools/             — Portable MinGW compiler (build kit, not in git)
├── setup_windows.bat  — Downloads and installs the Windows build kit
└── build.bat          — Build script
```

---

## Licence

White Cat is free software originally developed by Christoph Guillermet.  
You can redistribute it and/or modify it under the terms of the **GNU General Public License v2** (or any later version).

White Cat est un logiciel libre développé à l'origine par Christoph Guillermet.  
Vous pouvez le redistribuer et/ou le modifier selon les termes de la **Licence Publique Générale GNU v2** (ou toute version ultérieure).

See `COPYING` or <http://www.gnu.org/licenses/> for details.
