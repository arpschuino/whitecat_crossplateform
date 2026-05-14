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
- [x] video_tracking_core.cpp
- [x] faders_core_24.cpp → faders_core.cpp
- [x] arduino_core_6_UNO.cpp → arduino_core.cpp
- [x] arduino_6_UNO.cpp → arduino_visu.cpp
- [x] CFG_config_panel_8.cpp → CFG_config_panel.cpp
- [x] midi_13.cpp → midi_functions.cpp
- [x] banger_core.cpp → banger_core.cpp / banger_core.h

### À vérifier : banger_core.cpp
- [ ] Bang "bang it" : clic sur le bouton → flash rouge des 6 ronds + action
- [ ] Bang depuis mémoire (do_send_bang) : rappel mémoire avec bang associé
- [ ] do_go_back_bang : retour au banger précédent
- [ ] Chrono_Reset : reset du chrono dans la fenêtre time
- [ ] do_loop_bang : banger en boucle → cycle continu sans blocage
- [ ] do_logical_fenetre_banger : ouvrir la fenêtre banger, interagir avec les boutons

### À vérifier : midi_functions.cpp (ex midi_13)
- [ ] Envoi MIDI CC depuis un fader (fader MIDI out)
- [ ] Réception MIDI CC → déplace un fader
- [ ] Note ON/OFF depuis launchpad → action correspondante
- [ ] simulate_midi() : déclencher une action MIDI simulée depuis menu
- [ ] clock_wheel : tempo MIDI ticker visible et fonctionnel
- [ ] do_midi_config : ouvrir la fenêtre de config MIDI, changer un port, sauvegarder

### À vérifier : faders_core.cpp (ex faders_core_24)
- [ ] Déplacer un fader à la souris → niveau circuit OK
- [ ] LFO sur fader : oscillation visible
- [ ] Lock preset : verrouiller/déverrouiller un fader
- [ ] Dock fader : affecter une mémoire à un fader

### À vérifier : arduino_core + arduino_visu
- [ ] Connexion Arduino sur port COM → données reçues (analog/digital)
- [ ] arduino_do_digital_out_whitecat : envoi digital vers Arduino
- [ ] do_arduino_config : affichage de la fenêtre de config Arduino

### Phases suivantes
- [ ] **Phase 5** — Font cache (Font::Print GPU, CPU élevé)
- [ ] **Phase 6** — Dirty rects
- [ ] **Phase 7** — Port Linux / Raspberry Pi
