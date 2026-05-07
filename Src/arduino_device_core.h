#pragma once
#include "Serial.h"
extern CSerial serial0;
int arduino_init(int device);
int arduino_close(int device);
