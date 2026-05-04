#include "Crossplateform.h"
#include "patch.h"

//////////////////PATCH//////////////////////////////////////////////////////////
int xpatch_window = 200;
int ypatch_window = 100;
int scroller_patch = 1;
float Patch_Scroll_Factor = 25.0;
int iddim = 0;
bool index_affect_patch = 0;
bool index_menu_curve = 0;
bool index_reset_curve = 0;
bool index_square_curve = 0;
bool index_fluo_curve = 0;
bool index_preheat_curve = 0;
bool Dimmers_selected[514];
int Patch[514];
bool dimmer_type[514]; // 0=HTP 1=LTP
unsigned char MergerArray[514];
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
typedef struct curve_node {
    int x, y;
    fixed tangent;
} curve_node;
#define MAX_curve_nodeS 8
curve_node curve_nodes[MAX_curve_nodeS];
int curve_node_count = 0;
fixed curve_curviness;
typedef struct NODE {
    int x, y;
    fixed tangent;
} NODE;
#define MAX_NODES 49
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
