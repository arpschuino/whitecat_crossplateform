# WhiteCat — Changelog

## Version 0.10 (en cours — Jacques Bouault)

### Gradation 16 bits (préparation fixtures)

- **Cœur de gradation converti en 16 bits.** Tout le pipeline de niveaux (saisie, blind, séquentiel, faders, master) travaille désormais en haute résolution interne (0 → 65535) au lieu de 0 → 255. Le 8 bits devient un simple cas de sortie (octet de poids fort). Conséquence : les **circuits 8 bits classiques sont strictement inchangés** (non-régression à l'identique), et on peut piloter des canaux **16 bits réels** (coarse + fine) pour des fondus fins et lisses.
- **Patch 16 bits** : le bouton *Patch 16 bit* (fenêtre Patch) associe une paire de sorties (coarse + fine) à un même circuit. La sortie déplie alors le niveau sur deux canaux DMX. Ce patch 16 bits est **enregistré dans le show** et rechargé tel quel (y compris à la fermeture).
- **Modèle de patch « appareils » (fixtures)** : en interne, le patch est désormais décrit comme une liste d'appareils, chacun portant un ou plusieurs paramètres (un dimmer = un appareil à un paramètre *Intensity*, en 8 ou 16 bits, HTP). C'est la fondation du futur support multi-paramètres (lyres, LED RGBW…). Le patch est sauvegardé sous cette forme (`patch_fixtures.whc`, texte lisible) ; les shows antérieurs (patch « à plat ») restent rechargeables.
- **Fix : reset du show** — le reset remet maintenant les 16 **courbes droites** par défaut (auparavant les formes de courbes du show précédent persistaient) et **nettoie les appariements 16 bits** (plus de canal *fine* résiduel après un Clear / Default / nouveau show).
- **Saisie** : une valeur entrée en **%** ou en **DMX (0-255)** produit un vrai niveau 16 bits (ex. `51 %` → coarse 130 / fine 143). L'affichage en % du niveau circuit est désormais exact (plus d'arrondi 8 bits).
- **Contrôle fin** : **Ctrl maintenu + molette ou flèches haut/bas** ajuste le niveau au pas fin (1/65535), pour régler l'octet faible d'un canal 16 bits sans bouger l'octet fort. Sans Ctrl, la molette/les flèches gardent le pas grossier habituel (%/DMX).
- **Compatibilité des shows préservée** : les mémoires, grids, chasers, docks de faders et freeze restent stockés en 8 bits (conversion automatique aux frontières). Les shows `.whc` existants se chargent à l'identique, et un show enregistré reste relisible par une version antérieure.
- **Crossfade des grid players en 16 bit** : les transitions entre pas (fondu temporisé) sont calculées et sorties en 16 bit — interpolation par la position de fondu *lisse* (au lieu de 256 paliers) et sortie directe sans re-quantification 8 bit. Plus de paliers visibles sur les fondus de grid players, que la sortie passe par un fader, un draw player (source GPL) ou un snap banger.
- **Grids : allocation mémoire paresseuse (préparation 16 bit des pas)**. Les niveaux des grilles (`grid_levels`, 128 grilles × 1024 pas × 513 canaux) étaient alloués d'un seul bloc au démarrage (~67 Mo), même avec une ou deux grilles utilisées — soit ~134 Mo une fois les pas passés en 16 bit, notable sur Raspberry Pi. Désormais chaque grille est allouée **à la demande**, à sa première case non nulle ; une grille vide n'occupe **aucune mémoire**. Sauvegarde/rechargement, effacements et vidage de grille n'allouent que les grilles réellement utilisées. Le crossfade (boucle temps réel 50 Hz) et le save/load utilisent des pointeurs de bloc mis en cache — **aucun surcoût de performance** (la boucle chaude est même allégée). Ce prérequis rend le passage des pas de grille en 16 bit indolore côté RAM.
- **Pas de grille en 16 bit** : les niveaux stockés des pas (`grid_levels`) passent de 8 bit à **16 bit natif** (0 → 65535). Captures (saisie, blind, faders, séquentiel, mémoire affectée, snap fader) enregistrées sans troncature 8 bit ; le crossfade consomme directement du 16 bit. **Édition case par case** en 16 bit : clic = plein (65535), `nombre` + clic = %/DMX (le % donne une vraie valeur 16 bit), et **Ctrl + molette sur la case survolée** = retouche **fine** (pas de 1/65535), cohérente avec le contrôle fin des circuits. Un circuit dans une grille peut donc porter une valeur 16 bit précise — indispensable pour les futures positions de têtes mobiles. Compatibilité shows : nouveau format `grids_levels16_*.whc` (16 bit) ; les shows antérieurs (grilles 8 bit) sont rechargés et convertis automatiquement (×257).

