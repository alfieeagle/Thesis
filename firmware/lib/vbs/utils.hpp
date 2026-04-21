#ifndef UTILS_HPP

#define UTILS_HPP

#include <vector>
#include <Metro.h>
#include <MS5837.h>
#include <Wire.h>
#include <AccelStepper.h>

#include "vbs.hpp"
#include "vbs_config.h"
#include "pin_definitions.h"

extern MS5837 DepthSensor;  
extern Metro ActuatorTimer;
extern Metro ControlTimer;
extern VBS _VBS;
extern AccelStepper motor;  

void handle_max_extension();
void handle_max_retraction();
void send_motor_command(const std::vector<float>& motorCommand);
void stealth_chop_automatic_tuning();
void homing_sequence();
void neutral_point();

long distance_to_steps(float distance_m);

#endif