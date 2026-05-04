#pragma once

//////ARDUINO/////////////////////////////////
extern int open_arduino_on_open;
extern char string_Arduino_status[128];
extern int arduino_max_devices;

extern int nBytesReadArduino0;
extern int nBytesSendtoArduino;

extern int arduino_com0;
extern int arduino_baud_rate0;
extern bool arduino_device_0_is_ignited;

#define digital_limit 127
#define analog_limit 63
#define pwm_limit 35

extern unsigned char input_str_arduino[digital_limit];

extern int digital_data_from_arduino[digital_limit];
extern int previous_digital_data_from_arduino[digital_limit];
extern int arduino_max_digital;
extern int analog_data_from_arduino[analog_limit];
extern int previous_analog_data_from_arduino[analog_limit];
extern bool ventilate_analog_data[analog_limit];
extern int arduino_max_analog;
extern bool digital_data_is_switch[digital_limit];
extern bool snap_dig_for_switch[digital_limit];

extern int digital_data_to_arduino[digital_limit];
extern int previous_digital_data_to_arduino[digital_limit];

extern int arduino_digital_type[digital_limit];
extern int arduino_digital_function_input[digital_limit][2];
extern int arduino_analog_function_input[analog_limit];
extern int arduino_analog_attribution_input[analog_limit];
extern int position_line_io;
extern int position_line_ana;
extern bool arduino_simulating_midi;

extern int arduino_digital_function_output[digital_limit][2];

extern int pwm_data_to_arduino[pwm_limit];
extern int previous_pwm_data_to_arduino[pwm_limit];
