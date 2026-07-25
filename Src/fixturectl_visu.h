#pragma once

// [devices] Rendu et logique de la fenetre "Control Fixtures" (W_FIXTURECTL).
// fixturectl_window        : dessine la coquille (appele depuis graphics_rebuild1.cpp)
// do_logical_fixturectl    : gestion des clics/interactions (appele depuis procs_visuels_rebuild1.cpp)

int fixturectl_window(int xf, int yf);
int do_logical_fixturectl(int xf, int yf);

// [devices] Applique un delta (16 bit) a l'attribut <name> (nom GDTF ; "\x01" = Int) de tous les
// devices selectionnes. Appele par le drag (fixturectl_visu) et la molette (channels_core.cpp).
void fxc_apply_delta(const char* name, int delta);

// [devices] Menu de modes (dropdown) : true si un menu est ouvert. Le menu reste DANS la fenetre
// (qui s'agrandit automatiquement pour le loger), donc les clics sont captes normalement.
bool fxc_dropdown_open();
// Defile le menu ouvert a la molette (delta>0 = vers le haut). true si consomme (channels_core.cpp).
bool fxc_dropdown_wheel(int delta);
