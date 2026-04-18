#ifndef UTILS_HPP

#define UTILS_HPP

#include <vector>
#include <Metro.h>

#include "vbs.hpp"
#include "pin_definitions.h"
#include "MS5837.h"
#include "TMC2209.h"
#include "vbs_config.h"

TMC2209 Driver;
HardwareSerial &serial_stream = SERIAL_PORT;
MS5837 DepthSensor;
Metro ActuatorTimer = Metro(ACTUATOR_TIMER);
VBS _VBS;

void handle_max_extension();
void handle_max_retraction();
void send_motor_command(std::vector<float> motorCommand);
void stealth_chop_automatic_tuning();

#endif