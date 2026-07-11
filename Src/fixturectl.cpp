#include "fixturectl.h"

// [devices] Etat de la fenetre "Control Fixtures". Voir fixturectl_visu.cpp pour le rendu.

bool index_window_fixturectl = 0;
int  fixturectl_window_x = 320;
int  fixturectl_window_y = 180;

const int fixturectl_window_w = 670;   // meme largeur que la cue list (Sequenciel_Window : 670 de base)
const int fixturectl_window_h = 360;   // hauteur pour loger des faders style master (course 255)

int fixturectl_wheel_hover = -1;       // cible molette (cf. fixturectl.h)
