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
- **Sauvegarde/restauration** : la configuration MIDI est sauvegardée dans le fichier show et restaurée au chargement. Les ports MIDI IN actifs (bitmask) et le port MIDI OUT sélectionné sont rétablis automatiquement au démarrage — le contrôleur est reconnecté sans intervention de l'utilisateur.
- **Migration MIDI** : remplacement de MidiShare (obsolète) par RtMidi.

### Circuits DMX

- **Molette souris** : la molette de la souris permet à nouveau de monter/baisser le niveau des circuits sélectionnés (fonctionnalité perdue lors de la migration SDL2, désormais restaurée).
- **Molette véloce** : plus la molette tourne vite, plus le niveau change rapidement (courbe quadratique, sans inertie).

### MIDI

- **Rafraîchissement des faders en mode idle** : lorsque WhiteCat était en mode veille (aucune interaction utilisateur depuis > 500 ms), les faders actionnés via MIDI n'étaient pas rafraîchis visuellement. Cause racine : `wc_dirty` est une variable `static` locale à chaque TU de compilation — `wc_notify_midi_activity()` appelé depuis `midi_CORE.cpp` mettait à `true` la copie de ce TU, invisible pour la boucle de rendu dans `MAIN.cpp`. De plus, la boucle était bloquée dans `SDL_WaitEventTimeout(100 ms)` et ne se réveillait pas à l'arrivée de données MIDI (événements non-SDL).

  Correction : `wc_notify_midi_activity()` pousse maintenant un `SDL_USEREVENT` via `SDL_PushEvent()` (thread-safe), qui réveille immédiatement `SDL_WaitEventTimeout`. Le handler `wc_handle_event()` s'exécutant dans le contexte de MAIN.cpp, il met à `true` la bonne copie de `wc_dirty` → la boucle principale redessine dès la prochaine itération.

### Performance

- **Réduction CPU — ticker intelligent** : consommation processeur nettement réduite grâce à un mode idle adaptatif et un cap à trois niveaux :
  - **60 fps** (16 ms) tant que l'utilisateur interagit (< 500 ms depuis le dernier événement souris/clavier)
  - **25 fps** (40 ms) pendant les animations automatiques (LFO, chasers, crossfade GO/GO BACK, dampers de faders)
  - **Idle quasi-total** (100 ms) dès qu'aucune animation n'est active

  Le ticker détecte précisément quelles animations sont réellement actives (LFO par canal — modes montée/descente et cyclique —, chasers en cours, passage GO/GO BACK, dampers) et ne signale des frames que si nécessaire. L'ancienne implémentation appelait `wc_request_refresh()` à chaque tick (50 Hz), maintenant le mode 25 fps en permanence même au repos.

  Mesures constatées : CPU 0,4–0,8 %, GPU 0 % au repos ; CPU 7–8 %, GPU 7–8 % pendant des LFOs actifs.

### Channel Views — refonte UI et corrections

- **Barre de menu circuits** : restructuration en deux rangées — rangée haute (boutons opérationnels) et rangée basse (sélecteur Ch.View). Les boutons Freeze et Exclude quittent la fenêtre MENUS et rejoignent la barre, à gauche du bouton BLIND. Le bouton Help quitte MENUS et s'affiche à droite du bouton MENUS. Tous les boutons sont décalés vers la gauche.
- **Sélecteur Ch.View** : les petits carrés passent de 12×12 px à 18×18 px, alignés verticalement avec les boutons Classical/Patched. Renumérotés de 1 à 15 (le carré 0 est remplacé par le bouton "Patched"). Espacement 23 px, numéros centrés en `petitchiffre`.
- **Numérotation des vues** : les vues sont désormais numérotées de 1 à 15 dans tous les affichages (menu, titres de vue, tooltips MIDI). Correction de la numérotation chargée depuis le fichier de sauvegarde (migration automatique des anciens noms "CHANNEL VIEW N+1" → "CHANNEL VIEW N" au chargement).
- **Vue Patched** : la vue 0 (Patched) n'affiche plus les circuits déjà assignés à une Channel View 1–15 — seuls les circuits patchés non encore affectés à une autre vue y apparaissent.
- **Titre de vue** : suppression de l'indicateur de nombre de circuits (petit chiffre en haut de chaque titre de vue).

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

