#pragma once
extern volatile int ticks_arduino;
extern int old_ticks_arduino;
void ticker_arduino();
int arduino_do_analog_in_whitecat();
int arduino_do_digital_in_whitecat();
int new_arduino_decode_array_method(unsigned char *tmp_ard);
int arduino_read();
int arduino_do_digital_out_whitecat();
int arduino_merge_and_do_data_out();
