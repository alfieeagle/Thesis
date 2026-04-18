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
	Serial.begin(9600);
	Serial.println("Starting");

	Wire.begin();

	// Initialize pressure sensor
	// Returns true if initialization was successful
	// Defaults to Wire which corresponds with 
	// PINS 18 (SDA) and 19 (SCL) on Teensy 4.0
	while (!DepthSensor.init()) {
	  Serial.println("Init failed!");
	  Serial.println("Are SDA/SCL connected correctly?");
	  Serial.println("Blue Robotics Bar30: White=SDA, Green=SCL");
	  Serial.println("\n\n\n");
	  delay(5000);
	}

	Serial.println("Depth Sensor Found\n\n\n");

	// // Select 30 bar model of depth sensor
	DepthSensor.setModel(MS5837::MS5837_30BA);

	// // freshwater
	// // depth_sensor.setFluidDensity(997);
	// // salt water
	DepthSensor.setFluidDensity(1025);

	// Setup output pins
	pinMode(EN_PIN, OUTPUT);
	pinMode(STEP_PIN, OUTPUT);
	pinMode(DIR_PIN, OUTPUT);

	// Setup input pins
	pinMode(INDEX, INPUT);
	pinMode(LIM_EXT, INPUT);
	pinMode(LIM_RET, INPUT);

	debouncePins(LIM_EXT, LIM_RET, DEBOUNCE_TIME);

	// Disable driver initially 
	digitalWrite(EN_PIN, HIGH);

	SERIAL_PORT.begin(SERIAL_BAUD_RATE);      // HW UART drivers

	// Setup the driver
	Driver.setup(serial_stream, SERIAL_BAUD_RATE);      // UART: Init SW UART with default 115200 baudrate
	Driver.setHardwareEnablePin(EN_PIN);				// Tie the software and hardware enable pins together
	Driver.disable();
	Driver.enableAutomaticCurrentScaling();				// Put driver in current control mode
	Driver.enableAutomaticGradientAdaptation();			// Allow for automatic PID gradient adaption with changing loads
	Driver.setRunCurrent(RUN_CURRENT_PERCENT);			
	Driver.setStandstillMode(TMC2209::StandstillMode::STRONG_BRAKING);	// Have the motor brake hard when stopped
	Driver.setHoldCurrent(HOLD_CURRENT_PERCENT);
	Driver.enableStealthChop();								// Reduce driver noise 
	Driver.setStallGuardThreshold(STALL_GUARD_THRESHOLD);	// Set sensitivity to stalling (0..255), Sensitivity (Low..High)
	Driver.enableCoolStep();								// Cool step provides up 75% energy savings 
	stealth_chop_automatic_tuning();						// Run the tuning sequency to allow driver to auto adjust PID accurately

	// Setup interrup for limit switches
	attachInterrupt(LIM_EXT, handle_max_extension, LOW);
	attachInterrupt(LIM_RET, handle_max_retraction, LOW);
}

void loop() 
{
	if(ActuatorTimer.check() == true)
	{
		_VBS.update_motor_command();
		std::vector<float> motorCommand = _VBS.get_motor_command();
		
	}

}