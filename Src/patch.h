#pragma once

#include "wc_fixture.h"   // [Fixtures] wc::Fixture + std::vector<wc::Fixture> wc_patch (source de verite du patch)

// [Fixtures] Régénère les tableaux legacy (Patch/output_fine/is_fine/curves/dimmer_type)
// depuis wc_patch (source de vérité) — appelé après toute édition/chargement du patch.
int rebuild_patch_from_fixtures();
// [Fixtures] Reconstruit wc_patch depuis les tableaux legacy courants (démarrage + chargement vieux show).
int synthesize_fixtures_from_legacy();
// [Fixtures] Persistance texte du patch fixtures (dans le dossier du show).
int save_patch_fixtures_text(const char* file);
int load_patch_fixtures_text(const char* file);   // 0 = OK, 1 = absent (vieux show), 2 = format invalide

//////////////////PATCH//////////////////////////////////////////////////////////
extern int xpatch_window;
extern int ypatch_window;
extern int scroller_patch;
extern float Patch_Scroll_Factor;
extern int iddim;
extern bool index_affect_patch;
extern bool index_affect_patch_16bit;   // [2b] mode patch 16 bit
extern bool index_menu_curve;
extern bool index_reset_curve;
extern bool index_square_curve;
extern bool index_fluo_curve;
extern bool index_preheat_curve;
extern bool Dimmers_selected[514];
extern int Patch[514];
extern bool dimmer_type[514];
extern unsigned short MergerArray[514];   // [2c-2A] 16 bit
extern int output_fine[514];   // [2b] patch 16 bit : output coarse -> son output fine
extern bool is_fine[514];       // [2b] output = LSB d'un canal 16 bit
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
int x, y;
fixed tangent;
} curve_node;
#define MAX_curve_nodeS 8
extern curve_node curve_nodes[MAX_curve_nodeS];
extern int curve_node_count;
extern fixed curve_curviness;
typedef struct NODE {
int x, y;
fixed tangent;
} NODE;
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