- **Wizard mémoires en 16 bit** : les opérations Set / Add / Reduce sur les niveaux des mémoires travaillent désormais en 16 bit natif. Le **Set en %** passe par `pct_to_lvl` (vraie valeur 16 bit, plus d'arrondi `×2.55`) ; **Add / Reduce** ne repassent plus le niveau courant par du 8 bit avant le calcul → la finesse est **conservée** (ajouter 10 % à un circuit à 56,4 % ne le ramène plus d'abord à 56 %). Clamps 0 → 65535.

### Interopérabilité — import/export ASCII (USITT / ETC Eos)

- **Import de shows Eos.** WhiteCat lit désormais les fichiers USITT ASCII produits par ETC Eos (et compatibles) : mots-clés en TitleCase (`Cue`, `Up`, `Down`, `Chan`) lus insensiblement à la casse, lignes de cue **indentées** gérées, temps de transfert récupérés. Auparavant WhiteCat ne relisait que son propre format.
- **Niveaux 16 bit à l'import.** La notation de niveau `@H<hex>` est lue à **largeur variable** : 2 chiffres = 8 bit (×257), **4 chiffres = 16 bit** (ex. `5@H30a4` = 12452). WhiteCat lit les deux lignes d'Eos — `$$ChanMove` (pleine résolution 16 bit, prioritaire) et `Chan` (état 8 bit standard) — et conserve le 16 bit là où Eos le fournit.
- **Export lisible par Eos.** En-tête USITT correct (`Ident 3:0`), et par cue **deux lignes** comme Eos : `$$ChanMove` en 16 bit (4 chiffres hex) + `Chan` en 8 bit standard (lu par toute console). L'aller-retour WhiteCat → ASCII → WhiteCat préserve le **16 bit exact**. *(L'ASCII reste un format d'intensité ; le 16 bit des paramètres de têtes mobiles — `$$Param` — et le patch de fixtures sont un chantier ultérieur.)*
- **Patch dimmers importé dans le modèle Fixture.** Le patch lu en ASCII (Eos comme Cobalt/Congo) est désormais capté dans le modèle *Fixture* (source de vérité) via `synthesize_fixtures_from_legacy()`, puis les caches dérivés sont reconstruits — dont la liste « premier gradateur par circuit » (`generate_channel_preview_patch_list()`). Fini l'incohérence où *[show first dimmer]* affichait un patch droit juste après l'import et ne se corrigeait qu'après redémarrage.
- **Masters Congo → docks de faders.** Import human-centric des masters AVAB/Congo (`$$MASTPAGEITEM` + lignes `CHAN`) : la **page** devient le **dock** et l'**item** le **fader** (page 1 → dock 1 des faders 1-40, page 2 → dock 2, etc.). Les niveaux sont convertis en 8 bit pour le contenu des docks. *(Le patch des **devices** multi-paramètres — `$$CHANNELDATA` / `$$DEVICEMODEPAGE` — reste un chantier Fixtures ultérieur.)*
- **Patch Eos : casse insensible + table rase.** Les mots-clés du patch Eos en TitleCase (`Patch 1 …`, `Clear All`) sont désormais lus (avant, seuls les `PATCH`/`CLEAR PATCH` en majuscules de Cobalt/Congo passaient). Et `Clear All`/`CLEAR PATCH` remet à zéro **tout** le patch avant l'import (auparavant seuls les outputs *sélectionnés* étaient vidés — rien à l'import) : plus de patch droit fantôme sous les circuits non listés (devices, trous).
- **Dimmers 16 bit importés.** Lecture de la section fixtures Eos (`$Personality` + `$Patch`) limitée aux **dimmers intensité purs** : une personality à une seule `$$PersChan` param 1 en *size 2* (ex. « Dimmer_16B ») est patchée en 16 bit (coarse + fine appariés, comme le bouton « Patch 16 bit »). Les **devices** (personalities multi-paramètres) restent ignorés → chantier Fixtures. Les dimmers 8 bit continuent de passer par le patch conventionnel.
- **Noms de mémoires propres à l'import (Eos + Congo).** Trois corrections sur les labels de cue lus en ASCII : (1) copie à **longueur exacte** — fini le petit rectangle en fin de label (c'était le `\n` embarqué) ; (2) **accents corrects** — le texte Latin-1 d'Eos (ex. « mémoire ») est converti en UTF-8, et l'UTF-8 déjà présent (aller-retour WhiteCat) est laissé tel quel, sans double-encodage ; (3) **plus de nom fantôme** — `flagcue` est coupé sur chaque ligne vide (fin de bloc), donc un `Text` d'une section suivante (effets, palettes) n'est plus attribué par erreur à la dernière mémoire, et chaque mémoire naît sans nom sauf `Text` explicite.

