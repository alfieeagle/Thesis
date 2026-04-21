#ifndef UTILS_HPP

#define UTILS_HPP

#include <vector>
#include <Metro.h>
#include <TMC2209.h>
#include <MS5837.h>
#include <Wire.h>

#include "vbs.hpp"
#include "vbs_config.h"
#include "pin_definitions.h"

extern TMC2209 Driver;
extern HardwareSerial &serial_stream;
extern MS5837 DepthSensor;  
extern Metro ActuatorTimer;
extern Metro ControlTimer;
extern VBS _VBS;

void handle_max_extension();
void handle_max_retraction();
void send_motor_command(const std::vector<float>& motorCommand);
void stealth_chop_automatic_tuning();

#endif