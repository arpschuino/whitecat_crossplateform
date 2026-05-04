#pragma once

/// window plot
extern bool index_plot_window;
extern int x_plot;
extern int y_plot;
extern int plot_window_x_size;
extern int plot_window_y_size;
extern int plot_minimum_y_size;
extern int plot_nbre_col;
extern int plot_nbre_lin;
extern int plot_quadrillage_size;
extern int position_plan_x;
extern int position_plan_y;
extern bool index_edit_symbol_plot;
extern bool index_edit_light_plot;

extern int index_menus_lighting_plot;

extern int plot_viewpoint_xy[2];

extern Rgba CouleurPlotLine;
extern Rgba CouleurPlotFill;

extern int symbol_selected_type;
extern int max_symbol_type;
extern float size_symbol[127];
extern char symbol_nickname[127][25];
extern float angle_projo_selectionne;

extern int symbol_id_to_select;

// 128 symbols possibles par page x4
const int nbre_symbol_per_layer = 128;
extern bool show_calc_number[4];
extern short view_plot_calc_number_is;
extern bool plot_layer_mode;
extern int nbre_symbols_on_plot[4];

extern int symbol_type[4][nbre_symbol_per_layer];
extern bool symbol_is_selected[4][nbre_symbol_per_layer];
extern float angle_symbol[4][nbre_symbol_per_layer];
extern int pos_symbol[4][nbre_symbol_per_layer][2];
extern int stored_plot_position[nbre_symbol_per_layer][2];
extern int symbol_is_linked_to[4][nbre_symbol_per_layer];

extern char symbol_note[4][nbre_symbol_per_layer][4][25];
// move dans le plan
extern bool index_click_inside_plot;
extern int plot_original_mouse_x;
extern int plot_original_mouse_y;
extern int plot_facteur_move_x;
extern int plot_facteur_move_y;

extern bool plot_show_options[12];

extern bool plot_delete_selection[nbre_symbol_per_layer];

extern bool plot_name_text_size;

extern int symbol_channel_is[4][nbre_symbol_per_layer];
extern int symbol_dimmer_is[4][nbre_symbol_per_layer];
extern int nbre_gelats_manufact;
extern int gelat[4][nbre_symbol_per_layer][3];
extern int gelat_family[4][nbre_symbol_per_layer][3];

extern int last_selected_symbol_is;

extern char temp_plot_info[8];

extern bool index_edit_relativ_xy;
extern bool index_click_inside_relativ_xy;
extern int relatif_plot_xy[4][nbre_symbol_per_layer][5][2];
extern bool adjust_xy_type[5];

extern int temp_relatif_plot_xy_array[nbre_symbol_per_layer][5][2];
extern int rlativ_xm;
extern int rlativ_ym;

extern bool index_plot_send_to_mode;

extern bool index_ask_delete_symbols_on_calc;

extern int plot_layer_selected;

extern float global_symbol_size;

// SNAPSHOT pour manipulations sur un calque:
extern bool snapshot_symbol_is_selected[nbre_symbol_per_layer];
extern int snapshot_symbol_type[nbre_symbol_per_layer];
extern float snapshot_angle_symbol[nbre_symbol_per_layer];
extern int snapshot_pos_symbol[nbre_symbol_per_layer][2];
extern int snapshot_symbol_channel_is[nbre_symbol_per_layer];
extern int snapshot_symbol_dimmer_is[nbre_symbol_per_layer];
extern int snapshot_gelat[nbre_symbol_per_layer][4];
extern int snapshot_gelat_family[nbre_symbol_per_layer][4];
extern int snapshot_relatif_plot_xy[nbre_symbol_per_layer][5][2];
extern char snapshot_symbol_note[nbre_symbol_per_layer][4][25];
extern int snapshot_symbol_is_linked_to[nbre_symbol_per_layer];

extern bool index_build_patch_from_plot;

extern Bitmap LightPlanBitmapMain;
extern int position_relative_plan_theatre[2];
extern int taille_relative_plan_theatre[2];

extern float orientation_plan_theatre;

extern int old_position_relative_plan_theatre[2];
extern int old_taille_relative_plan_theatre[2];

extern bool index_editing_theatre_plan;
extern int original_posx;
extern int original_posy;
extern int moving_plan_relativ_x;
extern int moving_plan_relativ_y;

extern int editing_plan_data_type;

