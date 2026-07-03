# WhiteCat — Changelog

## Version 0.9.2 (en cours — Jacques Bouault)

### MIDI — Sortie (OUT)

- **Fix : sélection du périphérique MIDI OUT impossible** dès qu'un périphérique MIDI IN était présent. Dans la fenêtre de configuration MIDI, le clic posait son indicateur à `do_connect_out[j + compt_midi_in]` (et lisait l'état au même décalage), alors que l'ouverture du port lit `do_connect_out[j]`. Vestige de l'époque MidiShare (liste IN+OUT commune) ; RtMidi indexe IN et OUT séparément. Le décalage `+compt_midi_in` est retiré.
- **Fix : Control Change jamais émis en sortie**. `emit_midi_out()` envoie les faders avec le type MIDI `miditable[0]` (convention WhiteCat : `4` = Control Change), mais `midi_backend_send()` codait le CC en `case 3` sans `case 4` → les CC tombaient dans `default` et n'étaient jamais envoyés. CC géré à `case 4` (les Notes, types 1/2, fonctionnaient déjà).
- *Note : sous Windows, un port MIDI créé à chaud (ex. loopMIDI) après le lancement n'apparaît qu'au redémarrage — limite de l'API WinMM, pas un bug WhiteCat. Créer/brancher les ports avant de lancer WhiteCat.*

### Banger — Bouton LOOP

- **Fix : la mise en boucle ne fonctionnait pas**. L'intervalle de relance utilisait un facteur `×10000` hérité du timer Allegro : à 50 Hz, un réglage de « 2 s » donnait ~400 s avant relance. Corrigé en `×50` (`do_loop_bang()` tourne à 50 Hz → `time_loop_banger` est réellement en secondes).
- **Fix : affichage figé pendant une boucle**. Le « ticker intelligent » (cap fps d'économie CPU) détecte les LFO/chasers/GO/dampers mais pas les bangers : un banger en boucle laissait WhiteCat passer en veille d'affichage → les événements paraissaient désynchronisés/manquants **à l'écran** (la sortie DMX/MIDI restait correcte ; bouger la souris « réparait »). Les bangers en boucle sont ajoutés à la détection d'activité du rendu. Aucun surcoût au repos.

### Faders — Lock

- **Fix : un clic sur Lock mettait le fader à zéro.** Le toggle Lock était inversé/incomplet et `locklevel` valait 0 par défaut : sans master lock actif, le déverrouillage restaurait `niveau_sauvé × locklevel = 0`. Toggle corrigé et `locklevel` par défaut = plein (255).

### Faders — Flash

- **Fix : le fader ne redescendait pas immédiatement à l'écran au relâchement du Flash** (il restait figé haut jusqu'à un mouvement de souris). Le relâchement est détecté dans le code de dessin, mais le niveau n'est restauré qu'au tick suivant du merger : le rendu est désormais maintenu quelques frames après la fin du flash. Même classe de bug que le « banger en boucle ».
- **Fix : le Flash était lissé par le damper.** Un fader avec damper actif montait/redescendait progressivement au flash au lieu d'être instantané. Le flash écrit maintenant le niveau directement (et raccorde l'état du damper) → montée et descente instantanées.

### Faders — Damper

- **Fix : avec un damper actif, le fader ne redescendait pas complètement** (ex. piloté par LFO : il restait un poil au-dessus de 0). La glisse exponentielle du damper est asymptotique → elle n'atteint jamais la cible pile. Ajout d'un accrochage exact à la cible (snap) quand l'écart devient négligeable. Corrige aussi une initialisation manquante (`_damper_blocking_mode`/`_damper_accel`) qui pouvait figer le damper.

### Espace circuits — Vue Classical

- **Fix : le premier circuit d'une page (1-12, 49-60, …) caché sous la barre « Ch.View ».** L'auto-scroll à la sélection (`set_channel_scroll`) plaçait la 1re rangée de chaque page de 48 trop haut (Y=36, sous le clip à ~53). Offset -3 appliqué à tous les paliers (= valeur déjà validée au démarrage).

### Echo — Affichage

- **Fix : animation de l'echo saccadée à l'écran** (fluide seulement en bougeant la souris). Le « ticker intelligent » ne détectait pas les echos en cours de rebond : ajoutés à la détection d'activité du rendu (même correctif que les bangers en boucle). La sortie DMX, elle, était correcte.

### Sauvegarde / Chargement — Rapport

- **Fix : ligne « audio_conf.txt readed » affichée en rouge** au rechargement d'un show alors que la lecture réussissait. Plusieurs opérations partagent le même slot de rapport (`idf`) ; la branche succès ne remettait pas l'indicateur d'erreur à zéro. Corrigé.

### Grid players

