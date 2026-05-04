#include "arduino.h"

//////ARDUINO/////////////////////////////////
int open_arduino_on_open = 0;
char string_Arduino_status[128];
int arduino_max_devices = 1;
int nBytesReadArduino0 = 0;
int nBytesSendtoArduino = 0;
int arduino_com0 = 4;
int arduino_baud_rate0 = 9600;
bool arduino_device_0_is_ignited = 0;
#define digital_limit 127
#define analog_limit 63
#define pwm_limit 35
unsigned char input_str_arduino[digital_limit];
int digital_data_from_arduino[digital_limit];
int previous_digital_data_from_arduino[digital_limit];
int arduino_max_digital = 13;
int analog_data_from_arduino[analog_limit];
int previous_analog_data_from_arduino[analog_limit];
bool ventilate_analog_data[analog_limit];
int arduino_max_analog = 5;
bool digital_data_is_switch[digital_limit];
bool snap_dig_for_switch[digital_limit];
int digital_data_to_arduino[digital_limit];
int previous_digital_data_to_arduino[digital_limit];
int arduino_digital_type[digital_limit];
int arduino_digital_function_input[digital_limit][2];
int arduino_analog_function_input[analog_limit];
int arduino_analog_attribution_input[analog_limit];
int position_line_io = 0;
int position_line_ana = 0;
bool arduino_simulating_midi = 0;
int arduino_digital_function_output[digital_limit][2];
int pwm_data_to_arduino[pwm_limit];
int previous_pwm_data_to_arduino[pwm_limit];
