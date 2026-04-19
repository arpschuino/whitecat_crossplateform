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

### Technique (sans impact visible direct)

- Compilation avec GCC 5.1.0 (MinGW portable dans `tools/`).
- Build 100 % portable par chemin relatif (`build.bat`).
- Intégration VSCode (build `Ctrl+Shift+B`, debug F5).