- **Hold-to-scroll sur les boutons banger** : maintenir le bouton gauche enfoncé sur les boutons de navigation banger (BANGER ++/--), de type d'événement et d'action déclenche maintenant un défilement automatique rapide (délai initial 400 ms, répétition toutes les 80 ms). Avant, un seul clic était enregistré quelle que soit la durée d'appui.

  Fichiers modifiés : `Src/banger_core.cpp`, `Src/MAIN.cpp`.

  Détails techniques :
  - Mécanique timer SDL (`SDL_GetTicks`) : seuil initial 400 ms + répétition 80 ms.
  - `wc_request_refresh()` appelé inconditionnellement (pas uniquement lors d'un fire) pour maintenir la boucle en mode actif (25–60 fps) pendant l'appui.
  - `wc_dirty = true` forcé dans la boucle principale (`MAIN.cpp`) à chaque itération où `check_graphics_mouse_handling()` est appelé, garantissant le rafraîchissement visuel à chaque frame pendant le maintien.

- **Flash des 6 ronds lors de l'activation** : les 6 cercles de la fenêtre banger flashent maintenant en rouge (comme le bouton "bang it") lorsqu'ils sont actifs. Deux corrections :
  - Affichage : les ronds utilisaient `CouleurSurvol` (couleur de survol statique) au lieu de `CouleurFader.WithAlpha(alpha_blinker)` (rouge pulsé).
  - Interaction : un clic direct sur un rond ne déclenchait pas le flash visuel (`do_light_bang_solo[lp]` non mis à jour côté GUI, contrairement au handler MIDI).

### Nettoyage — fichiers obsolètes supprimés

- **Sources C++** : `plot.cpp`, `generate_strings_arrays.cpp`, `dmx_enttec_pro_FTDI.cpp`, `dmx_ftdi_IN_test_enttec_pro.cpp`, `tracker_main14nov.cpp`, `help_2.cpp`, `WhiteCat_private.h`, `debug_informations.h`, `midi_affectation_table.h`, `enttec_pro_dmxin.exe`.
- **Anciennes bibliothèques** : sources et binaires Allegro 4.4.2, OpenLayer 2.1, Audiere 1.9.4, MidiShare 1.91 supprimés de `whitecatlib/`. Documentation OpenLayer (`doc_ol/`, 172 Mo) supprimée.
- **DLL obsolètes** dans le dossier build : `alleg44.dll`, `audiere.dll`, `libopenlayer.dll`, `mshare32.dll`, `msMMSystem.dll`. Log parasite `msMMSystem.log` (65 Mo) supprimé.

### UI / Séquenciel

- **Édition inline des noms et annotations dans la cue list** : un double-clic sur le nom ou la ligne d'annotation d'une mémoire (onstage, preset, mem_before_one, mémoires suivantes) ouvre un champ d'édition directement dans la cue list. La modification est validée par Entrée ou annulée par Échap. Le texte s'affiche en `petitchiffre` avec un scroll horizontal automatique si le contenu dépasse la largeur du champ.
- **Navigation curseur ← → dans tous les champs texte** : les flèches gauche/droite permettent de déplacer le curseur à l'intérieur du texte en cours de saisie, dans toutes les fenêtres d'édition — cue list (inline), fenêtre de confirmation W_ASKCONFIRM, et saisie numérique F5. La navigation est UTF-8 correcte (pas de coupure en milieu de caractère multi-octet). Le curseur clignote à 25 fps dans toutes ces fenêtres.
- **Saisie de numéro de mémoire** : la saisie d'un numéro supérieur à 999.9 affiche désormais un message d'erreur dans la fenêtre de confirmation ("Invalid memory number (max 999.9)") au lieu de tronquer silencieusement l'index. Aucune mémoire n'est créée en cas de dépassement.
- **Fenêtre W_ASKCONFIRM élargie** : largeur passée de 400 à 500 px pour accueillir les messages d'erreur sans débordement.

### Banger

- **Type 6 renommé Sequences → CueList** : le libellé "Sequences" est remplacé par "CueList" dans l'affichage du type de banger, la fenêtre de type Windows et le type de fader.
- **Validation val1 / val2 (CueList)** : val1 (partie entière) bornée à 0–999, val2 (décimale) à 0–9 — le popup reflète clairement les deux bornes.

### MIDI — Change Reaction

- **Ascenseur (scrollbar)** : remplacement des anciens boutons LineUp/LineDown par un ascenseur complet — piste, thumb draggable, flèches `^`/`v`. La liste de 128 entrées défile 10 par 10.
- **Flèches à répétition continue** : maintenir le bouton enfoncé sur `^` ou `v` scrolle en continu sans avoir à relâcher.
- **Scroll molette** : la roulette de la souris fait défiler la liste lorsque le curseur survole la zone Change Reaction.
- **Protection anti-clic** : les items de la liste ne changent plus de mode pendant le drag du thumb.

### Performance — Font cache (Phase 5)

- **Cache texte 4-way set-associatif** : remplacement du cache direct-mapped 512 slots par un cache 4-way LRU 2048 slots (512 sets × 4 ways). Sur un miss, le slot le moins récemment utilisé du set est évinc (horodatage `SDL_GetTicks()`), au lieu d'écraser aveuglément l'unique slot disponible. Réduit drastiquement le nombre de `SDL_DestroyTexture` / `SDL_CreateTextureFromSurface` par frame dans les vues denses (patch, séquenciel, faders). Empreinte mémoire : ~310 Ko (stable, pas d'allocation dynamique).

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
