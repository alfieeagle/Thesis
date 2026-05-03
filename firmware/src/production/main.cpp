/** ------------------------ main.cpp ------------------------

Author:			Alfie Eagleton

Date:			15/4/26

Description:
This file contains the main entry point of the VBS program which 
communicates with sensors, controls the VBS hardware and reports
back via USB to an external computer.

Dependencies:	TMCStepper.h, Arduino.h, vbs.hpp, pin_definitions.h
              MS5837.h

**/

#include <Arduino.h>
#include <Debounce.h>

#include "utils.hpp"

void setup() {
	// Start USB coms
	Serial.begin(115200);
	while(!Serial){};
	encode_data_and_send("[INFO] Starting");

	// Setup messaging interval timer to send message every 0.1 s
	msgTimer.begin(timer_callback, 100000);

	Wire.begin();

	// Initialize pressure sensor
	// Returns true if initialization was successful
	// Defaults to Wire which corresponds with 
	// PINS 18 (SDA) and 19 (SCL) on Teensy 4.0
	if(!DepthSensor.init()) 
	{
		encode_data_and_send("[ERROR] Depth sensor initialisation failed!\n\
							[DEBUG] Are SDA/SCL connected correctly?\n\
							[DEBUG] Blue Robotics Bar30: White=SDA, Green=SCL");
	}
	else
	{
		encode_data_and_send("[INFO] Depth Sensor Found");

		// Select 30 bar model of depth sensor
		DepthSensor.setModel(MS5837::MS5837_30BA);

		// freshwater
		// depth_sensor.setFluidDensity(997);
		// salt water
		DepthSensor.setFluidDensity(1025);

		// Set the initial depth
		_VBS.update_depth(DepthSensor.depth());
	}
	
	// Setup output pins
	pinMode(EN_PIN, OUTPUT);
	pinMode(STEP_PIN, OUTPUT);
	pinMode(DIR_PIN, OUTPUT);
	pinMode(DM0, OUTPUT);
	pinMode(DM1, OUTPUT);
	pinMode(DM2, OUTPUT);
	pinMode(RESET_PIN, OUTPUT);
	pinMode(AGC_PIN, OUTPUT);

	// Setup input pins
	pinMode(LIM_EXT, INPUT);
	pinMode(LIM_RET, INPUT);

	// Set the limit switches as debounce pins
	debouncePins(LIM_EXT, LIM_RET, DEBOUNCE_TIME_MS);

	// Setup interrup for limit switches
	attachInterrupt(LIM_EXT, handle_max_extension, FALLING);
	attachInterrupt(LIM_RET, handle_max_retraction, FALLING);

	// // Full step mode
	digitalWrite(DM0, HIGH);
	digitalWrite(DM1, HIGH);
	digitalWrite(DM2, HIGH);

	// Enable active gain control to increase power savings 
	digitalWrite(AGC_PIN, HIGH);

	// Don't reset until the homing sequence is complete
	// LOW resets 
	digitalWrite(RESET_PIN, HIGH);

	// Setup the motor
	motor.setMaxSpeed(MAX_MOTOR_STEPS_SEC);
	motor.setAcceleration(MAX_ACCELERATION);
	motor.setMinPulseWidth(MIN_PULSE_WIDTH_MS);
	motor.setEnablePin(EN_PIN);
	homing_sequence();
	delay(200);
	neutral_point();
	encode_data_and_send("[INFO] Finished Setup");
}

void loop() 
{
	// Read depth and update control at 10 Hz
	// provided it's not within the 10 cm deadzone
	// if(ControlTimer.check() == true)
	// {
	// 	float newDepth = DepthSensor.depth();
	// 	// if (std::abs(newDepth - _VBS.get_current_depth()) > DEADZONE_THRESHOLD)
	// 	// {
	// 	_VBS.update_depth(newDepth);
	// 	_VBS.update_control((float)TIMER_INTERVAL_MILLIS/1000.0f);
	// 	_VBS.update_motor_command();
	// 	std::vector<float> motorCommand = _VBS.get_motor_command();
	// 	send_motor_command(motorCommand);
	// 	// }

	// 	// Encode the current system telemetry
	// 	// encode_data_and_send(NULL);
	// }

	// // Step the motor
	// step();

	// if(Serial.available() > 0)
	// {
	// 	decode_data_and_read(&latest_command);
    // }
}


