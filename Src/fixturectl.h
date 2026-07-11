#pragma once

// [devices] Fenetre "Control Fixtures" (W_FIXTURECTL) : editeur d'attributs des devices
// (Pan/Tilt/RGBW/Zoom/Shutter...) + palettes. Tranche 1 = coquille (ouvre/ferme/deplace).
// Cf. plan devices_control_window_plan. Modele de cablage : la fenetre GRID (grider.*).

extern bool index_window_fixturectl;   // fenetre ouverte (1) / fermee (0)
extern int  fixturectl_window_x;        // position ecran (coin haut-gauche)
extern int  fixturectl_window_y;

// dimensions de la coquille (partagees rendu <-> zones de clic)
extern const int fixturectl_window_w;
extern const int fixturectl_window_h;

// [devices] cible de la molette publiee chaque frame par fixturectl_window (lue par DoMouseLevel) :
//   -1 = aucun fader survole ; 0 = fader d'INTENSITE (nudge bufferSaisie[circuit]) ;
//   >=1 = output d'un attribut (nudge output_devval[output]).
extern int fixturectl_wheel_hover;
