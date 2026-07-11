#define WC_SKIP_GLOBALS
#include "graphics_backend.h"
#include "Crossplateform.h"
#include "patch.h"

//////////////////PATCH//////////////////////////////////////////////////////////
int xpatch_window = 200;
int ypatch_window = 100;
int scroller_patch = 1;
float Patch_Scroll_Factor = 25.0;
int iddim = 0;
bool index_affect_patch = 0;
bool index_affect_patch_16bit = 0;   // [2b] mode patch 16 bit (coarse + fine=coarse+1)
bool index_affect_patch_device = 0;  // [devices] mode : clic sur un output -> device RGB (3 outputs) sur le circuit selectionne
bool index_menu_curve = 0;
bool index_reset_curve = 0;
bool index_square_curve = 0;
bool index_fluo_curve = 0;
bool index_preheat_curve = 0;
bool Dimmers_selected[514];
int Patch[514];
bool dimmer_type[514]; // 0=HTP 1=LTP
unsigned short MergerArray[514];   // [2c-2B] 16 bit pleine echelle (x257)
int output_fine[514];   // [2b] pour un output coarse : son output fine (LSB) ; 0 = output 8 bit
bool is_fine[514];      // [2b] cet output est le LSB d'un canal 16 bit (rendu par son coarse)
unsigned char output_attribute[514];   // [devices] attribut GDTF de l'output (wc::AttrId) ; rempli par rebuild_patch_from_fixtures
unsigned short output_devval[514];     // [devices] valeur vivante 16 bit par output des attributs NON-Dimmer (0 par defaut)
unsigned short devval_preset[514];     // [devices] endpoint cue entrante du crossfade (cf. patch.h)
std::vector<wc::Fixture> wc_patch;   // [Fixtures] SOURCE DE VERITE du patch (tableaux ci-dessus = cache derive)
char string_monitor_patch[1024];
bool index_patch_affect_is_done = 0;
int last_dim_selected = 0;
int dimmer_check_level = 192;
// Curves splines
bool index_writing_curve = 0;
float curve_spline_level = 0.0;
int curve_selected = 0;
int the_curve_spline_level[16];
int curve_report[16][256];
int index_curve_spline_level = 0;
bool index_enable_curve_editing = 0;
int curves[514];
int curve_ctrl_pt[16][8][2];
int diam_curve_node = 10;
curve_node curve_nodes[MAX_curve_nodeS];
int curve_node_count = 0;
fixed curve_curviness;
NODE nodes[MAX_NODES];
int node_count;
fixed curviness;
bool show_control_points = 0;
int actual_step_node = 0;
int next_step_node = 0;
int numero_de_dock_goto_spline = 1;
/////////////////LISTE PROJOS//////////////////////////
bool index_list_projecteurs = 1;
bool index_edit_listproj = 0;
char descriptif_projecteurs[514][25];
int Xlistproj = 300;
int Ylistproj = 100;
int line_list_is = 0;
char listnum[12];
