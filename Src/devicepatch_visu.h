#pragma once

// [devices] Rendu et logique de la fenetre "Patch a device" (W_DEVICEPATCH).
int devicepatch_window(int xd, int yd);
int do_logical_devicepatch(int xd, int yd);

// [devices] molette sur les colonnes : colonne survolee (0=aucune) + defilement (steps>0 = vers le bas).
int  devicepatch_wheel_col();
void devicepatch_wheel(int steps);

// [devices] panneau compte GDTF Share (reutilisable : popup fenetre Patch + ecran Config).
void gdtf_account_draw(int x, int y);
void gdtf_account_logical(int x, int y);
