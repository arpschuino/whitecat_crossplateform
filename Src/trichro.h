#pragma once

// GEL LIST in TRICHROMY
// GUI
int gel_size_window = 400;
bool show_gel_list = 1;
int gel_position[4];
int call_ref_number = 0; // appel clavier
// gel list trichro 0 Lee 1 Rosco 2 Gamcolor 3 Apollo
int index_gel_type_selected = 0;
int refs_of_gels[4][10000];      // numerical reference
char name_of_gels[4][10000][96]; // nom des gels
int rvb_of_gels[4][10000][3];    // rvb of gels
float gel_transimission[4][10000];
int gel_position_selected[4];
bool show_designer_list = 0;
bool index_use_transmission = 0;

volatile bool index_snap_color_wheel_levels = 0;
int previous_trichro_wheel = 0;

////////////////////////THRICHRO////////////////////////////////////////////////

#define ACCURACY_DOUBLE 1.e-06
int xtrichro_window = 1128, ytrichro_window = 350;
int default_xtrichro_window = 1128, default_ytrichro_window = 350;
// les 8 colors docks et leurs données
bool dock_color_type[8];                   // 0 trichro 1 mode quadri
int x_y_picker_par_colordock[8][2];
float angle_hue_par_colordock[8];
int picker_trichro[8][4];                  // stockage des niveaux RVB YELLOW
unsigned char dock_color_buffer_C[8][514]; // resultat dans buffers separés
bool index_quadri = 0;                     // 0 mode trichro 1 mode quadri jaune
bool dock_color_channels[8][4][514];       // trichro sur circuits
float position_curseur_hue_x;
float position_curseur_hue_y;
float angle;
float angle_snap;
float vx, vy;
float vxh = 0;   // hue triangle position
float vyh = 100;
float vxd = -90; // Dark position
float vyd = -50;
float vxw = 90;  // white position
float vyw = -50;
// le hue general
int cref = 0;
int r_pick = 255, b_pick = 0, v_pick = 0;
// le choix dans le triangle
float picker_x, picker_y, last_picker_x, last_picker_y;
int my_red, my_blue, my_green, my_yellow;
int colorpicker;
// Variables roue de maison
double xcl, ycl;
int rcl, gcl, bcl;
float hcl, ccl;

// Variables petite section trichro (dock)
bool index_affect_color_to_dock = 0;
int dock_color_selected = 0;
SDL_Surface *bmp_buffer_trichro;
char string_dock_col_sel[36];
