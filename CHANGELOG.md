# WhiteCat — Changelog

## Version 0.9.x (2026 — Jacques Bouault)

### Audio

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

- **Réduction CPU** : consommation processeur nettement réduite grâce à un mode idle adaptatif et un cap à 30 fps. WhiteCat sollicite beaucoup moins la machine en veille ou entre les actions.

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

### Nettoyage — fichiers obsolètes supprimés

- **Sources C++** : `plot.cpp`, `generate_strings_arrays.cpp`, `dmx_enttec_pro_FTDI.cpp`, `dmx_ftdi_IN_test_enttec_pro.cpp`, `tracker_main14nov.cpp`, `help_2.cpp`, `WhiteCat_private.h`, `debug_informations.h`, `midi_affectation_table.h`, `enttec_pro_dmxin.exe`.
- **Anciennes bibliothèques** : sources et binaires Allegro 4.4.2, OpenLayer 2.1, Audiere 1.9.4, MidiShare 1.91 supprimés de `whitecatlib/`. Documentation OpenLayer (`doc_ol/`, 172 Mo) supprimée.
- **DLL obsolètes** dans le dossier build : `alleg44.dll`, `audiere.dll`, `libopenlayer.dll`, `mshare32.dll`, `msMMSystem.dll`. Log parasite `msMMSystem.log` (65 Mo) supprimé.

### Technique (sans impact visible direct)

- Compilation avec GCC 5.1.0 (MinGW portable dans `tools/`).
- Build 100 % portable par chemin relatif (`build.bat`).
- Intégration VSCode (build `Ctrl+Shift+B`, debug F5).
- **Signatures** : les 10 fichiers modifiés portent la signature `Jacques Bouault - arpschuino.fr - 2026` dans leur en-tête.
- **Indentation** : reformatage clang-format (4 espaces, style LLVM, col. 120) sur les 10 fichiers modifiés.
