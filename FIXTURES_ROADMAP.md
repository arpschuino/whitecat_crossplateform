# WhiteCat — Plan de travail : support des fixtures multi-paramètres

Document de conception et de suivi (synchronisé Nextcloud + GitHub — accessible depuis n'importe quel poste).
Créé le 2026-06-21. Cible : **après** la publication de la release 0.9.1.

---

## Objectif

Faire passer WhiteCat d'un pupitre **orienté circuits** (1 canal DMX = 1 niveau 0-255) à un pupitre
capable de piloter des **fixtures multi-paramètres** (lyres, LED RGBW, barres pixel…), à terme
**multi-univers**, « sans rien à envier aux grandes consoles ». **Pas pressé** : progression par
briques, sur une base de code stable.

---

## Principes directeurs (décisions d'architecture actées)

1. **Couche « devices » au-dessus des circuits existants** — pas de réécriture big-bang. Le moteur
   continue de produire un buffer DMX ; les fixtures sont une abstraction au-dessus. WhiteCat est en
   single-file legacy + modernisation cross-platform : on avance incrémentalement.

2. **Valeur interne en haute résolution dès le départ** — le `Channel` stocke sa valeur en float
   `0..1` (ou entier 16 bit) et a une **propriété « résolution de sortie » (8/16 bit)**. Le rendu DMX
   « déplie » en 1 octet (coarse) ou 2 (coarse+fine). Conséquence : le **16 bit n'est pas un type ni
   une étape**, juste un flag → la gradation ne se réécrit qu'une fois.

3. **L'univers fait partie de l'adresse dès maintenant** — patch en `(univers, canal, fine?)`, même
   si on n'utilise qu'un univers au début. Très pénible à rajouter après (patch + save + réseau).

3 bis. **Nombre de circuits CONFIGURABLE par l'utilisateur** — pouvoir avoir plus *ou* moins de
   512 circuits. Les **circuits** sont du contrôle **logique** (faders, mémoires), découplés des
   **outputs** physiques (sorties DMX, limitées à 512/univers) : leur nombre n'a donc pas de raison
   d'être plafonné à 512. Le modèle `Fixture`/`Channel` (composition + `vector`) est nativement
   dynamique ; corollaire : les tableaux legacy figés `[513]` (`MergerArray`, `Patch`, `curves`,
   faders…) devront passer en dimensionnement dynamique. Côté sortie, l'adresse `(univers, canal)`
   absorbe l'extension (plusieurs univers d'outputs).

4. **HTP/LTP porté par l'objet `Channel`** — WhiteCat est tout-HTP aujourd'hui (le plus fort gagne).
   Couleurs et positions exigent du **LTP** (le dernier gagne). Concept nouveau, à poser dès le channel.

5. **POO sobre** — `Channel` = classe simple ; `Fixture` **contient** un `vector<Channel>`
   (**composition, jamais héritage profond** type `Channel→ColorChannel→…`). Objets pour la logique
   et l'édition, mais **buffer plat au rendu** (boucle ~40 Hz, pas de vtables/indirections lourdes).

6. **Modèle interne neutre + importeurs-adaptateurs** — le moteur ne connaît QUE sa taxonomie
   d'attributs. Les formats de profils sont importés vers ce modèle :
   ```
   OFL  (JSON)  ─┐
                 ├─►  [importeur]  ─►  MODÈLE INTERNE (attributs)  ─►  moteur WhiteCat
   GDTF (zip/XML)─┘
   ```
   Ajouter un format = écrire un importeur, sans toucher au cœur. **OFL d'abord** (JSON simple,
   catalogue libre), **GDTF ensuite** (plus complet mais zip+XML ; sa richesse 3D/photométrie ne sert
   pas au contrôle, seulement à la pré-viz). Lib JSON pressentie : **`nlohmann/json`** (header-only).

7. **Fil rouge visible à chaque phase** — toujours un cas d'usage concret qui s'allume, pour ne jamais
   refactorer « à l'aveugle ».

---

## Phases

### Phase 0 — Fondations ✅ ACTÉ (2026-06-21, conception sans code)

**Taxonomie d'attributs** — nommage **canonique GDTF** (rend le mapping GDTF/OFL quasi trivial).
Organisée en groupes ; chaque attribut porte : groupe · combinaison **HTP/LTP** par défaut · valeur
**home** · type (`continu` = fadeable / `slot` = roue à plages).