### Corrections (echo, faders, circuits, grid players)

- **Echo** : fin des saccades d'affichage (le rendu reste actif tant qu'un echo rebondit) + animation en **16 bit** (sortie directe depuis `echo_levels`, plus de paliers).
- **Faders / Flash** : le fader redescend immédiatement à l'écran au relâchement (le rendu reste actif quelques frames le temps que le niveau soit restauré) ; le flash n'est plus **lissé par le damper** (montée et descente instantanées).
- **Damper** : atteint enfin 0 (accrochage exact à la cible — la glisse exponentielle était asymptotique) + initialisation manquante qui pouvait le figer.
- **Espace circuits (vue classical)** : le 1er circuit de chaque page (1-12, 49-60…) n'est plus caché sous la barre Ch.View (auto-scroll recalé).
- **Grid players** : fin des saccades d'affichage (rendu actif tant qu'un player tourne) ; en-tête réagencé — titre « Grid Players » sur une seule ligne, champs (Beg.Chan, Col, Rows, edit, View…) décalés à droite ; ligne Grid/Step du global viewer recadrée (ne touche plus le bord gauche).
- **Grid players — fix plantage (thread ticker)** : dans `gridder_prepare_cross()` (calcul du fondu, 50×/s), les indices `grid_in_preset` (grille/pas du pas suivant) pouvaient sortir des bornes (goto avec `grid_goto` corrompu passant le test `>= 0` sans borne haute, séquentiel, ou `pas+1 = 1024`) → accès `grid_times`/`grid_levels` hors bornes → plantage (access violation), **d'autant plus en 16 bit** (débordement de bloc). Indices bornés (grille 0-127, pas 0-1023) ; 1re occurrence loguée dans `wc_debug.txt`. *(Même correctif backporté en 0.9.2.)*
- **Rapport save/reload** : la ligne « audio_conf.txt readed » ne s'affiche plus en rouge à tort (slot de rapport partagé).
- **Bangers — vestige iCat retiré** : la fenêtre iCat ayant été supprimée, la **catégorie « iCat »** (et son action « iCAT Builder » dans la catégorie Windows) subsistait dans le sélecteur d'événements banger. Catégorie rendue inatteignable (défilement haut/bas + chargement) et code mort retiré.

### Interface

