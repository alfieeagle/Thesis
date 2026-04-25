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

// Used to store incoming messages
const ulong BUFFER_LENGTH = 1000;

char buffer[BUFFER_LENGTH];

void setup() {
	// Start USB coms
	Serial.begin(9600);
	Serial.setTimeout(1000);
	memset(buffer, 0, sizeof(char) * BUFFER_LENGTH);
	Serial.println("Starting");
}

void loop() 
{
	if(Serial.available() > 0)
	{
		int bytesread = Serial.readBytesUntil('\n', buffer, BUFFER_LENGTH - 1);
		Serial.printf("Read %d bytes\r\n", bytesread);

		buffer[bytesread] = '\0';

		if (bytesread > 0) {
            Serial.printf("Received Message: %s\r\n", buffer);
            
            // Reset buffer for the next run
            memset(buffer, 0, bytesread + 1);
        }
	}
}
