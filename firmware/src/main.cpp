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

#include <TMCStepper.h>
#include <Arduino.h>

#include "vbs.hpp"
#include "pin_definitions.h"
#include "MS5837.h"

TMC2209Stepper driver(&SERIAL_PORT, R_SENSE, DRIVER_ADDRESS);   // Hardware Serial
MS5837 sensor;

void setup() {
  // Start USB coms
  Serial.begin(9600);
  Serial.println("Starting");

  Wire.begin();

  // Initialize pressure sensor
  // Returns true if initialization was successful
  // Defaults to Wire which corresponds with 
  // PINS 18 (SDA) and 19 (SCL) on Teensy 4.1
  while (!sensor.init()) {
    Serial.println("Init failed!");
    Serial.println("Are SDA/SCL connected correctly?");
    Serial.println("Blue Robotics Bar30: White=SDA, Green=SCL");
    Serial.println("\n\n\n");
    delay(5000);
  }

  Serial.println("Depth Sensor Found\n\n\n");

  // Select 30 bar model of depth sensor
  sensor.setModel(MS5837::MS5837_30BA);

  // 997 for freshwater
  sensor.setFluidDensity(1025);

  // Setup output pins
  pinMode(EN_PIN, OUTPUT);
  pinMode(STEP_PIN, OUTPUT);
  pinMode(DIR_PIN, OUTPUT);

  // Setup input pins
  pinMode(INDEX, INPUT);
  pinMode(LIM_EXT, INPUT);
  pinMode(LIM_RET, INPUT);

  // Disable driver initially 
  digitalWrite(EN_PIN, HIGH);

  SERIAL_PORT.begin(115200);      // HW UART drivers

  driver.begin();                 // UART: Init SW UART with default 115200 baudrate
  driver.toff(5);                 // Enables driver in software
  driver.rms_current(1000);       // Set motor RMS current
  driver.microsteps(0);           // Set microsteps to full steps 

driver.en_spreadCycle(true);      // Toggle spreadCycle
driver.pwm_autoscale(true);       // Needed for stealthChop
}

void loop() {
digitalWrite(DIR_PIN, HIGH);

  // Run 5000 steps and switch direction in software
  // for (uint16_t i = 5000; i>0; i--) {
  //   digitalWrite(STEP_PIN, HIGH);
  //   delayMicroseconds(500);
  //   digitalWrite(STEP_PIN, LOW);
  //   delayMicroseconds(500);
  // }

  // Update pressure and temperature readings
  sensor.read();

  Serial.print("Pressure: ");
  Serial.print(sensor.pressure());
  Serial.println(" mbar");

  Serial.print("Temperature: ");
  Serial.print(sensor.temperature());
  Serial.println(" deg C");

  Serial.print("Depth: ");
  Serial.print(sensor.depth());
  Serial.println(" m");

  Serial.print("Altitude: ");
  Serial.print(sensor.altitude());
  Serial.println(" m above mean sea level");

  delay(1000);
}