| Groupe | Attributs | Combine | Type |
|---|---|---|---|
| Intensity | `Dimmer` | **HTP** | continu |
| Position | `Pan`, `Tilt` | LTP | continu (souvent 16 bit) |
| Color | `ColorAdd_R/G/B/W` (+ ext. `_A/_UV/_Lime/_Cyan/_Indigo…`), `ColorSub_C/M/Y`, `CTO/CTC` | LTP | continu |
| Color *(ext.)* | `Color1` (roue) | LTP | slot |
| Beam | `Shutter1`/`Shutter1Strobe`, `Iris`, `Zoom`, `Focus1` | LTP | continu |
| Beam *(ext.)* | `Gobo1`+`Gobo1PosRotate`, `Prism1`, `Frost1` | LTP | slot / continu |
| Control *(ext.)* | `Function`/`Control` (reset, lamp, fan…) | LTP | slot |

Stratégie « **figer large, implémenter étroit** » :
- **Vague 1** (Phases 1-3) : `Dimmer`, `ColorAdd_R/G/B/W`, `Pan`, `Tilt` — continus uniquement.
- **Vague 2** (Phase 5+) : `Zoom`/`Focus`/`Iris`/`Shutter`, puis le concept **slot** (roues + capabilities), puis `Control`.
- Palette couleur **listée large** dès maintenant (ce n'est qu'une liste), câblage R/G/B/W d'abord.
- Concept **slot** posé dans le modèle dès maintenant, **implémenté en Vague 2**.

**Modèle de données** — 3 objets en **composition**. Point clé : **un circuit classique = une
`Fixture` à un seul `Channel` `Dimmer`** → modèle unifié, le circuit est le cas le plus simple.
```
Channel { attribute;  value:uint16;  combine:HTP|LTP;
          universe; coarse_addr; fine_addr(=0 si 8 bit); resolution:8|16; curve }
Fixture { id; name; profile; base_universe; base_address; channels:Channel[] }
Patch   { fixtures[] }
```
Rendu : pour chaque `Channel` → `curve(value)` → écrit `coarse` (+`fine` si 16 bit) dans le
**buffer DMX plat** de l'univers. Logique dans les objets, hot path = balayage plat (perf 40 Hz).

**Décisions de représentation** :
- `value` interne = **`uint16`** (haute résolution **entière**, pas float). Raison : la résolution
  interne doit **dépasser la sortie** pour absorber la chaîne `mémoire × master × grand master ×
  courbe` sans **banding** ; `uint16` = 65 536 niveaux, couvre 8 bit (`value>>8`) et 16 bit (MSB/LSB),
  reste **léger** (cible Pi), et permet des **courbes en LUT entière**. Le `float` est réservé aux
  calculs couleur **ponctuels à l'édition** (jamais au stockage ni au rendu).
