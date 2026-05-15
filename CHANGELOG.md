# WhiteCat — Changelog

## Version 0.9.x (2026 — Jacques Bouault)

### Audio

- **Chargement OGG/FLAC sans glitch** : charger un fichier OGG ou FLAC dans un player ne coupe plus brièvement le son des autres players en cours de lecture. La cause était un verrouillage interne SDL (`SDL_LockAudio`) pendant le décodage, bloquant le callback audio. Les fichiers sont maintenant lus en RAM puis décodés de façon incrémentale dans le callback (stb_vorbis pour OGG, dr_flac pour FLAC), sans jamais acquérir le lock depuis le thread principal.

  **Limite RAM configurable** (défaut : 300 MB, réglable de 50 à 2048 MB dans *CFG menu → core cfg → Audio RAM OGG/FLAC*). Au-delà de cette limite, l'ancien comportement s'applique (glitch possible au chargement). La valeur correspond à la taille du fichier compressé sur disque, pas au PCM décodé. En pratique avec la limite par défaut :

  | Format | Durée max sans glitch |
  |---|---|
  | OGG 128 kbps stéréo | ~5 h |
  | OGG 256 kbps stéréo | ~2 h 45 |
  | FLAC 44100 Hz / 16 bits stéréo | ~52 min |
  | FLAC 44100 Hz / 24 bits stéréo | ~35 min |
  | FLAC 96000 Hz / 24 bits stéréo | ~16 min |

  Augmenter la limite permet de couvrir des fichiers plus longs, au prix d'une plus grande consommation de RAM au moment du chargement. La valeur est sauvegardée dans `user/config_core.txt` et prise en compte au prochain chargement de fichier (pas de redémarrage nécessaire).

- **Lecture MP3** : les fichiers MP3 de toute taille sont désormais lus correctement via minimp3 (streaming frame-by-frame). Auparavant, la lecture s'arrêtait après une fraction de seconde.
- **Durée des fichiers audio** : la durée totale affichée est maintenant correcte pour tous les formats (MP3 CBR/VBR, WAV, OGG…).
- **Affichage des temps en h:mm:ss** : la position, la durée totale, les points In/Out s'affichent maintenant au format heures:minutes:secondes au lieu d'un nombre décimal de secondes.
- **Courbe de volume logarithmique** : le fader de volume suit une courbe en v³ (loi cubique), ce qui correspond mieux à la perception sonore humaine.
- **Migration audio** : remplacement complet d'Audiere par SDL2_mixer + minimp3. Formats supportés : WAV, MP3, OGG, FLAC.

### MIDI

- **Multi-ports MIDI** : prise en charge de plusieurs interfaces MIDI simultanées.
- **Hotplug MIDI** : branchement/débranchement d'interfaces MIDI à chaud, sans redémarrage.
- **Stabilité MIDI** : les décrochages et déconnexions aléatoires d'interfaces MIDI présents avec l'ancienne bibliothèque (MidiShare) sont résolus.
- **Sauvegarde/restauration** : la configuration MIDI est sauvegardée dans le fichier show et restaurée au chargement.
- **Migration MIDI** : remplacement de MidiShare (obsolète) par RtMidi.

### Circuits DMX

- **Molette souris** : la molette de la souris permet à nouveau de monter/baisser le niveau des circuits sélectionnés (fonctionnalité perdue lors de la migration SDL2, désormais restaurée).
- **Molette véloce** : plus la molette tourne vite, plus le niveau change rapidement (courbe quadratique, sans inertie).

### Performance

- **Réduction CPU — ticker intelligent** : consommation processeur nettement réduite grâce à un mode idle adaptatif et un cap à trois niveaux :
  - **60 fps** (16 ms) tant que l'utilisateur interagit (< 500 ms depuis le dernier événement souris/clavier)
  - **25 fps** (40 ms) pendant les animations automatiques (LFO, chasers, crossfade GO/GO BACK, dampers de faders)
  - **Idle quasi-total** (100 ms) dès qu'aucune animation n'est active

  Le ticker détecte précisément quelles animations sont réellement actives (LFO par canal — modes montée/descente et cyclique —, chasers en cours, passage GO/GO BACK, dampers) et ne signale des frames que si nécessaire. L'ancienne implémentation appelait `wc_request_refresh()` à chaque tick (50 Hz), maintenant le mode 25 fps en permanence même au repos.

  Mesures constatées : CPU 0,4–0,8 %, GPU 0 % au repos ; CPU 7–8 %, GPU 7–8 % pendant des LFOs actifs.

### Interface & graphisme

- **Migration graphique** : remplacement d'Allegro + OpenLayer par SDL2. L'application tourne sans dépendance aux anciennes bibliothèques.

### DMX — Multi-interfaces

- **Sélection multiple** : plusieurs interfaces DMX peuvent être actives simultanément (ArtNet, Enttec Open, Enttec Pro, Sunlite). L'ancien bouton "Double DMX" est supprimé ; chaque interface est un toggle indépendant.
- **DMX King UltraDMX2 Pro** : corrigé — l'interface était détectée mais aucun DMX ne sortait. Causes : taille de trame incorrecte (514 octets au lieu de 513), format du port COM (`\\.\COMx` universel), détection registre étendue à `\Device\USBSER`.
- **Enttec Pro** : détection étendue aux drivers CDC génériques (en plus de FTDI).

### Interface & graphisme — corrections affichage

