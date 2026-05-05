#define WC_SKIP_GLOBALS
#include "graphics_backend.h"
using namespace ol;
#include "plot.h"

/// window plot
bool index_plot_window = 0;
int x_plot = 100;
int y_plot = 100;
int plot_window_x_size = 1024;
int plot_window_y_size = 750;
int plot_minimum_y_size = 570;
int plot_nbre_col = 10;
int plot_nbre_lin = 10;
int plot_quadrillage_size = 50;
int position_plan_x = 230;
int position_plan_y = 50;
bool index_edit_symbol_plot = 0;
bool index_edit_light_plot = 1;
int index_menus_lighting_plot = 0; // 0 bckgs 1 shape 2 symbol 3 legend 4 working space
int plot_viewpoint_xy[2];
Rgba CouleurPlotLine(0.0, 0.0, 0.0);
Rgba CouleurPlotFill(1.0, 1.0, 1.0);
int symbol_selected_type = 0;
int max_symbol_type = 66;
float size_symbol[127];
char symbol_nickname[127][25];
float angle_projo_selectionne = 0.0;
int symbol_id_to_select = 0;
// 128 symbols possibles par page x4
bool show_calc_number[4];
short view_plot_calc_number_is = 0;
bool plot_layer_mode = 0; // 0 calc, 1 solo
int nbre_symbols_on_plot[4];
int symbol_type[4][nbre_symbol_per_layer];
bool symbol_is_selected[4][nbre_symbol_per_layer];
float angle_symbol[4][nbre_symbol_per_layer];
int pos_symbol[4][nbre_symbol_per_layer][2];
int stored_plot_position[nbre_symbol_per_layer][2];
int symbol_is_linked_to[4][nbre_symbol_per_layer];
char symbol_note[4][nbre_symbol_per_layer][4][25];
// move dans le plan
bool index_click_inside_plot = 0;
int plot_original_mouse_x = 0;
int plot_original_mouse_y = 0;
int plot_facteur_move_x = 0;
int plot_facteur_move_y = 0;
bool plot_show_options[12];
bool plot_delete_selection[nbre_symbol_per_layer];
bool plot_name_text_size = 0;
int symbol_channel_is[4][nbre_symbol_per_layer];
int symbol_dimmer_is[4][nbre_symbol_per_layer];
int nbre_gelats_manufact = 3;                  // 0: LEE 1: ROSCO 2: GAMCOLOR 3: Apollo
int gelat[4][nbre_symbol_per_layer][3];        // 3 slots de gelat par symbol
int gelat_family[4][nbre_symbol_per_layer][3];
int last_selected_symbol_is = 0;
char temp_plot_info[8];
bool index_edit_relativ_xy = 0;
bool index_click_inside_relativ_xy = 0;
int relatif_plot_xy[4][nbre_symbol_per_layer][5][2]; // calc / Symbols / Famille / X Y
bool adjust_xy_type[5];
int temp_relatif_plot_xy_array[nbre_symbol_per_layer][5][2];
int rlativ_xm = 0;
int rlativ_ym = 0;
bool index_plot_send_to_mode = 0;
bool index_ask_delete_symbols_on_calc = 0;
int plot_layer_selected = 0;
float global_symbol_size = 1.0;
// SNAPSHOT pour manipulations sur un calque:
bool snapshot_symbol_is_selected[nbre_symbol_per_layer];
int snapshot_symbol_type[nbre_symbol_per_layer];
float snapshot_angle_symbol[nbre_symbol_per_layer];
int snapshot_pos_symbol[nbre_symbol_per_layer][2];
int snapshot_symbol_channel_is[nbre_symbol_per_layer];
int snapshot_symbol_dimmer_is[nbre_symbol_per_layer];
int snapshot_gelat[nbre_symbol_per_layer][4];
int snapshot_gelat_family[nbre_symbol_per_layer][4];
int snapshot_relatif_plot_xy[nbre_symbol_per_layer][5][2];
char snapshot_symbol_note[nbre_symbol_per_layer][4][25];
int snapshot_symbol_is_linked_to[nbre_symbol_per_layer];
bool index_build_patch_from_plot = 1;
Bitmap LightPlanBitmapMain;
int position_relative_plan_theatre[2];
int taille_relative_plan_theatre[2];
float orientation_plan_theatre;
int old_position_relative_plan_theatre[2];
int old_taille_relative_plan_theatre[2];
bool index_editing_theatre_plan = 0;
int original_posx = 0;
int original_posy = 0;
int moving_plan_relativ_x = 0;
int moving_plan_relativ_y = 0;
int editing_plan_data_type = 0;
char Name_of_plane_is[256];
char list_import_plans[127][256];
int importplan_selected = 0;
int line_importplan = 0;
float alpha_plan = 1.0;
bool editing_plot_sizey = 0;
bool editing_plot_sizex = 0;
int moving_size_relativ_x = 0;
int moving_size_relativ_y = 0;
int old_plot_sizex = 0;
int old_plot_sizey = 0;
int plot_view_port_x = 0;
int plot_view_port_y = 0;
int previous_plot_view_port_x = 0;
int previous_plot_view_port_y = 0;
bool index_move_plot_view_port = 0;
bool plot_editing_color_background = 0;
bool plot_editing_color_line = 0;
bool plot_index_show_levels = 1;
bool plot_index_show_levels_from_faders = 1;
int plot_grid_type = 0; // 0 petits points, 1 ligne pleine
float alpha_grille = 0.5;
float Color_plotline = 0.0;
float Color_plotfill = 1.0;
int nbre_shapes_on_plot = 0;
char shape_nickname[max_shape_type + 1][25]; // 12 Shapes
int lock_background_proportions = 0;
int shape_selected_type = 0;
float general_shape_size_to_apply = 1.0;
float angle_shape_selected = 0.0;
int color_pattern_selected = 0;
float opening_angle_selected = 0.8;
bool index_adjusting_shape_x = 0;
bool index_adjusting_shape_y = 0;
float shape_rect_size_x = 2.0;
float shape_rect_size_y = 4.0;
float temp_shape_rect_size_x = 2.0;
float temp_shape_rect_size_y = 4.0;
char temp_shape_legend_name[25];
float general_alpha_for_shape = 1.0;
int shape_type[nbre_symbol_per_layer];
float shape_attributes[nbre_symbol_per_layer][5]; // size angle 0=general 1=rotation 2 size_x 3 size_y 4 opening
int shape_position[nbre_symbol_per_layer][4];     // pour les lignes
char shape_legend_name[nbre_symbol_per_layer][25];
int shape_relativ_position_legend_name[nbre_symbol_per_layer][2];
int shape_color_type[nbre_symbol_per_layer];
int shape_groups[nbre_symbol_per_layer];
bool shape_selected[nbre_symbol_per_layer];
bool handle_selected_for_line_editing = 0; // 0 = x1 y1 1= x2 y2
int stored_plot_shape_position[nbre_symbol_per_layer][4];
bool index_ask_general_clear_on_plot = 0;
int editing_shape_line_number = 0;
int index_show_shape_id = 0;
int index_last_shape_selected = 0;
int nbre_branches_polygon = 6;
// snapshot
bool snap_shape_selected[nbre_symbol_per_layer];
int snap_shape_type[nbre_symbol_per_layer];
float snap_shape_attributes[nbre_symbol_per_layer][5];
int snap_shape_position[nbre_symbol_per_layer][4];
char snap_shape_legend_name[nbre_symbol_per_layer][25];
int snap_shape_relativ_position_legend_name[nbre_symbol_per_layer][2];
int snap_shape_color_type[nbre_symbol_per_layer];
int snap_shape_groups[nbre_symbol_per_layer];
int shape_id_to_select = 0;
int temp_shape_relativ_position_legend_name[nbre_symbol_per_layer][2];
int fonttype_selected = 0;
TextRenderer PlotTextRenderer;
float ratio_lock_plot_scale = 1.0;
bool index_do_clear_all_the_patch = 0;
bool index_ask_clear_preset_relativ = 0;
bool index_ask_modify_preset_relativ = 0;
bool index_ask_record_preset_relativ = 0;
int xyrelativ_preset = 999;
int plot_relativxy_preset[8][4][2];
int mode_relatif_xy_solo = 0;
int number_of_shapes_groups = 0;
// legend
int legend_position_x = 0;
int legend_position_y = 0;
int legend_size_y = 600;
int plot_list_appareils[127];
bool plot_show_typ_appareil[127];
int plot_ecartement_legende[127];
int nbre_symbols_differents_sur_plot = 0;
int index_list_appareils_pour_la_legende[127];
int plot_correcteur_portx = 0;
int plot_correcteur_porty = 0;
char plot_renseignements[40][25];
bool legend_view = 1;
int tmp_gel_list[5][1000][127]; // gel manufacturer / ref max 1000 / symbol
char Geo[1000][48];
char GelTyp[12];
char Appareil[25];
int nbre_ligne_pour_gelats = 0;
bool index_do_a_screen_capture = 0;
bool index_do_a_plot_screen_capture = 0;
char plot_name_of_capture[25];
float index_show_button_export = 0.0;
float plot_light_preset_on_click[8];
int last_xyrelativ_preset = 0;
bool index_tab_on = 0;
