# WhiteCat — TODO

## Bugs / Fonctionnalités incomplètes



---

## Phase 4 — vérifications post-extraction TU

### À vérifier : procs_visuels_rebuild1.cpp
- [x] Clic sur la croix (X) d'une fenêtre → ferme bien la fenêtre
- [x] Clic sur le bouton M → déplace bien la fenêtre
- [x] `detection_over_window` : passer la souris sur chaque fenêtre ouverte → focus correct
- [x] Déplacer plusieurs fenêtres différentes (trichromie, séquentiel, patch…)
- [x] `logical_channelspace` : scroll circuits en vue classique et en vue preset
- [x] `check_graphics_mouse_handling` : vérifier que les interactions souris fonctionnent dans chaque fenêtre ouverte (faders, banger, audio, grider, chasers…)

### À vérifier : dmx_functions.cpp (ex dmx_functions_13)
- [x] Sortie DMX ArtNet : les circuits s'allument correctement
- [ ] Sortie DMX Enttec Open : idem (si interface dispo)
- [x] Sortie DMX Enttec Pro : idem (si interface dispo)
- [x] Crossfade GO : le passage de mémoire se fait bien (niveaux, timing)
- [xi] GO BACK : retour à la mémoire précédente OK
- [x] LFOs : un fader avec LFO actif monte/descend correctement
- [x] Merger : HTP faders + séquentiel → résultat correct sur les circuits
- [ ] DMX IN (Enttec Pro IN ou Sunlite) : réception et affectation à un dock

### Phase 4 — fichiers TU restants à extraire
- [x] saves_export_pdf2.cpp
- [x] save_show.cpp (ex save_show_13.cpp)
- [x] logicals_intres.cpp
- [x] wizard.cpp
- [x] video_tracking_core.cpp  ← fait session précédente

### Phases suivantes
- [ ] **Phase 5** — Font cache (Font::Print GPU, CPU élevé)
- [ ] **Phase 6** — Dirty rects
- [ ] **Phase 7** — Port Linux / Raspberry Pi
