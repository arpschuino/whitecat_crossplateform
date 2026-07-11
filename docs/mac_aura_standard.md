# MAC Aura — fixture de référence (mode Standard, 14 canaux)

Fixture d'essai pour le développement du moteur *devices* (têtes mobiles) de WhiteCat.
Carte lue **directement dans le GDTF**, pas d'après une table du web.

- **Fabricant / modèle** : Martin Professional — MAC Aura
- **Fichier GDTF** : `Martin_Professional@MAC_Aura@20230201NoMeas_MAWorkAround.gdtf`
- **Mode** : `Standard` — *« Washlight without control of Aura effect »*
- **Empreinte** : 14 canaux
- **Visualiseur de test** : BlenderDMX (voir la fiche mémoire BlenderDMX Art-Net)

## Carte des canaux (offset = adresse relative à l'adresse de patch)

| Canal | Attribut GDTF | Type | Notes |
|:-----:|---------------|------|-------|
| 1     | Shutter1      | Plages (ranges) | Patchwork ouvert / effets — voir détail plus bas |
| 2     | Dimmer        | Linéaire | 0 = fermé → 255 = plein |
| 3     | Zoom          | Linéaire | 0 = Wide 41,7° → 255 = Narrow 9° |
| 4 + 5 | **Pan**       | **16 bit** | canal 4 = poids fort (coarse), 5 = fin. Physique 270° → −270° |
| 6 + 7 | **Tilt**      | **16 bit** | canal 6 = poids fort (coarse), 7 = fin. Physique 116° → −116° |
| 8     | Control1 (Settings) | Plages | reset / fan / courbes de dimmer — **aucun rendu visuel** (normal) |
| 9     | Color1        | Roue virtuelle | color wheel virtuel (BeamColorWheel) |
| 10    | ColorAdd_R    | Linéaire | Rouge |
| 11    | ColorAdd_G    | Linéaire | Vert |
| 12    | ColorAdd_B    | Linéaire | Bleu |
| 13    | ColorAdd_W    | Linéaire | White |
| 14    | CTC           | Linéaire | Température de couleur (Color Temperature Control) |

> **Base lumineuse pour tester** : Shutter ouvert (voir ci-dessous) + Dimmer (canal 2) à fond
> + au moins une couleur (R/V/B canal 10/11/12, ou White canal 13) à fond.

## Canal 1 — Shutter (fonction à plages)

Le canal shutter **n'est pas un fader linéaire** : ses 0–255 sont découpés en bandes,
alternant des **îlots « Open »** et des **bandes d'effet** (strobe, pulse, burst).
Valeurs exactes du GDTF (`ChannelSet DMXFrom`) :

| Plage DMX | Fonction |
|-----------|----------|
| 0 – 19    | Fermé (Closed) |
| **20 – 24** | **Ouvert (Open) — valeur canonique = 22** (`Default`/`Highlight` du GDTF) |
| 25 – 64   | Strobe (Fast → Slow) |
| 65 – 69   | Ouvert |
| 70 – 84   | Opening Pulse (Fast → Slow) |
| 85 – 89   | Ouvert |
| 90 – 104  | Closing Pulse |
| 105 – 109 | Ouvert |
| 110 – 124 | Strobe Random |
| 125 – 129 | Ouvert |
| 130 – 144 | Opening Pulse Random |
| 145 – 149 | Ouvert |
| 150 – 164 | Closing Pulse Random |
| 165 – 169 | Ouvert |
| 170 – 184 | Burst Pulse |
| 185 – 189 | Ouvert |
| 190 – 204 | Random Burst Pulse |
| 205 – 209 | Ouvert |
| 210 – 224 | Sine Wave |
| 225 – 229 | Ouvert |
| 230 – 244 | Burst |
| 245 – 255 | Ouvert |

### Écart spec GDTF ↔ rendu BlenderDMX (observé)

- **Spec GDTF** : à **DMX 0 le shutter est fermé** (faisceau coupé).
- **BlenderDMX (observé)** : à **DMX 0 le faisceau est OUVERT**. BlenderDMX n'implémente
  visiblement pas la logique fermé/ouvert du shutter — il rend le faisceau selon le
  **Dimmer** (et la couleur) et « bricole » les bandes d'effet.
- **BlenderDMX ne simule pas** strobe / pulse / burst : dans ces bandes il ne clignote pas,
  il **coupe** le faisceau. D'où l'observation « ça coupe à certaines valeurs, jamais de strobe ».
- **Conséquence pratique** : sous BlenderDMX, ne pas se fier au shutter ; piloter la coupure
  par le **Dimmer (canal 2)**. Sur du **vrai matériel**, viser **DMX 22** pour un ouvert franc.

## Enseignements pour le modèle `wc::Fixture` / moteur devices

- **Pan et Tilt sont en 16 bit** (2 canaux chacun) → le pipeline 16 bit déjà réalisé sert
  directement aux têtes mobiles. Pan/Tilt = coarse + fine.
- Un canal = un **attribut typé** (Dimmer, Pan, Tilt, ColorAdd_R…) **+ des plages de
  fonctions** (DMXFrom/DMXTo → sens physique). `wc::Channel` porte déjà l'attribut ;
  il faudra lui ajouter la notion de **plages (ranges/ChannelSet)** pour les canaux non linéaires.
- Catégorie **control** (canal 8) : présente dans l'empreinte mais **non rendue** — à traiter
  comme un canal de gestion (souvent « maintenir N secondes »).
- C'est exactement ce que le **GDTF** encode : à terme, import GDTF → `wc::Fixture` (Phase 5).
