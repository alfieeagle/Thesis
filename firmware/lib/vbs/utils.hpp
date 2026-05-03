#ifndef UTILS_HPP

#define UTILS_HPP

#include <vector>
#include <Metro.h>
#include <MS5837.h>
#include <Wire.h>
#include <AccelStepper.h>
#include <string>
#include <cstring>
#include <IntervalTimer.h>

#include "vbs.hpp"
#include "vbs_config.h"
#include "pin_definitions.h"
#include "messages.pb.h"
#include "pb.h"
#include "pb_decode.h"
#include "pb_encode.h"

extern MS5837 DepthSensor;  
extern Metro ControlTimer;
extern VBS _VBS;
extern AccelStepper motor;  
extern IntervalTimer msgTimer;

// Global variable to store latest command
extern Command latest_command;

void handle_max_extension();
void handle_max_retraction();
void send_motor_command(const std::vector<float>& motorCommand);
void homing_sequence();
void neutral_point();
void step();
long distance_to_steps(float distance_m);

void encode_data_and_send(const char* msg);
void decode_data_and_read(Command* telemetry);
void timer_callback();

#endif