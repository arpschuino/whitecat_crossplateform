#include "fixturectl.h"

// [devices] Etat de la fenetre "Control Fixtures". Voir fixturectl_visu.cpp pour le rendu.

bool index_window_fixturectl = 0;
int  fixturectl_window_x = 320;
int  fixturectl_window_y = 180;

int fixturectl_window_w = 670;   // largeur (redimensionnable via la poignee coin bas-droit)
int fixturectl_window_h = 360;   // hauteur (redimensionnable)

char fixturectl_wheel_hover[24] = {0};  // cible molette : nom de l'attribut survole ("" = aucun ; "\x01" = Int) (cf. fixturectl.h)
