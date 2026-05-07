# WhiteCat — TODO

## Bugs / Fonctionnalités incomplètes

- [x] **Trichromie — bouton NUMERIC/DESIGNER sans effet**
  Origine : code original Christoph, jamais implémenté.
  `show_designer_list` est togglé mais la liste s'affiche toujours dans l'ordre numérique.
  - Mode NUMERIC = ordre par numéro de référence (ex: Lee 201, 202...)
  - Mode DESIGNER = ordre par teinte/famille de couleur (Ambers, Blues, Greens...) — voir https://leefilters.com/lighting/colour-effect-lighting-filters/
  Solution : charger un deuxième fichier `gel_list_designer_order.txt` trié par teinte, ou stocker un index de tri par teinte.
  Fichiers : `Src/trichro_visu2.cpp` (affichage l.462-484), `Src/trichro_core2.cpp` (logique l.606-616)
