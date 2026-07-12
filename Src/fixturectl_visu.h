#pragma once

// [devices] Rendu et logique de la fenetre "Control Fixtures" (W_FIXTURECTL).
// fixturectl_window        : dessine la coquille (appele depuis graphics_rebuild1.cpp)
// do_logical_fixturectl    : gestion des clics/interactions (appele depuis procs_visuels_rebuild1.cpp)

int fixturectl_window(int xf, int yf);
int do_logical_fixturectl(int xf, int yf);

// [devices] Applique un delta (16 bit) a l'attribut <attr> de tous les devices selectionnes.
// Appele par le drag (fixturectl_visu) et par la molette (DoMouseLevel, channels_core.cpp).
void fxc_apply_delta(unsigned char attr, int delta);
