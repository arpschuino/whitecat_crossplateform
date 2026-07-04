# WhiteCat — TODO

## ⚠️ À faire avant chaque release

- [ ] **Sync doc/** : copier `whitecatbuild/build/white_cat_for_mingw/doc/` → `doc/` (racine du repo) avant de committer/pusher la release.
  ```
  xcopy /E /Y /I whitecatbuild\build\white_cat_for_mingw\doc doc
  ```

---

## Bugs / Fonctionnalités incomplètes

- [x] **Bug corrigé : Flash** — l'affichage ne redescendait pas immédiatement au relâchement (rendu maintenu quelques frames après la fin du flash) ; le flash était lissé par le damper (écrit `Fader` + snap damper → instantané) ; `LevelFaderBeforeFlash` 8→16 bit. Commit `af49c894` (0.10), backporté 0.9.2 `da241d08`.
- [x] **Bug corrigé : affichage du premier circuit sélectionné** — vue classical, `set_channel_scroll()` plaçait la 1re rangée de chaque page de 48 sous la barre Ch.View (Y=36 < clip ~53) ; offset -3 sur tous les paliers. Commit `af49c894` (0.10), backporté 0.9.2 `da241d08`.
- [x] **Bug corrigé : plantage ticker `gridder_prepare_cross`** — `grid_in_preset` hors bornes → accès `grid_times`/`grid_levels` hors tableau → crash (access violation, thread ticker). Bornage (grille 0-127, pas 0-1023) + log 1re occurrence. Commit 0.10 `73b896da`, backport 0.9.2 `dad76582`.
- [x] **Résolu — cause amont du garbage `grid_goto` (559903)**. Enquête : save/load `grid_goto` corrects (`*sizeof(int)`) ; défaut BSS = 0 (pas -1 ni 559903) ; aucune écriture normale ne produit 559903 → le garbage était **dans le fichier show** (donnée legacy, session passée). Confirmé : **non reproductible** sur show neuf ni en ré-essayant après assainissement. Fixes : (1) **borne haute au test goto** (grider_calcul.cpp:269, grille ≤127 / pas ≤1023 → goto corrompu ignoré, progression normale) ; (2) **assainissement au chargement** (save_show.cpp : grid_goto hors plage → -1, rapport « Sanitized N »). + clamp ticker d'hier. Trois couches défensives.
- [ ] **(optionnel, soft sain) auditer `copy_grid_partially` / `insert_steps`** (grider_calcul.cpp) : la copie/insertion de plages de pas pourrait lire/écrire `grid_goto`/`grid_times`/`grid_levels` au-delà du pas 1023 si la plage n'est pas bornée → lecture OOB propagée (candidat théorique à l'origine du 559903 d'un vieux show). À vérifier + borner si besoin. Aussi : initialiser `grid_goto` à -1 au démarrage (défaut correct = pas de goto, au lieu de BSS=0 = « goto grille 0 »).

- [ ] **Simplifier l'écran d'accueil / l'affichage de la version au démarrage** : le splash défile trop vite, le numéro de version (`versionis`) n'est pas lisible humainement. Repenser durée / lisibilité / position (cf. `show_title()` dans core.cpp et le splash de chargement `save_load_print_to_screen`).
- [ ] Réorganiser la fenêtre MENUS (Call_everybody_5.cpp → Menus()) : Freeze et Exclude retirés, Help retiré → revoir la mise en page des colonnes restantes
- [x] **Documenter le DAMPER de fader** (bouton « ~ » en bas de chaque fader, faders_visuels.cpp:49 `fader_damper_commands` ; Decay/Delta/Mode). Section ajoutée dans `doc/espace_faders2.html` (FR). Reste : version EN (`espace_faders_eng.html`).
- [ ] **Refaire proprement toute la doc des faders** : la doc actuelle (`espace_faders2.html`, DokuWiki 2012) est ancienne et incomplète (damper ajouté à la main, captures à refaire, fonctionnalités récentes manquantes). Reprendre l'ensemble proprement (FR + EN), captures d'écran à jour, 16 bit.
- [x] **Faders & masters en 16 bit (option A)** — FAIT (commit `a57f6ae8` sur 0.10-devices) : `Fader[48]` 16 bit + interpolation de courbe + damper 16 bit + toutes intégrations. Cf. mémoire `crossfade-16bit-plan`. Reste à tester : save→reload, echo/grid snap.
- **Stockage des scènes en 16 bit — vers le 16 bit TOTAL** (⚠️ FONDAMENTAL têtes mobiles). Objectif validé avec Jacques : **16 bit complet et total** à terme. Approche par étapes (échafaudage 8 bit tag `[mem16 s1]` à retirer au fil des stages) :
  - [x] **Phase A — Mémoires (cues) 16 bit** — FAIT. `Memoires` (+`Wiz_Memoires`, buffers temp wizard) → `unsigned short`. Enregistrement/rappel/playback 16 bit direct. Save/load : nouveau `memories16.whc` (16 bit) + fallback `memories.whc` 8 bit ×257 pour vieux shows (décision : **16 bit seul**, pas de double écriture). **Dock mémoire d'un fader** → sortie 16 bit directe (`_mem16`, comme echo/grid). Copier/coller wizard 16 bit.
  - [x] **Stage B0 (PRÉREQUIS) — `grid_levels` en allocation paresseuse par grille** — FAIT (0.10-devices). Classe `GridLevels` (grider.h) : 128 pointeurs de bloc, bloc `[1024][513]` alloué (`calloc`) à la **1re écriture non nulle** ; grille inutilisée = `nullptr` = 0 octet. Syntaxe `grid_levels[g][s][c]` **préservée** via proxys inline `GridP`/`Row`/`Cell` (écrire 0 dans une grille vide = no-op → save/load/clears n'allouent que les grilles utilisées). **Perf** : boucle chaude crossfade (grider_calcul.cpp:444) avec pointeurs de ligne cachés (grille vide → ligne de zéros statique) = **branchless, sans multiplication d'index** (plus rapide que le tableau brut) ; save/load par `memcpy`/`memset` de bloc (`.block()`/`.ensure_block()`/`.free_block()`). Diagnostic `ADDR MAP` (MAIN.cpp) adapté. **Blocs restent `unsigned char` (8 bit)** — le passage en `unsigned short` est le Stage B. ⚠️ Effet de bord traité : l'ancien tableau contigu tolérait les index légèrement hors bornes (lecture grille voisine) ; les blocs séparés plantent → garde `gr_st_selected>0` sur l'aperçu « step précédent » (grider_visu.cpp:491). Validé : crossfade, édition/copie, save→reload, grille vidée.
  - [x] **Stage B — Grid players (endpoints)** — FAIT (0.10-devices). `grid_levels` (+`temp_grid_levels_for_save`) en `unsigned short` (16 bit, alloc paresseuse conservée). Captures sans `lvl_to_dmx8` (saisie/blind/faders/séquentiel/mémoire/snap fader, grider_core.cpp + banger_core.cpp). Crossfade + `refresh_step_in_player` sans `×257`. Compat : nouveaux `grids_levels16_*.whc` + fallback `grids_levels_*.whc` 8 bit ×257 (memset si absent → grilles libérées). Affichages (survol/viewer/PDF) : `LVL_MAX`, alpha /65535, `lvl_to_pct`, casts variadic (proxy Cell). Validé : peinture, Ctrl+molette, crossfade, snap/mémoire, save→reload.
    - [x] **Édition case par case en 16 bit** — FAIT. toggle case `0 ↔ 65535` ; entrée `%` → `wc::pct_to_lvl` (clamp 100) ; entrée DMX → `wc::dmx8_to_lvl`. **Ctrl+molette sur la case survolée** = retouche fine ±1/cran (accel jusqu'à 45) : bloc gated dans channels_core.cpp (comme le bloc fader), **consomme la molette** (`return 0`) pour ne pas piloter les circuits ; cible publiée via `grid_wheel_hover_player` (reset/frame). Overchan (grider_visu) : % exact / DMX 8 bit.
      - **Note d'architecture (horizon devices, À GARDER)** : le `%` est l'unité indépendante de la résolution ET du paramètre (« pan 50 % », « dimmer 22 % » ; un circuit = à terme level + pan + tilt…). L'écriture de case reste centralisée (peinture + Ctrl+molette) → futur point d'accroche « écrire la valeur du paramètre P d'un device » plutôt que « niveau du canal C ». Éviter une « vue DMX16 » qui raisonne canal.
  - [ ] **Stage C — Wizard (maths)** : Set/Add/Reduce niveaux encore en 8 bit (wrappers `[mem16 s1]` wizard_operations.cpp) → passer la maths en 16 bit (%, clamps 65535).
  - [x] **Stage D — Chasers 16 bit** — FAIT. `TrackContains`/`TracksBuffer`/`MergerBufferChasers` (déjà des `int`, type inchangé) portent maintenant du 16 bit. Écritures 16 bit direct (mémoire/stage/blind/faders) ; la math de sortie `/127×track_level` normalisait déjà par `track_level/127` → marche telle quelle. **Dock chaser d'un fader** → sortie 16 bit directe (`_chaser16`). Affichages/PDF en 8 bit (conversion). Compat : nouveau `chaser_trackcont16.whc` + fallback vieux shows ×257.
  - [ ] **Stage E — Export/import ASCII** : format texte 8 bit (wrappers `[mem16 s1]` saves_export_import.cpp). Étendre le format (ex. hex 16 bit) pour fidélité WhiteCat↔WhiteCat — ⚠️ casse l'interchange avec d'autres logiciels (à décider).

  → À rattacher au modèle Fixture (qui saura qu'un attribut est 16 bit). Cf. mémoires `fixtures-patch-model`, `crossfade-16bit-plan`, `memoires-16bit-bottleneck`.

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
