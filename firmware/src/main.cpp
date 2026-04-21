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

// Used to store incoming messages
int incomingByte = 0;

void setup() {
	// Start USB coms
	Serial.begin(9600);
	Serial.println("Starting");

	Wire.begin();

	// Initialize pressure sensor
	// Returns true if initialization was successful
	// Defaults to Wire which corresponds with 
	// PINS 18 (SDA) and 19 (SCL) on Teensy 4.0
	// while (!DepthSensor.init()) {
	//   Serial.println("Init failed!");
	//   Serial.println("Are SDA/SCL connected correctly?");
	//   Serial.println("Blue Robotics Bar30: White=SDA, Green=SCL");
	//   Serial.println("\n\n\n");
	//   delay(5000);
	// }

	// Serial.println("Depth Sensor Found\n\n\n");

	// // Select 30 bar model of depth sensor
	// DepthSensor.setModel(MS5837::MS5837_30BA);

	// // freshwater
	// // depth_sensor.setFluidDensity(997);
	// // salt water
	// DepthSensor.setFluidDensity(1025);

	// // Setup output pins
	pinMode(EN_PIN, OUTPUT);
	pinMode(STEP_PIN, OUTPUT);
	pinMode(DIR_PIN, OUTPUT);

	// Setup input pins
	// pinMode(LIM_EXT, INPUT);
	pinMode(LIM_RET, INPUT);

	// Set the limit switches as debounce pins
	debouncePins(LIM_RET, LIM_RET, DEBOUNCE_TIME_MS);

	// Setup interrup for limit switches
	// attachInterrupt(LIM_EXT, handle_max_extension, LOW);
	attachInterrupt(LIM_RET, handle_max_retraction, FALLING);

	// Disable driver initially 
	digitalWrite(EN_PIN, LOW);

	// Setup the motor
	motor.setMaxSpeed(MAX_MOTOR_STEPS_SEC);
	motor.setAcceleration(2000);
	motor.setMinPulseWidth(10);
	homing_sequence();
	delay(200);
	neutral_point();
				
	// Set the initial depth
	// _VBS.update_depth(DepthSensor.depth());

	Serial.println("Finished Setup");
}

void loop() 
{
	// digitalWrite(STEP_PIN, HIGH);
	// delayMicroseconds(500);
	// digitalWrite(STEP_PIN, LOW);
	// delayMicroseconds(500);

	// // Update and send the motor command at 100 Hz
	// if(ActuatorTimer.check() == true)
	// {
	// 	_VBS.update_motor_command();
	// 	std::vector<float> motorCommand = _VBS.get_motor_command();
	// 	send_motor_command(motorCommand);

		
	// }

	// // Read depth at 10 Hz and update control
	// // provided it's not within the 10 cm deadzone
	// if(ControlTimer.check() == true)
	// {
	// 	float newDepth = DepthSensor.depth();
	// 	if (std::abs(newDepth - _VBS.get_current_depth()) > DEADZONE_THRESHOLD)
	// 	{
	// 		_VBS.update_depth(newDepth);
	// 		_VBS.update_control(CONTROL_TIMER_INTERVAL_MILLIS/1000);
	// 	}
	// }

	// if(Serial.available() > 0)
	// {
	// 	incomingByte = Serial.read();
	// 	Serial.print("Message recieved: ");
	// 	Serial.println(incomingByte, DEC);
	// }
}