- **Navigateur import/export corrigé** : le contenu des sous-dossiers (`ascii`, `pdf`…) ne s'affichait pas — le chemin de scan pointait sur le dossier lui-même au lieu d'y descendre. Corrigé (Windows + Linux/macOS). L'entrée « remonter d'un dossier » est désormais une **petite flèche retour** (au lieu du `..` peu lisible), synthétisée à l'identique sur toutes les plateformes ; l'entrée système `.` est masquée. Choisir un élément dans la liste annule proprement une saisie de nom en cours.
- **Noms éditables au double-clic** (nom de show — Save *Binary* et *Import/Export* —, nom de **banger**, nom de **grid player**, nom de **chaser** et de ses **pistes**, et progressivement d'autres champs) : le nom des pistes de chaser était même **introuvable** avant (il fallait F5 + clic sur le bouton ON en mode edit) — il est maintenant **découvrable** par double-clic direct sur le nom. double-cliquer sur un champ de nom permet de le saisir **directement**, sans passer par la fenêtre texte F5 (curseur blanc, flèches, backspace, UTF-8) ; Entrée valide, Échap annule, un clic sur le bouton d'action valide aussi le nom en cours. Repose sur un **composant d'édition inline réutilisable** (`wc_inline_edit`) : ajouter cette saisie directe à un nouveau champ tient en trois lignes, sans code dupliqué.
- **Fenêtre banger — cohérence visuelle** : la boîte de nom passe du rouge (couleur de survol, permanente) au **bleu** standard des champs ; les repères de ligne de la grille (1, 9, 17…) passent en **blanc** (au lieu du rouge).
- **Fenêtre principale redimensionnable à la souris** : on peut désormais agrandir / rétrécir la fenêtre en tirant ses bords. L'interface garde ses coordonnées fixes — agrandir donne plus d'espace vide autour, rétrécir rogne le bas / la droite. *(Sous Windows, le contenu peut apparaître étiré pendant le glissement de bordure — boucle de redimensionnement modale de l'OS qui met le rendu en pause — puis redevient net au relâché ; sous Linux/X11 c'est en principe plus fluide.)*

---

## Version 0.9.2 (en cours — Jacques Bouault)

### MIDI — Sortie (OUT)

- **Fix : sélection du périphérique MIDI OUT impossible** dès qu'un périphérique MIDI IN était présent. Dans la fenêtre de configuration MIDI, le clic posait son indicateur à `do_connect_out[j + compt_midi_in]` (et lisait l'état au même décalage), alors que l'ouverture du port lit `do_connect_out[j]`. Vestige de l'époque MidiShare (liste IN+OUT commune) ; RtMidi indexe IN et OUT séparément. Le décalage `+compt_midi_in` est retiré.
- **Fix : Control Change jamais émis en sortie**. `emit_midi_out()` envoie les faders avec le type MIDI `miditable[0]` (convention WhiteCat : `4` = Control Change), mais `midi_backend_send()` codait le CC en `case 3` sans `case 4` → les CC tombaient dans `default` et n'étaient jamais envoyés. CC géré à `case 4` (les Notes, types 1/2, fonctionnaient déjà).
- *Note : sous Windows, un port MIDI créé à chaud (ex. loopMIDI) après le lancement n'apparaît qu'au redémarrage — limite de l'API WinMM, pas un bug WhiteCat. Créer/brancher les ports avant de lancer WhiteCat.*

### Banger — Bouton LOOP

- **Fix : la mise en boucle ne fonctionnait pas**. L'intervalle de relance utilisait un facteur `×10000` hérité du timer Allegro : à 50 Hz, un réglage de « 2 s » donnait ~400 s avant relance. Corrigé en `×50` (`do_loop_bang()` tourne à 50 Hz → `time_loop_banger` est réellement en secondes).
- **Fix : affichage figé pendant une boucle**. Le « ticker intelligent » (cap fps d'économie CPU) détecte les LFO/chasers/GO/dampers mais pas les bangers : un banger en boucle laissait WhiteCat passer en veille d'affichage → les événements paraissaient désynchronisés/manquants **à l'écran** (la sortie DMX/MIDI restait correcte ; bouger la souris « réparait »). Les bangers en boucle sont ajoutés à la détection d'activité du rendu. Aucun surcoût au repos.

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