- **Fix : plantage (thread ticker) en éditant / navigant dans une grille.** Dans `gridder_prepare_cross()` (calcul du fondu, appelé 50×/s par le ticker), les indices `grid_in_preset` (grille/pas du pas *suivant*) pouvaient sortir des bornes selon le chemin (goto, asservissement séquentiel, ou `pas+1 = 1024` au dernier pas) et l'accès `grid_times[grille][pas]` lisait **hors du tableau** → access violation (`0xC0000005`), diagnostiquée via `wc_debug.txt` (adresse → `grider_calcul.cpp:334`). Indices désormais **bornés** (grille 0-127, pas 0-1023) avant tout accès ; la 1re occurrence est loguée pour tracer la cause. *(Un garde-fou d'affichage a aussi été ajouté à l'aperçu « pas précédent » qui lisait le pas -1 au pas 1 — bénin sur tableau contigu, mais incorrect.)*
- **Fix : animation saccadée à l'écran** (fluide seulement en bougeant la souris). Le « ticker intelligent » ne détectait pas les grid players en lecture : ajoutés à la détection d'activité du rendu (même correctif que les bangers en boucle / l'echo). La sortie DMX, elle, était correcte.
- **En-tête réagencé** : le titre « Grid Players » (écrit sur deux lignes, qui débordait) passe sur **une seule ligne** ; les champs Beg.Chan / Col / Rows / edit / View / cases GridPlayers sont décalés à droite pour lui faire de la place (rendu **et** zones de clic alignés).
- **Global viewer** : la ligne Grid / Step est recadrée de quelques pixels vers la droite (le label « Grid » touchait le bord gauche de la fenêtre).

### Bangers — vestige iCat retiré

- **La catégorie « iCat »** (et son action « iCAT Builder » dans la catégorie Windows) subsistait dans le sélecteur d'événements banger alors que la fenêtre iCat a été supprimée. Catégorie rendue inatteignable (défilement vers le haut ET vers le bas + chargement d'un show) et code mort retiré.

### Interface

- **Fenêtre principale redimensionnable à la souris** : on peut désormais agrandir / rétrécir la fenêtre en tirant ses bords. L'interface garde ses coordonnées fixes — agrandir donne plus d'espace vide autour, rétrécir rogne le bas / la droite. *(Sous Windows, le contenu peut apparaître étiré pendant le glissement de bordure — boucle de redimensionnement modale de l'OS — puis redevient net au relâché ; sous Linux/X11 c'est en principe plus fluide.)*
- **Fix : nom (F5) — chiffres du pavé numérique comptés double** (« 01 » donnait « 0101 »). Les touches du pavé numérique portent le bit `SDLK_SCANCODE_MASK` : elles échappaient au filtre qui laisse la saisie texte à `SDL_TEXTINPUT` en mode nom → elles étaient ajoutées deux fois (file de touches + `SDL_TEXTINPUT`). Traitées comme les chiffres de la rangée du haut.

---

## Version 0.9.1 (28 mai 2026 — Jacques Bouault)

### Portage Linux — corrections de bugs (profitent aussi à Windows et au Raspberry Pi)