extern char Name_of_plane_is[256];
extern char list_import_plans[127][256];
extern int importplan_selected;
extern int line_importplan;

extern float alpha_plan;
extern bool editing_plot_sizey;
extern bool editing_plot_sizex;
extern int moving_size_relativ_x;
extern int moving_size_relativ_y;
extern int old_plot_sizex;
extern int old_plot_sizey;
extern int plot_view_port_x;
extern int plot_view_port_y;
extern int previous_plot_view_port_x;
extern int previous_plot_view_port_y;
extern bool index_move_plot_view_port;
extern bool plot_editing_color_background;
extern bool plot_editing_color_line;

extern bool plot_index_show_levels;
extern bool plot_index_show_levels_from_faders;

extern int plot_grid_type;
extern float alpha_grille;

extern float Color_plotline;
extern float Color_plotfill;

extern int nbre_shapes_on_plot;

const short max_shape_type = 11;
extern char shape_nickname[max_shape_type + 1][25];

extern int lock_background_proportions;

extern int shape_selected_type;
extern float general_shape_size_to_apply;
extern float angle_shape_selected;
extern int color_pattern_selected;
extern float opening_angle_selected;
extern bool index_adjusting_shape_x;
extern bool index_adjusting_shape_y;
extern float shape_rect_size_x;
extern float shape_rect_size_y;
extern float temp_shape_rect_size_x;
extern float temp_shape_rect_size_y;

extern char temp_shape_legend_name[25];

extern float general_alpha_for_shape;

extern int shape_type[nbre_symbol_per_layer];
extern float shape_attributes[nbre_symbol_per_layer][5];
extern int shape_position[nbre_symbol_per_layer][4];
extern char shape_legend_name[nbre_symbol_per_layer][25];
extern int shape_relativ_position_legend_name[nbre_symbol_per_layer][2];
extern int shape_color_type[nbre_symbol_per_layer];
extern int shape_groups[nbre_symbol_per_layer];

extern bool shape_selected[nbre_symbol_per_layer];

extern bool handle_selected_for_line_editing;
extern int stored_plot_shape_position[nbre_symbol_per_layer][4];

extern bool index_ask_general_clear_on_plot;
extern int editing_shape_line_number;
extern int index_show_shape_id;
extern int index_last_shape_selected;
extern int nbre_branches_polygon;

// snapshot
extern bool snap_shape_selected[nbre_symbol_per_layer];
extern int snap_shape_type[nbre_symbol_per_layer];
extern float snap_shape_attributes[nbre_symbol_per_layer][5];
extern int snap_shape_position[nbre_symbol_per_layer][4];
extern char snap_shape_legend_name[nbre_symbol_per_layer][25];
extern int snap_shape_relativ_position_legend_name[nbre_symbol_per_layer][2];
extern int snap_shape_color_type[nbre_symbol_per_layer];
extern int snap_shape_groups[nbre_symbol_per_layer];
extern int shape_id_to_select;

extern int temp_shape_relativ_position_legend_name[nbre_symbol_per_layer][2];

extern int fonttype_selected;
extern TextRenderer PlotTextRenderer;

extern float ratio_lock_plot_scale;
extern bool index_do_clear_all_the_patch;
extern bool index_ask_clear_preset_relativ;
extern bool index_ask_modify_preset_relativ;
extern bool index_ask_record_preset_relativ;

extern int xyrelativ_preset;

extern int plot_relativxy_preset[8][4][2];
extern int mode_relatif_xy_solo;

extern int number_of_shapes_groups;

// legend
extern int legend_position_x;
extern int legend_position_y;
extern int legend_size_y;

extern int plot_list_appareils[127];
extern bool plot_show_typ_appareil[127];
extern int plot_ecartement_legende[127];
extern int nbre_symbols_differents_sur_plot;

extern int index_list_appareils_pour_la_legende[127];

extern int plot_correcteur_portx;
extern int plot_correcteur_porty;
extern char plot_renseignements[40][25];

extern bool legend_view;

extern int tmp_gel_list[5][1000][127];
extern char Geo[1000][48];
extern char GelTyp[12];
extern char Appareil[25];
extern int nbre_ligne_pour_gelats;

extern bool index_do_a_screen_capture;
extern bool index_do_a_plot_screen_capture;

extern char plot_name_of_capture[25];
extern float index_show_button_export;
extern float plot_light_preset_on_click[8];
extern int last_xyrelativ_preset;

extern bool index_tab_on;
