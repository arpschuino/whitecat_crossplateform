#pragma once

// GEL LIST in TRICHROMY
// GUI
extern int gel_size_window;
extern bool show_gel_list;
extern int gel_position[4];
extern int call_ref_number;
// gel list trichro 0 Lee 1 Rosco 2 Gamcolor 3 Apollo
extern int index_gel_type_selected;
extern int refs_of_gels[4][10000];
extern char name_of_gels[4][10000][96];
extern int rvb_of_gels[4][10000][3];
extern float gel_transimission[4][10000];
extern int gel_position_selected[4];
extern bool show_designer_list;
extern bool index_use_transmission;

extern volatile bool index_snap_color_wheel_levels;
extern int previous_trichro_wheel;

////////////////////////THRICHRO////////////////////////////////////////////////

#define ACCURACY_DOUBLE 1.e-06
extern int xtrichro_window, ytrichro_window;
extern int default_xtrichro_window, default_ytrichro_window;
// les 8 colors docks et leurs données
extern bool dock_color_type[8];
extern int x_y_picker_par_colordock[8][2];
extern float angle_hue_par_colordock[8];
extern int picker_trichro[8][4];
extern unsigned char dock_color_buffer_C[8][514];
extern bool index_quadri;
extern bool dock_color_channels[8][4][514];
extern float position_curseur_hue_x;
extern float position_curseur_hue_y;
extern float angle;
extern float angle_snap;
extern float vx, vy;
extern float vxh;
extern float vyh;
extern float vxd;
extern float vyd;
extern float vxw;
extern float vyw;
// le hue general
extern int cref;
extern int r_pick, b_pick, v_pick;
// le choix dans le triangle
extern float picker_x, picker_y, last_picker_x, last_picker_y;
extern int my_red, my_blue, my_green, my_yellow;
extern int colorpicker;
// Variables roue de maison
extern double xcl, ycl;
extern int rcl, gcl, bcl;
extern float hcl, ccl;

// Variables petite section trichro (dock)
extern bool index_affect_color_to_dock;
extern int dock_color_selected;
extern SDL_Surface *bmp_buffer_trichro;
extern char string_dock_col_sel[36];
