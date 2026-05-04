#pragma once

//////////////////PATCH//////////////////////////////////////////////////////////
extern int xpatch_window;
extern int ypatch_window;
extern int scroller_patch;
extern float Patch_Scroll_Factor;
extern int iddim;
extern bool index_affect_patch;
extern bool index_menu_curve;
extern bool index_reset_curve;
extern bool index_square_curve;
extern bool index_fluo_curve;
extern bool index_preheat_curve;
extern bool Dimmers_selected[514];
extern int Patch[514];
extern bool dimmer_type[514];
extern unsigned char MergerArray[514];
extern char string_monitor_patch[1024];
extern bool index_patch_affect_is_done;
extern int last_dim_selected;
extern int dimmer_check_level;

// Curves splines
extern bool index_writing_curve;
extern float curve_spline_level;
extern int curve_selected;
extern int the_curve_spline_level[16];
extern int curve_report[16][256];
extern int index_curve_spline_level;
extern bool index_enable_curve_editing;
extern int curves[514];
extern int curve_ctrl_pt[16][8][2];
extern int diam_curve_node;
typedef struct curve_node {
extern int x, y;
extern fixed tangent;
extern } curve_node;
#define MAX_curve_nodeS 8
extern curve_node curve_nodes[MAX_curve_nodeS];
extern int curve_node_count;
extern fixed curve_curviness;
typedef struct NODE {
extern int x, y;
extern fixed tangent;
extern } NODE;
#define MAX_NODES 49
extern NODE nodes[MAX_NODES];
extern int node_count;
extern fixed curviness;
extern bool show_control_points;
extern int actual_step_node;
extern int next_step_node;
extern int numero_de_dock_goto_spline;

/////////////////LISTE PROJOS//////////////////////////
extern bool index_list_projecteurs;
extern bool index_edit_listproj;
extern char descriptif_projecteurs[514][25];
extern int Xlistproj;
extern int Ylistproj;
extern int line_list_is;
extern char listnum[12];
