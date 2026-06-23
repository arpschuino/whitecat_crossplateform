# WhiteCat — TODO

## ⚠️ À faire avant chaque release

- [ ] **Sync doc/** : copier `whitecatbuild/build/white_cat_for_mingw/doc/` → `doc/` (racine du repo) avant de committer/pusher la release.
  ```
  xcopy /E /Y /I whitecatbuild\build\white_cat_for_mingw\doc doc
  ```

---

## Bugs / Fonctionnalités incomplètes

- [ ] **Simplifier l'écran d'accueil / l'affichage de la version au démarrage** : le splash défile trop vite, le numéro de version (`versionis`) n'est pas lisible humainement. Repenser durée / lisibilité / position (cf. `show_title()` dans core.cpp et le splash de chargement `save_load_print_to_screen`).
- [ ] Réorganiser la fenêtre MENUS (Call_everybody_5.cpp → Menus()) : Freeze et Exclude retirés, Help retiré → revoir la mise en page des colonnes restantes

---

## Phase 4 — vérifications post-extraction TU

### À vérifier : procs_visuels_rebuild1.cpp
- [x] Clic sur la croix (X) d'une fenêtre → ferme bien la fenêtre
- [x] Clic sur le bouton M → déplace bien la fenêtre
- [x] `detection_over_window` : passer la souris sur chaque fenêtre ouverte → focus correct
- [x] Déplacer plusieurs fenêtres différentes (trichromie, séquentiel, patch…)
- [x] `logical_channelspace` : scroll circuits en vue classique et en vue preset
- [x] `check_graphics_mouse_handling` : vérifier que les interactions souris fonctionnent dans chaque fenêtre ouverte (faders, banger, audio, grider, chasers…)

### À vérifier : dmx_functions.cpp (ex dmx_functions_13)
- [x] Sortie DMX ArtNet : les circuits s'allument correctement
- [ ] Sortie DMX Enttec Open : idem (si interface dispo)
- [x] Sortie DMX Enttec Pro : idem (si interface dispo)
- [x] Crossfade GO : le passage de mémoire se fait bien (niveaux, timing)
- [x] GO BACK : retour à la mémoire précédente OK
- [x] LFOs : un fader avec LFO actif monte/descend correctement
- [x] Merger : HTP faders + séquentiel → résultat correct sur les circuits
- [ ] DMX IN (Enttec Pro IN ou Sunlite) : réception et affectation à un dock

### Phase 4 — fichiers TU restants à extraire
- [x] saves_export_pdf2.cpp
- [x] save_show.cpp (ex save_show_13.cpp)
- [x] logicals_intres.cpp
- [x] wizard.cpp
- [x] video_tracking_core.cpp
- [x] faders_core_24.cpp → faders_core.cpp
- [x] arduino_core_6_UNO.cpp → arduino_core.cpp
- [x] arduino_6_UNO.cpp → arduino_visu.cpp
- [x] CFG_config_panel_8.cpp → CFG_config_panel.cpp
- [x] midi_13.cpp → midi_functions.cpp
- [x] banger_core.cpp → banger_core.cpp / banger_core.h

### À vérifier : banger_core.cpp
- [x] Bang "bang it" : clic sur le bouton → flash rouge des 6 ronds + action
- [x] Bang depuis mémoire (do_send_bang) : rappel mémoire avec bang associé
- [ ] do_go_back_bang : retour au banger précédent
- [x] Chrono_Reset : reset du chrono dans la fenêtre time
- [ ] do_loop_bang : banger en boucle → cycle continu sans blocage
- [x] do_logical_fenetre_banger : ouvrir la fenêtre banger, interagir avec les boutons (hold-to-scroll ajouté sur ++/--, type, action)

### À vérifier : midi_functions.cpp (ex midi_13)
- [x] Envoi MIDI CC depuis un fader (fader MIDI out)
- [x] Réception MIDI CC → déplace un fader
- [x] Note ON/OFF depuis launchpad → action correspondante
- [ ] simulate_midi() : déclencher une action MIDI simulée depuis menu
- [ ] clock_wheel : tempo MIDI ticker visible et fonctionnel
- [x] do_midi_config : ouvrir la fenêtre de config MIDI, changer un port, sauvegarder

### À vérifier : faders_core.cpp (ex faders_core_24)
- [x3333] Déplacer un fader à la souris → niveau circuit OK
- [x] LFO sur fader : oscillation visible (testé — montée/descente + cyclique)
- [ ] Lock preset : verrouiller/déverrouiller un fader
- [x] Dock fader : affecter une mémoire à un fader

### À vérifier : arduino_core + arduino_visu
- [ ] Connexion Arduino sur port COM → données reçues (analog/digital)
- [ ] arduino_do_digital_out_whitecat : envoi digital vers Arduino
- [ ] do_arduino_config : affichage de la fenêtre de config Arduino

### À vérifier : validation banger val1/val2 (banger_core.cpp)
- [x] Fader (type 1) : val1 = 1 à core_user_define_nb_faders → popup si dépassement
- [ ] MIDI (type 2) : val1 = Ch.Midi 0-15 (actions 0-4) ou 0-127 (actions 5+) ; val2 = 0-127 ou 0-1 selon action → popup
- [ ] Windows (type 3) : val1 ≤ 128, val2 = 0-1440
- [x] Alarm (type 4) : val2 = 0-1
- [ ] Audio (type 5) : val1 = 0 à index_nbre_players_visibles ; val2 = 0-126 (fichier, actions 1/3), 0-127 (vol/pan/pitch), 0-1 (autres) — **vérifier borne haute fichier (0 ou 1 indexé ?)**
- [x] Sequences (type 6) : val1 ≤ 999
- [ ] Chasers (type 7) : val1 = 1 à core_user_define_nb_chasers
- [ ] MiniFaders (type 8) : val1 ≤ core_user_define_nb_faders, val2 = 0-1
- [ ] Chrono (type 10) : val1 = 0-2
- [ ] SetBanger (type 12) : val1 ≤ 128, val2 = 0-1
- [ ] MidiMute (type 13) : val1 ≤ core_user_define_nb_faders, val2 = 0-1
- [ ] GridPlayer (type 14) : val1 ≤ 127
- [ ] Hardware (type 15) : val1 = 0-1, val2 = 0-50 (port COM)
- [ ] Draw (type 17) : val1 = 1-6

### Phases suivantes
- [x] **Ticker intelligent** — wc_request_refresh() ciblé (LFO/chasers/GO/dampers) + cap 3 niveaux (60/25/idle fps)
- [x] **Correction PCH Makefile** — graphics_backend.h ajouté comme dépendance du PCH
- [x] **wc_cache global WC_SKIP_GLOBALS** — rendu texte restauré dans tous les TUs
- [x] **Phase 5** — Font cache 4-way set-associatif LRU (2048 slots, éviction LRU par SDL_GetTicks)
- [x] **Phase 6** — Dirty rects (texture cache + wc_bg_dirty + wc_win_dirty + throttle 67ms → 33% → ~18% CPU)
- [ ] **Phase 7** — Port Linux / Raspberry Pi 3

---

## Phase 7 — Port Linux / Raspberry Pi 3 / macOS

Cibles :
- **Linux x86** (Ubuntu/Debian) — étape intermédiaire avant Pi
- **Raspberry Pi 3** (Cortex-A53, 1GB RAM, VideoCore IV, RPi OS Lite + X11)
- **macOS** (Apple Silicon + Intel, Homebrew SDL2)

### 7a — Audit dépendances Windows
- [ ] Inventaire de tous les `#include <windows.h>` / `<winsock2.h>` dans le code
- [ ] Liste des appels Win32 : `GetModuleFileName`, `FindFirstFile/NextFile`, `WSAStartup`, `SetUnhandledExceptionFilter`, `FTD2XX`, serial COM
- [ ] Identifier ce qui peut être gardé avec `#ifdef _WIN32` vs ce qui doit être réécrit

### 7b — Build system Linux
- [ ] Makefile Linux (GCC, SDL2, RtMidi ALSA, SDL_mixer, SDL_ttf, SDL_image)
- [ ] Retirer `-mwindows`, `whitecat_res.o`, FTD2XX.lib du link Linux
- [ ] Premier objectif : **compiler sur Linux x86** (Ubuntu/Debian) avant de cibler Pi
- [ ] Makefile ARM pour Pi 3 (cross-compile depuis Linux x86 ou compilation native)

### 7b-mac — Build system macOS
- [ ] Makefile macOS (Clang, SDL2 via Homebrew, RtMidi CoreMIDI, SDL_mixer, SDL_ttf, SDL_image)
- [ ] Retirer `-mwindows`, `whitecat_res.o`, FTD2XX.lib du link macOS
- [ ] Gérer Universal Binary (arm64 + x86_64) ou cibles séparées

### 7c — Portabilité chemins et fichiers
- [ ] `GetModuleFileName` → `/proc/self/exe` (Linux) / `dladdr` ou `NSBundle` (macOS)
- [ ] Séparateurs `\` → `/` dans les chemins (save/load, chemins relatifs)
- [ ] `FindFirstFile/FindNextFile` → `opendir/readdir` POSIX (Linux + macOS)
- [ ] Dossier temporaire (`GetTempPath`) → `/tmp`

### 7d — Portabilité réseau Art-Net
- [ ] `winsock2.h` / `WSAStartup` / `WSACleanup` → POSIX `sys/socket.h` (Linux + macOS)
- [ ] `SOCKADDR` → `struct sockaddr_in`
- [ ] Wrapper `#ifdef _WIN32` / `#else` dans network_artnet_3.cpp

### 7e — DMX Linux
- [ ] FTD2XX.lib (Windows-only) → sur Linux : `/dev/ttyUSB0` via libftdi ou driver kernel FTDI
- [ ] Option court terme : désactiver DMX USB hardware sur Linux, garder Art-Net (aucune lib externe)
- [ ] Enttec Pro / Enttec Open / DMX King : implémenter backends Linux si interfaces disponibles

### 7e-mac — DMX macOS
- [ ] FTD2XX.lib → libftdi (Homebrew) ou D2XX macOS d'FTDI (dylib macOS officielle)
- [ ] Option court terme : désactiver DMX USB hardware sur macOS, garder Art-Net
- [ ] Serial : `/dev/tty.usbserial-*` ou `/dev/cu.usbmodem*` (Arduino)

### 7f — Divers
- [ ] Retirer `SetUnhandledExceptionFilter` (Win32) → garder `signal(SIGABRT)` déjà en place
- [ ] Icône application : ressource `.rc` Windows → `.icns` macOS / pas nécessaire sur Linux
- [ ] OpenCV : rebuilder 2.4.8 pour ARM, ou désactiver `W_TRACKINGVIDEO` sur Pi/Mac (non bloquant)
- [ ] Serial Arduino : vérifier si le code COM est POSIX-compatible (`/dev/ttyACM0` Linux, `/dev/cu.*` macOS)
- [ ] RtMidi : CoreMIDI sur macOS (déjà supporté par RtMidi ✅), ALSA sur Linux ✅

### 7g — Test et tuning Pi 3
- [ ] Compiler nativement sur Pi 3 (RPi OS Lite, `sudo apt install libsdl2-dev libsdl2-ttf-dev libsdl2-image-dev libsdl2-mixer-dev`)
- [ ] Mesurer les temps de render réels (fond, fenêtres, hover)
- [ ] Ajuster throttle / cap FPS si nécessaire pour le VideoCore IV
- [ ] Valider : MIDI ALSA, DMX Art-Net, audio SDL_mixer

### 7h — Test macOS
- [ ] Compiler sur macOS (Homebrew : `brew install sdl2 sdl2_ttf sdl2_image sdl2_mixer`)
- [ ] Valider RtMidi CoreMIDI, Art-Net POSIX, audio SDL_mixer
- [ ] Tester sur Intel et Apple Silicon (arm64)
