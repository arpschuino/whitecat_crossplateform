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

### Phase 0 — Fondations (papier, pas de code)
- Figer la **taxonomie d'attributs** (intensité / couleur / position / faisceau / contrôle).
- Spécifier le **modèle de données** : `Channel/Parameter`, `Fixture`, `Patch (univers, canal, fine)`.
- Trancher les décisions ouvertes (voir plus bas).
- Valider le choix de la lib JSON.

### Phase 1 — Objet `Channel` (cœur)
- Classe sobre : valeur interne haute résolution, résolution sortie 8/16 bit (flag), courbe, mode HTP/LTP.
- **Cohabitation** : le `Channel` *wrappe* `grid_levels`/`Memoires` (ne remplace pas encore).
- 🎯 Fil rouge : « un circuit 8 bit s'allume exactement comme avant » (non-régression).

### Phase 2 — 16 bit + patch multi-univers + gradation haute résolution
- Patch étendu `(univers, coarse, fine?)`.
- Rendu DMX qui déplie 8/16 bit ; courbes passées en interne haute résolution (aujourd'hui en 256).
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

### Phase 5 — Contrôles dédiés
- **Color picker** (RGB/CMY), **Pan/Tilt** (pad/joystick), encodeurs d'attributs.

### Phase 6 — Intégration mémoires / séquentiel / chasers
- Mémoires stockant les **attributs de fixtures** (format save étendu + **rétrocompat `.whc`**).
- **LTP** dans les crossfades / masters.

### Phase 7 — Long terme
- Importeur **GDTF** (dézip `zlib` ✅ + XML `tinyxml2` ; ignorer 3D/photométrie).
- **Multi-univers Art-Net** complet. Éventuel visualiseur (réutiliserait la géométrie GDTF).

---

## ⚠️ Points de vigilance transversaux
- **Cohabitation** avec le cœur legacy (tableaux `grid_levels`/`Memoires` indexés par circuit) : wrapper
  d'abord, remplacer ensuite.
- **Rétrocompat du format de save** : les shows `.whc` existants doivent continuer à se charger.
- **Performance** : garder un buffer DMX plat au rendu (40 Hz).
- **Fil rouge** : un cas d'usage validé à chaque brique.

## ❓ Décisions à trancher en Phase 0
- Taxonomie exacte des attributs (liste + nommage).
- Format de profil prioritaire — **OFL d'abord** (acté), GDTF plus tard.
- 1 univers au départ, ou multi d'emblée dans les structures (adresse = `(univers, canal)` acté).
- Stratégie de migration des shows existants.

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
