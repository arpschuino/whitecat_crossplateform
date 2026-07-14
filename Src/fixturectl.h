#pragma once

// [devices] Fenetre "Control Fixtures" (W_FIXTURECTL) : editeur d'attributs des devices
// (Pan/Tilt/RGBW/Zoom/Shutter...) + palettes. Tranche 1 = coquille (ouvre/ferme/deplace).
// Cf. plan devices_control_window_plan. Modele de cablage : la fenetre GRID (grider.*).

extern bool index_window_fixturectl;   // fenetre ouverte (1) / fermee (0)
extern int  fixturectl_window_x;        // position ecran (coin haut-gauche)
extern int  fixturectl_window_y;

// dimensions de la coquille (redimensionnables : partagees rendu <-> zones de clic)
extern int fixturectl_window_w;
extern int fixturectl_window_h;

// [devices] cible de la molette publiee chaque frame par fixturectl_window (lue par DoMouseLevel) :
//   = le NOM d'attribut GDTF de l'encodeur survole ("" = aucun ; "\x01" = Int/intensite). La molette
//   applique alors un delta relatif a cet attribut sur TOUTE la selection (fxc_apply_delta).
extern char fixturectl_wheel_hover[24];