- **Compatibilité SDL ancienne** : `KMOD_SCROLL` (SDL ≥ 2.0.18) et les hints DPI `SDL_HINT_WINDOWS_DPI_*` (SDL ≥ 2.24) sont désormais protégés par des gardes de version, permettant de compiler contre SDL 2.0.10 (Ubuntu 18.04). Le binaire Linux est ainsi compatible glibc 2.27 → fonctionne de Mint 19 / Ubuntu 18.04 / Debian 10 jusqu'aux versions les plus récentes.
- **Séparateurs de chemin** : 18 constructions de chemins utilisaient le séparateur Windows `\` codé en dur (saves, audio, import/export, plans, ressources). Sur Linux, ces antislashs devenaient des caractères littéraux dans les noms de fichiers (`saves\last_save` au lieu de `saves/last_save`), cassant la sauvegarde et le rechargement. Tous remplacés par `/` (valide sur Windows ET Linux).
- **Tableaux Arduino sous-dimensionnés** : `digital_limit` (127 → 128) et `analog_limit` (63 → 64) ne correspondaient pas à la taille de sérialisation utilisée par les `fread`/`fwrite` (128/64). Le chargement d'un show lisait un élément de trop → débordement mémoire (corruption silencieuse sur Windows, crash `__fread_chk` sur Linux).
- **Buffer overflow `do_sprintf_job`** : `string_ratio_x1x2[4]` était trop petit pour `sprintf("%.2f", …)` (toujours ≥ 5 octets). Débordement à chaque rafraîchissement → crash immédiat sous Linux (détecté par `_FORTIFY_SOURCE`). Buffer agrandi, valeurs de temps bornées dans `affichage_time_format`.
- **Diagnostic crash** : ajout d'un gestionnaire de signaux POSIX (SIGSEGV/SIGBUS/SIGFPE/SIGABRT) qui écrit la pile d'appel (backtrace symbolisé via `-rdynamic`) dans `wc_debug.txt`, facilitant le diagnostic des plantages sur le portage Linux/Pi.

### MIDI — Crossfade continu (sans raccrochage des potards)

- **Nouvelle option** dans l'onglet *MIDI PRESETS & OPTIONS* : **Continuous xfade**. Lorsqu'elle est activée, les potards X1/X2 (contrôles MIDI 491/492) permettent d'enchaîner les crossfades sans jamais avoir à les recharger :
  - **Descendre les deux potards** → le crossfade se déclenche (preset → stage), la phase bascule en "inversée"
  - **Remonter les deux potards** → le crossfade suivant se déclenche automatiquement, la phase revient en "normale"
  - Et ainsi de suite indéfiniment, en alternant descente et montée.

  En mode classique (option désactivée), le comportement existant est conservé : WC attend que les potards soient remontés ("GET UP MIDI FADERS") avant d'accepter un nouveau crossfade.

  L'option est sauvegardée dans le fichier show (`index_report_customs[71]`).

### MIDI — Configuration

- **4e onglet "MIDI CLOCK"** : les paramètres de l'horloge MIDI (BPM, encodeur, grille de tempos mémorisés) sont déplacés dans un onglet dédié. Cela libère de la place dans l'onglet *MIDI PRESETS & OPTIONS* pour accueillir les nouvelles options.
- **Layout 2 colonnes** dans l'onglet *MIDI PRESETS & OPTIONS* : les 9 options s'affichent sur deux colonnes au lieu d'une, évitant tout débordement de la fenêtre de configuration.
- **Fix : position du rond orange** de la roue de vitesse MIDI CLOCK au démarrage — le curseur était affiché en (0, 0) à l'ouverture car sa position absolue était calculée depuis un emplacement hardcodé de l'ancienne version. Le curseur est maintenant recalculé à chaque rendu depuis le centre de la roue.

### Cue list — Interface

- **Boutons ovales du bas élargis de 3 px** (50 → 53 px) : le label "Preset+" tient maintenant entièrement dans son bouton.
- **Grille plus contrastée** : les traits verticaux (séparation colonnes) et horizontaux (séparation lignes) passent de `alpha 0.5` à `alpha 0.35`, rendant la grille légèrement plus foncée et plus lisible.

### Banger

- **Fix : taille de sauvegarde des délais** (`banger_times_size`) : la valeur était `128×6×2` au lieu de `128×6`, causant un dépassement de tampon lors de la lecture/écriture du fichier `banger_times.whc` (le double des octets nécessaires étaient lus/écrits). Les fichiers existants se chargent correctement — seuls les 768 premiers floats (délais réels) sont lus, la portion excédentaire est ignorée.
- **Affichage val1/val2** : les colonnes val1, val2, action et délai ne s'affichent plus pour les événements de type 0 (vide), éliminant l'apparition de valeurs aberrantes dans les lignes non configurées.

### Circuits — Affichage des niveaux

- **Fix : latence des niveaux** dans la liste des circuits lors d'un mouvement rapide de fader. `Merger()` (qui calcule `bufferSequenciel` à partir des faders) tourne dans un timer SDL séparé à ~50 Hz : l'affichage pouvait donc avoir jusqu'à 20 ms de retard sur la valeur réelle. `Merger()` est maintenant appelé juste avant chaque dessin → les niveaux sont toujours à jour.

### Faders — REPORT (F3)

- **Champ titre pour le REPORT** : enregistrer un niveau stage dans un dock via F3 + clic affiche désormais le champ "Nom" dans la fenêtre de confirmation (comme STORE), et le nom saisi est appliqué au dock.
- **Confirmation par F3** : la fenêtre de confirmation d'un REPORT se valide avec **F3** (F1 reste réservé à STORE). Le filtre de saisie du champ nom laisse désormais passer F3.

### Fenêtre Save — Ascenseurs

- **Ascenseurs complets** : les ronds +/- des listes (import/export et shows) sont remplacés par un ascenseur identique à celui de la fenêtre audio — piste, flèches haut/bas, thumb déplaçable à la souris, et **défilement à la molette**. L'ascenseur n'apparaît que si la liste dépasse la zone visible.

### Fenêtre Save — Sélecteurs d'éléments

- **Entrées mortes retirées** : iCAT, MOVER et le slot vide "-" (fonctionnalités supprimées) disparaissent des sélecteurs de sauvegarde binaire et d'export PDF.
- **Réorganisation sans trou** : le sélecteur binaire est piloté par tableaux (label + index réel) et affiché en **3 colonnes de 11** ; le sélecteur PDF est nettoyé de la même façon. Les index de sauvegarde sont préservés → compatibilité totale avec les anciens fichiers show.

### Fenêtre MENUS — Réorganisation

- **Disposition par tableau** : les 5 colonnes codées en dur (inégales 5/3/5/5/3, avec des trous laissés par le retrait de Freeze/Exclude/Help) sont remplacées par une table unique partagée découplant la position d'affichage de l'action et de l'ID MIDI. Affichage en **4 colonnes de 4 + 1 colonne de 5**.

---

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
- **Colonnes Link / Bang / Gpl.1 uniformisées** : les trois colonnes passent à 30 px de large chacune avec un espacement égal de 3 px, libérant de la place pour la colonne texte. Le label "Banger" est raccourci en "Bang". Zones de clic et de survol recalées en conséquence.
- **Zone texte agrandie de 36 px** : largeur inscriptible passée de 148 px à 184 px (+24 px par réduction des colonnes Link/Bang/Gpl.1, +4 px par récupération de la marge droite de la boîte d'édition — sans déplacer aucun élément visible).

### MIDI — Crossfade continu (sans raccrochage)

- **Enchaînement crossfade X1/X2 sans recharger les potards** : nouvelle option dans l'onglet MIDI de la configuration. Lorsqu'elle est activée, le crossfade manuel via les potards X1/X2 (contrôles MIDI 491/492) fonctionne en mode alterné :
  - **Descendre les deux potards** → le crossfade se déclenche (preset → stage), phase bascule en "inversée"
  - **Remonter les deux potards** → le crossfade suivant se déclenche automatiquement, phase revient en "normale"
  - Et ainsi de suite, sans jamais avoir à recharger les potards.

  En mode classique (option désactivée), le comportement existant est conservé : WC attend que les potards soient remontés ("GET UP MIDI FADERS") avant d'accepter un nouveau crossfade.

  L'option `seq_midi_xfade_continuous` est sauvegardée dans le fichier show (index_report_customs[71]). La phase interne `seq_midi_xfade_inverted` est réinitialisée à `false` lorsque l'option est désactivée.

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

### Clavier / Saisie numérique

- **Touche `.` du pavé numérique** : la touche point du pavé numérique ne fonctionnait pas. Cause : `KEY_DEL_PAD` était mappé sur `SDL_SCANCODE_KP_DECIMAL` (scancode 220, variante claviers asiatiques) au lieu de `SDL_SCANCODE_KP_PERIOD` (scancode 99, standard USB HID). Corrigé dans `graphics_backend.h`.
- **Curseur clignotant en saisie numérique** : un curseur clignotant s'affichait dans la zone de saisie numérique (chiffres, niveaux…) alors qu'il ne doit apparaître qu'en mode saisie de nom/texte (F5, `index_type == 1`). Le curseur est désormais masqué en saisie numérique pure.
- **Saisie de temps décimaux** : taper `1.1` donnait `1.01 s` (1 centième) au lieu de `1.10 s` (1 dixième). La partie décimale est maintenant normalisée selon sa longueur : 1 chiffre après le point = dixièmes (×10), 2 chiffres = centièmes. Exemples : `1.5` → 1.50 s, `1.05` → 1.05 s, `1.10` → 1.10 s.

### Cue list — protection des interactions

- **Drag du ratio X1/X2 — isolation des zones** : glisser le curseur de ratio entre les deux crossfaders pouvait accidentellement déclencher d'autres éléments (GO, GO BACK, DOUBLE GO, bouton TOGETHER, pastilles MIDI out X1/X2/Speed, ronds de raccrochage MIDI, fader Speed). Principe de correction : chaque élément interactif de la fenêtre cue list ne répond désormais qu'aux clics dont l'origine (bouton enfoncé) est dans sa propre zone. Un drag entrant de l'extérieur est ignoré.

  Mécanisme : deux globaux `mouse_click_x` / `mouse_click_y` enregistrent la position du dernier `MOUSE_FLAG_LEFT_DOWN` (dans MAIN.cpp). Chaque élément vérifie que le clic initial est dans sa zone avant de traiter l'événement. Le flag `seq_ratio_drag_active` protège en plus les boutons GO / GO BACK / DOUBLE GO / TOGETHER contre les drags démarrant dans la zone du curseur de ratio.

### Technique (sans impact visible direct)

- Compilation avec GCC 5.1.0 (MinGW portable dans `tools/`).
- Build 100 % portable par chemin relatif (`build.bat`).
- Intégration VSCode (build `Ctrl+Shift+B`, debug F5).
- **Signatures** : les 10 fichiers modifiés portent la signature `Jacques Bouault - arpschuino.fr - 2026` dans leur en-tête.
- **Indentation** : reformatage clang-format (4 espaces, style LLVM, col. 120) sur les 10 fichiers modifiés.
