#pragma once

int plot_generate_appareils_list();
int scan_planfolder();
int add_channel_selection_to_layers_plot();
int substract_channel_selection_to_layers_plot();
int clear_shape_slot(int s);
int clear_calc(int calc);
int change_shape_type(int shapetype);
int affect_angle_to_shapes(float a);
int unselect_all_shapes();
int delete_selected_shapes();
int add_shape_to_plot(int shapetype);
int reset_symbols_selected(int calc);
int change_symbol_type(int calc, int typ);
int add_a_symbol_to_plot(int plot_calc_number_is);
int duplicate_selected_symbols(int calc);
int delete_selected_symbols(int calc);
int send_symbols_from_calc_to_calc(int calc_source, int calc_dest);
int duplicate_selected_shapes();
int affect_angle_to_symbols(int plot_calc_number_is, float angle_is);
