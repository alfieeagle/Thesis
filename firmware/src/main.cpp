/** ------------------------ main.cpp ------------------------

Author:			Alfie Eagleton

Date:			15/4/26

Description:
This file contains the main entry point of the VBS program which 
communicates with sensors, controls the VBS hardware and reports
back via USB to an external computer.

Dependencies:	TMCStepper.h, Arduino.h, VBS.hpp

**/


#include <TMCStepper.h>
#include <Arduino.h>


#define DIR_PIN          18 // Direction
#define STEP_PIN         19 // Step
#define SERIAL_PORT Serial1 // HardwareSerial port
#define DRIVER_ADDRESS 0b00 // TMC2209 Driver address according to MS1 and MS2

#define R_SENSE 0.11f 

TMC2209Stepper driver(&SERIAL_PORT, R_SENSE, DRIVER_ADDRESS);                    // Hardware Serial

void setup() {
  // pinMode(EN_PIN, OUTPUT);
  pinMode(STEP_PIN, OUTPUT);
  pinMode(DIR_PIN, OUTPUT);
  // digitalWrite(EN_PIN, LOW);      // Enable driver in hardware

  SERIAL_PORT.begin(115200);      // HW UART drivers

  driver.begin();                 //  SPI: Init CS pins and possible SW SPI pins
                                  // UART: Init SW UART (if selected) with default 115200 baudrate
  driver.toff(5);                 // Enables driver in software
  driver.rms_current(1000);        // Set motor RMS current
  driver.microsteps(0);          // Set microsteps to full steps 

driver.en_spreadCycle(true);   // Toggle spreadCycle on TMC2208/2209/2224
driver.pwm_autoscale(true);     // Needed for stealthChop
}

void loop() {
digitalWrite(DIR_PIN, HIGH);

  // Run 5000 steps and switch direction in software
  for (uint16_t i = 5000; i>0; i--) {
    digitalWrite(STEP_PIN, HIGH);
    delayMicroseconds(500);
    digitalWrite(STEP_PIN, LOW);
    delayMicroseconds(500);
  }
}