- **Trait MIDI pastille** : la ligne reliant les faders à leur pastille de retour MIDI s'arrête maintenant exactement sur le bord du cercle (correction géométrique — intersection paramétrique droite/cercle). Corrigé sur : faders, grand master, crossfaders (X1/X2) dans la cue list.
- **Épaisseur cercles** : `Circle::DrawOutline()` respecte désormais la valeur passée à `SetLineWidth()` (la valeur était ignorée dans le backend SDL2 — rendu toujours 1 px). Les boutons Go/GoBack/DoubleGo s'affichent avec une bordure 2 px.
- **Graduation cue list** : suppression des graduations (tirets + chiffres rouges 1.1 à 2.0) affichées à côté des crossfaders X1/X2.
- **Bloc debug** : suppression de l'affichage debug permanent (version, Focus, Over Window, Last_save, string_debug) visible dans l'interface principale.

### Banger

- **Flash des 6 ronds lors de l'activation** : les 6 cercles de la fenêtre banger flashent maintenant en rouge (comme le bouton "bang it") lorsqu'ils sont actifs. Deux corrections :
  - Affichage : les ronds utilisaient `CouleurSurvol` (couleur de survol statique) au lieu de `CouleurFader.WithAlpha(alpha_blinker)` (rouge pulsé).
  - Interaction : un clic direct sur un rond ne déclenchait pas le flash visuel (`do_light_bang_solo[lp]` non mis à jour côté GUI, contrairement au handler MIDI).

### Nettoyage — fichiers obsolètes supprimés

- **Sources C++** : `plot.cpp`, `generate_strings_arrays.cpp`, `dmx_enttec_pro_FTDI.cpp`, `dmx_ftdi_IN_test_enttec_pro.cpp`, `tracker_main14nov.cpp`, `help_2.cpp`, `WhiteCat_private.h`, `debug_informations.h`, `midi_affectation_table.h`, `enttec_pro_dmxin.exe`.
- **Anciennes bibliothèques** : sources et binaires Allegro 4.4.2, OpenLayer 2.1, Audiere 1.9.4, MidiShare 1.91 supprimés de `whitecatlib/`. Documentation OpenLayer (`doc_ol/`, 172 Mo) supprimée.
- **DLL obsolètes** dans le dossier build : `alleg44.dll`, `audiere.dll`, `libopenlayer.dll`, `mshare32.dll`, `msMMSystem.dll`. Log parasite `msMMSystem.log` (65 Mo) supprimé.

### Phase 4 — Découpage en TUs indépendants (en cours)

Extraction progressive des fichiers inclus par `#include <xxx.cpp>` dans MAIN.cpp
vers des TUs compilés séparément, avec leurs propres headers `.h`.

Fichiers extraits (session courante) :
- `faders_core_24.cpp` → `faders_core.cpp` / `faders_core.h`
- `arduino_core_6_UNO.cpp` → `arduino_core.cpp` / `arduino_core.h`
- `arduino_6_UNO.cpp` → `arduino_visu.cpp` / `arduino_visu.h`
- `CFG_config_panel_8.cpp` → `CFG_config_panel.cpp` / `CFG_config_panel.h`
- `midi_13.cpp` → `midi_functions.cpp` / `midi_13.h`
- `banger_core.cpp` → `banger_core.cpp` / `banger_core.h` (nom conservé)

Nouveaux wrappers audio (audio_core.h) : `player_op_is_playing`, `player_op_get_position`,
`player_op_get_length`, `player_op_get_repeat`, `player_op_loop_back_to`.

Fichiers non extractibles (contraintes techniques) :
- `audio_core.cpp` / `audio_visu.cpp` : `audio_backend.h` contient des implémentations concrètes (minimp3, stb_vorbis, dr_flac) sans garde d'inclusion → ne peut être inclus qu'une seule fois
- `keyboard_routines2.cpp` : utilise `wc_key_queue` (static dans `graphics_backend.h`)
- `graphics_rebuild1.cpp` : inclus après les gestionnaires d'événements SDL dans MAIN, ordre critique

### Corrections architecture (Phase 4 TUs)

- **Rendu texte invisible dans les TUs séparés — correction critique** : après l'extraction en TUs indépendants, tout le texte disparaissait dans toutes les fenêtres sauf la fenêtre principale. Cause racine : `wc_cache` et `wc_cache_mutex` étaient déclarés `static` dans `graphics_backend.h`, créant un symbole distinct par TU de compilation — chaque TU avait son propre pointeur initialisé à `nullptr`, au lieu de partager le pointeur alloué dans MAIN.cpp. Conséquence : `Font::Print()` cherchait dans un cache toujours vide et n'affichait rien. Aggravant : le PCH (`wc_tus.h.gch`) ne dépendait pas de `graphics_backend.h` dans le Makefile ; après modification de ce header, le PCH n'était pas régénéré et tous les TUs compilaient avec l'ancienne déclaration `static`.

  Corrections appliquées :
  - Migration de `wc_cache` / `wc_cache_mutex` vers le patron `WC_SKIP_GLOBALS` : définition concrète dans MAIN.cpp (sans `WC_SKIP_GLOBALS`), déclarations `extern` dans tous les autres TUs (avec `WC_SKIP_GLOBALS`).
  - Règle PCH du Makefile corrigée : `$(PCH)` dépend désormais de `$(SRC)/graphics_backend.h` en plus de `$(SRC)/wc_tus.h`, garantissant la régénération du PCH à chaque modification de ce header.

### Technique (sans impact visible direct)

- Compilation avec GCC 5.1.0 (MinGW portable dans `tools/`).
- Build 100 % portable par chemin relatif (`build.bat`).
- Intégration VSCode (build `Ctrl+Shift+B`, debug F5).
- **Signatures** : les 10 fichiers modifiés portent la signature `Jacques Bouault - arpschuino.fr - 2026` dans leur en-tête.
- **Indentation** : reformatage clang-format (4 espaces, style LLVM, col. 120) sur les 10 fichiers modifiés.
