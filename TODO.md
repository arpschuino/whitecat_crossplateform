# WhiteCat — TODO

## Bugs / Fonctionnalités incomplètes

- [ ] Réorganiser la fenêtre MENUS (Call_everybody_5.cpp → Menus()) : Freeze et Exclude retirés, Help retiré → revoir la mise en page des colonnes restantes

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
- [x] GO BACK : retour à la mémoire précédente OK
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
- [x] Bang "bang it" : clic sur le bouton → flash rouge des 6 ronds + action
- [x] Bang depuis mémoire (do_send_bang) : rappel mémoire avec bang associé
- [ ] do_go_back_bang : retour au banger précédent
- [x] Chrono_Reset : reset du chrono dans la fenêtre time
- [ ] do_loop_bang : banger en boucle → cycle continu sans blocage
- [x] do_logical_fenetre_banger : ouvrir la fenêtre banger, interagir avec les boutons (hold-to-scroll ajouté sur ++/--, type, action)

### À vérifier : midi_functions.cpp (ex midi_13)
- [x] Envoi MIDI CC depuis un fader (fader MIDI out)
- [x] Réception MIDI CC → déplace un fader
- [x] Note ON/OFF depuis launchpad → action correspondante
- [ ] simulate_midi() : déclencher une action MIDI simulée depuis menu
- [ ] clock_wheel : tempo MIDI ticker visible et fonctionnel
- [x] do_midi_config : ouvrir la fenêtre de config MIDI, changer un port, sauvegarder

### À vérifier : faders_core.cpp (ex faders_core_24)
- [x3333] Déplacer un fader à la souris → niveau circuit OK
- [x] LFO sur fader : oscillation visible (testé — montée/descente + cyclique)
- [ ] Lock preset : verrouiller/déverrouiller un fader
- [x] Dock fader : affecter une mémoire à un fader

### À vérifier : arduino_core + arduino_visu
- [ ] Connexion Arduino sur port COM → données reçues (analog/digital)
- [ ] arduino_do_digital_out_whitecat : envoi digital vers Arduino
- [ ] do_arduino_config : affichage de la fenêtre de config Arduino

### À vérifier : validation banger val1/val2 (banger_core.cpp)
- [x] Fader (type 1) : val1 = 1 à core_user_define_nb_faders → popup si dépassement
- [ ] MIDI (type 2) : val1 = Ch.Midi 0-15 (actions 0-4) ou 0-127 (actions 5+) ; val2 = 0-127 ou 0-1 selon action → popup
- [ ] Windows (type 3) : val1 ≤ 128, val2 = 0-1440
- [x] Alarm (type 4) : val2 = 0-1
- [ ] Audio (type 5) : val1 = 0 à index_nbre_players_visibles ; val2 = 0-126 (fichier, actions 1/3), 0-127 (vol/pan/pitch), 0-1 (autres) — **vérifier borne haute fichier (0 ou 1 indexé ?)**
- [x] Sequences (type 6) : val1 ≤ 999
- [ ] Chasers (type 7) : val1 = 1 à core_user_define_nb_chasers
- [ ] MiniFaders (type 8) : val1 ≤ core_user_define_nb_faders, val2 = 0-1
- [ ] Chrono (type 10) : val1 = 0-2
- [ ] SetBanger (type 12) : val1 ≤ 128, val2 = 0-1
- [ ] MidiMute (type 13) : val1 ≤ core_user_define_nb_faders, val2 = 0-1
- [ ] GridPlayer (type 14) : val1 ≤ 127
- [ ] Hardware (type 15) : val1 = 0-1, val2 = 0-50 (port COM)
- [ ] Draw (type 17) : val1 = 1-6

### Phases suivantes
- [x] **Ticker intelligent** — wc_request_refresh() ciblé (LFO/chasers/GO/dampers) + cap 3 niveaux (60/25/idle fps)
- [x] **Correction PCH Makefile** — graphics_backend.h ajouté comme dépendance du PCH
- [x] **wc_cache global WC_SKIP_GLOBALS** — rendu texte restauré dans tous les TUs
- [x] **Phase 5** — Font cache 4-way set-associatif LRU (2048 slots, éviction LRU par SDL_GetTicks)
- [ ] **Phase 6** — Dirty rects
- [ ] **Phase 7** — Port Linux / Raspberry Pi
