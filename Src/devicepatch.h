#pragma once

// [devices] Fenetre "Patch a device" (W_DEVICEPATCH) : recherche d'une fixture dans la bibliotheque
// locale, choix du mode DMX, circuit/adresse/quantite/offset (auto-adressage editable), puis PATCH.
// Phase 1 = offline (bibliotheque embarquee, ~30 fixtures + generiques). Cf. fixtures-patch-model.
// Modele de cablage : la fenetre GRID / Control Fixtures.

extern bool index_window_devicepatch;   // fenetre ouverte (1) / fermee (0)
extern int  devicepatch_window_x;
extern int  devicepatch_window_y;

extern const int devicepatch_window_w;
extern const int devicepatch_window_h;
