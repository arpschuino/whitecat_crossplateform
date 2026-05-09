#pragma once
int define_colors();
int launchpad_set_led(int control, int color);
int launchpad_set_bool_value(int control, int value);
int launchpad_set_zero_value(int control);
int launchpad_refresh_buffer_led();
int reset_launchpad();
