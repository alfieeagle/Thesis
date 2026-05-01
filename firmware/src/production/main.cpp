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
	// for(int i = 0; i < 5; i++)
	// {
	// 	if(!DepthSensor.init()) 
	// 	{
	// 		Serial.println("Init failed!");
	// 		Serial.println("Are SDA/SCL connected correctly?");
	// 		Serial.println("Blue Robotics Bar30: White=SDA, Green=SCL");
	// 		Serial.println("\n\n\n");
	// 		delay(3000);
	// 		continue;
	// 	}
	// 	else
	// 	{
	// 		Serial.println("Depth Sensor Found\n\n\n");

	// 		// Select 30 bar model of depth sensor
	// 		DepthSensor.setModel(MS5837::MS5837_30BA);

	// 		// freshwater
	// 		// depth_sensor.setFluidDensity(997);
	// 		// salt water
	// 		DepthSensor.setFluidDensity(1025);

	// 		// Set the initial depth
	// 		_VBS.update_depth(DepthSensor.depth());
	// 		break;
	// 	}
	// }
	
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
	// pinMode(LIM_EXT, INPUT);
	// pinMode(LIM_RET, INPUT);

	// // Set the limit switches as debounce pins
	// debouncePins(LIM_EXT, LIM_RET, DEBOUNCE_TIME_MS);

	// // Setup interrup for limit switches
	// attachInterrupt(LIM_EXT, handle_max_extension, FALLING);
	// attachInterrupt(LIM_RET, handle_max_retraction, FALLING);

	// Full step mode
	digitalWrite(DM0, LOW);
	digitalWrite(DM1, LOW);
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
	motor.disableOutputs();
	// homing_sequence();
	// Serial.println("Homing Finished");
	// delay(200);
	// Serial.println("Moving to neutral point");
	// neutral_point();

	Serial.println("Finished Setup");
}

void loop() 
{
	// Read depth and update control at 10 Hz
	// provided it's not within the 10 cm deadzone
	// if(ControlTimer.check() == true)
	// {
	// 	float newDepth = DepthSensor.depth();
	// 	if (std::abs(newDepth - _VBS.get_current_depth()) > DEADZONE_THRESHOLD)
	// 	{
	// 		_VBS.update_depth(newDepth);
	// 		_VBS.update_control(TIMER_INTERVAL_MILLIS/1000);
	// 		_VBS.update_motor_command();
	// 		std::vector<float> motorCommand = _VBS.get_motor_command();
	// 		send_motor_command(motorCommand);
	// 	}
	// }

	// // Step the motor
	// step();

	// if(Serial.available() > 0)
	// {
	// 	incomingByte = Serial.read();
	// 	Serial.print("Message recieved: ");
	// 	Serial.println(incomingByte, DEC);
	// }

	digitalWrite(DIR_PIN, HIGH);
	digitalWrite(STEP_PIN, HIGH);
	delayMicroseconds(500);
	digitalWrite(DIR_PIN, LOW);
	digitalWrite(STEP_PIN, LOW);
	delayMicroseconds(500);

	
}
