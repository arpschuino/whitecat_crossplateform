#pragma once

int affichage_time_format(float time_to_convert);
int refresh_step_in_player(int grid_number, int num_step, int grider_player);
int clear_a_grid_step(int grid_number, int num_step);
int clear_part_of_a_grid(int grid_number, int num_stepfrom, int num_stepto);
int clear_a_grid(int grid_number);
int copy_grid_partially(int from_grid_number, int from_num_step, int to_step_number, int dest_grid_number, int dest_num_step);
int copy_step_to_step(int from_grid_number, int from_num_step, int dest_grid_number, int dest_num_step);
int insert_steps(int dest_grid_number, int from_num_step, int dest_nbr_step);
int gridder_prepare_cross(int grid_pl, int gr_actual_grid, int gr_actual_step);
int do_grid();
int gridplayer_step_minus(int num_grid_player);
int gridplayer_step_plus(int num_grid_player);
int gridplayer_seek(int num_grider);