- **Courbe par `Channel`** (comme une courbe par circuit aujourd'hui), appliquée en interne via LUT.

**Fils rouges (validation incrémentale)** :
> circuit 8 bit *(non-régression)* → **dimmer 16 bit *(fade fin)*** → PAR RGBW *(couleur)*

**Lib JSON** : **`nlohmann/json`** (header-only, MIT) — parsing ponctuel de petits fichiers OFL,
à **isoler dans le TU de l'importeur** (gros header → hors PCH global). GDTF plus tard : XML `tinyxml2`.

*Reste de la Phase 0 réellement à faire avant de coder : rien de bloquant — on peut démarrer la
Phase 1 (objet `Channel`) quand 0.9.1 sera publiée.*

### Phase 1 — Objet `Channel` (cœur)
- Classe sobre : valeur interne haute résolution, résolution sortie 8/16 bit (flag), courbe, mode HTP/LTP.
- **Cohabitation** : le `Channel` *wrappe* `grid_levels`/`Memoires` (ne remplace pas encore).
- 🎯 Fil rouge : « un circuit 8 bit s'allume exactement comme avant » (non-régression).

### Phase 2 — 16 bit (cœur converti) + patch multi-univers + gradation haute résolution
**Décision actée (2026-06-24)** : on convertit **tout le cœur** en 16 bit (échelle interne 0..65535)
plutôt qu'un chemin parallèle. Le 8 bit devient un simple **cas de sortie** (`niveau >> 8`).
Facteur d'échelle = **257** (255 × 257 = 65535) → round-trip 8 bit exact, donc non-régression
**bit-exacte** possible à chaque étape.

- **2a** ✅ `render()` déplie le 16 bit (MSB sur `coarse_addr`, LSB sur `fine_addr`).
  *(channel.h — testé hors-ligne ; pas encore activé : tous les outputs sont en 8 bit)*
- **2b** — **patch 16 bit** : marquer une paire d'outputs (coarse + fine) comme un canal 16 bit.
- **2c** — **conversion du cœur en 16 bit** (le gros morceau), en sous-étapes non-régressives :
  - **2c-0** *Cartographie* — recenser les buffers de niveau (`MergerArray`, `bufferFaders`,
    `bufferSequenciel`, `freeze_state`, Grand Master…) et les `255` en dur (distinguer « niveau »
    d'« octet DMX »).
  - **2c-1** *Échelle + helpers* — `LVL_MAX = 65535`, `lvl→dmx8 (v>>8)`, `dmx8→lvl (d×257)`,
    `lvl↔%`. Aucun changement de comportement.
  - **2c-2** *Pipeline de rendu en `uint16`* — buffers + Merger (`Tmax`, Grand Master) en 0..65535 ;
    sortie 8 bit = `lvl>>8`, output 16 bit = `lvl` complet. 🎯 Non-régression : même DMX 8 bit qu'avant.
  - **2c-3** *Sources fines* — crossfade GO **interpolé** en 16 bit (le gain visible), LFO 16 bit,
    faders stockés ×257.
  - **2c-4** *Périphérie* — save/load (anciens shows 0-255 → ×257 à la lecture), UI (% depuis 16 bit) ;
    Art-Net déjà couvert par le rendu.
- **2d** — **courbes haute résolution** (interpolation de la LUT 8 bit, ou LUT 16 bit).
- 🎯 Fil rouge : « un canal 16 bit fait un fade fin et lisse ».

### Phase 3 — Fixtures (groupement d'attributs)
- Classe `Fixture` = `vector<Channel>` + référence de profil (composition).
- 🎯 Fil rouge : « un PAR RGBW change de couleur ».

### Phase 4 — Import Open Fixture Library
- Importeur **OFL → taxonomie**, sous-ensemble : Intensity, ColorIntensity (R/G/B/W/A…), Pan/Tilt,
  ShutterStrobe, coarse/fine, gestion des **modes** DMX.
- À ignorer au début : wheels à plages complexes, switching/virtual channels, matrices de pixels.
- Bibliothèque de profils + UI de **patch fixtures**.
- ⚠️ Le vrai travail n'est pas le parsing mais le **mapping** vers la taxonomie + les cas tordus.

### Phase 5 — Contrôles dédiés & UI ergonomique
- **Color picker** (RGB/CMY), **pad XY** Pan/Tilt (souris + manette), **banque d'encodeurs** d'attributs.
- **Palettes** (positions / couleurs / gobos / beams) référencées par les mémoires.
- **Sheets configurables** (Channel Views étendues → vue par fixture / par attribut).
- OSC en entrée (télécommandes, surfaces tactiles). Détail : voir section « Interface utilisateur ».

### Phase 6 — Intégration mémoires / séquentiel / chasers
- Mémoires stockant les **attributs de fixtures** (format save étendu + **rétrocompat `.whc`**).
- **LTP** dans les crossfades / masters.

### Phase 7 — Long terme
- Importeur **GDTF** (dézip `zlib` ✅ + XML `tinyxml2` ; ignorer 3D/photométrie).
- **Multi-univers Art-Net** complet. Éventuel visualiseur (réutiliserait la géométrie GDTF).

---

## Interface utilisateur (transversale — pas une phase finale)

**Principe directeur — UI native souris/clavier/écran, ne PAS émuler une console physique.**
WhiteCat est un logiciel : pas de touches dédiées (`[Params]`, `[Data]`… comme sur Eos), donc on évite
les combinaisons « modificateur maintenu + touche » pensées pour un hardware. À la place : **toggles
directs** (un clic affiche/masque), **menus contextuels** (clic droit sur en-tête de colonne),
**glisser-déposer**, et surtout des **vues sauvegardées** (sheets nommées rappelables d'un clic/raccourci
— on *prépare* puis on *rappelle*, souvent plus rapide en exploitation qu'une reconfiguration à la volée).
C'est un avantage, pas une contrainte : aucun layout de console figé.

L'UI accompagne **chaque** fil rouge : deux niveaux selon le stade.
- **UI « de travail » brute** (avec les Phases 1-3) : des **faders bruts par attribut** suffisent à
  valider le moteur (RGBW = 4 faders, Pan/Tilt = 2 faders). Moche mais fonctionnel.
- **UI ergonomique** (Phase 5+) : color picker, pad XY, galeries de slots, palettes — là où WhiteCat
  « ne doit rien envier aux grandes ».

**Paradigme central : `sélection → attribut → valeur`.** WhiteCat en a déjà la moitié : la sélection
(clavier numérique, thru, +/−) et un buffer (`bufferSaisie` = germe de **programmer**). Le seul ajout
structurant est la **couche « attribut »** (choisir quel paramètre on édite) entre la sélection et le
réglage. Aujourd'hui il n'y a qu'un attribut implicite (l'intensité).

**Surfaces d'entrée** (par ordre d'arrivée) :
1. **Souris + clavier** (base).
2. **MIDI** — déjà présent : mapper des **encodeurs/Launchpad** aux attributs (gros atout existant).
3. **OSC** — à terme (télécommandes, surfaces tactiles, inter-logiciels).
4. **Hardware dédié** (encodeurs physiques pour Raspberry Pi) — long terme.

**Widgets de valeur, par groupe d'attribut** :
| Groupe | Contrôleur | Réutilise |
|---|---|---|
| Intensity | fader / niveau | espace circuits actuel |
| Color | **color picker** (roue/carré HSV) + faders RGBW + **palettes** | trichromie (embryon à faire grandir) |
| Position | **pad XY 2D** (souris + manette/joystick) | — (W_MOVER retiré = retour d'expérience) |
| Beam (zoom/focus/iris) | banque d'**encodeurs** virtuels | — |
| Slot (gobo/color wheel) | **galerie de vignettes** | — (Vague 2) |

**Vues d'état — extension des Channel Views.** Les Channel Views deviennent des **« sheets »
configurables** (tableau *fixtures × attributs*, colonnes masquables) → on obtient avec **un seul
mécanisme** la **vue par fixture** (toutes les colonnes) ET la **vue par attribut** (une colonne isolée :
« tous les Tilt ensemble » pour aligner / repérer une déviation). Config par **toggles / menu contextuel /
vues sauvegardées** (cf. principe directeur). Liberté qu'Eos n'a pas : masquer aussi des **catégories
entières** (Intensity/Color/Beam) pour une vue vraiment épurée.

**Palettes — essentiel, à porter tôt.** Presets réutilisables par groupe d'attributs (positions,
couleurs, gobos, beams), référencés par les mémoires (une mémoire pointe une palette → modifier la
palette met à jour toutes les mémoires qui l'utilisent). Le **modèle de palette est à prévoir dès le
modèle de données** (Phase 0/3), l'UI suit en Phase 5.

## ⚠️ Points de vigilance transversaux
- **Cohabitation** avec le cœur legacy (tableaux `grid_levels`/`Memoires` indexés par circuit) : wrapper
  d'abord, remplacer ensuite.
- **Rétrocompat du format de save** : les shows `.whc` existants doivent continuer à se charger.
- **Performance** : garder un buffer DMX plat au rendu (40 Hz).
- **Fil rouge** : un cas d'usage validé à chaque brique.

## ✅ Décisions Phase 0 — tranchées (2026-06-21)
- Taxonomie d'attributs **figée**, nommage **GDTF** canonique (voir détail Phase 0).
- Format de profil : **OFL d'abord**, importeur **GDTF remonté après Phase 4** (couverture).
- Adresse = `(univers, canal)` dès maintenant ; 1 univers utilisé au départ, structures prêtes multi-univers.
- `value` interne = **`uint16`** ; **courbe par `Channel`** ; lib JSON = **`nlohmann/json`**.
- **Séquentiel : cue-only d'abord** (mémoires = états complets, comme aujourd'hui). Le **tracking**
  reste une évolution possible *plus tard* (couche de calcul par-dessus, sans changer la structure de
  mémoire) → porte non fermée. Le tracking brille surtout sur les longues conduites ; le public
  WhiteCat (théâtre/danse) s'accommode bien du cue-only.
- **Encore ouvert** : modèle de mixage **couches + programmer prioritaire** (HTP intensité / LTP
  couleur-position) — nécessaire *indépendamment* du cue-only (à confirmer/concevoir, cf. Phase 6) ;
  stratégie de migration / rétrocompat des shows `.whc` existants.

## Comparatif des formats de profils (rappel)

| | GDTF | OFL |
|---|---|---|
| Canaux / modes / 8-16 bit / plages | ✅ | ✅ |
| Rôles d'attributs (dimmer, RGB, pan/tilt…) | ✅ | ✅ |
| Géométrie 3D, modèles physiques (pré-viz) | ✅ | ❌ |
| Photométrie (faisceau, émetteurs couleur) | ✅ | partiel |
| Format / accès | zip + XML / GDTF-Share (compte) | JSON / web libre |
| **Couverture du catalogue** (juin 2026) | **> 7 000** fixtures (fabricants publient à la source) | **621** fixtures (communautaire) |
| Verdict pour le **contrôle** WhiteCat | format surdimensionné, mais **catalogue ~10× plus fourni** | format idéal pour prototyper, catalogue plus modeste |

**Nuance stratégique — couverture du catalogue.** OFL reste le **meilleur point de départ** (JSON
simple) pour bâtir et valider tout le pipeline d'import. Mais pour la **valeur à l'utilisateur final**
(« trouver SON projecteur »), GDTF est **~10× plus fourni** (> 7 000 contre 621 en juin 2026, car les
fabricants y publient officiellement). → **Remonter l'importeur GDTF plus tôt** que la stricte Phase 7,
dès que le modèle interne est stabilisé (≈ après Phase 4). Le modèle neutre + double importeur rend ce
glissement indolore.
