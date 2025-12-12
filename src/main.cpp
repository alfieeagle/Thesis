#include <Arduino.h>
/**
 * Teensy 4.1 PWM Example Program (Arduino Framework)
 * * This sketch generates a PWM signal on Pin 5, creating a 'breathing' 
 * or dimming effect by slowly fading the LED brightness up and down.
 * * Target Board: Teensy 4.1 (IMXRT1062)
 */

// Define the pin we will use for PWM output
const int PWM_PIN = 5; 

// Variables for the fade effect
int brightness = 0;   // How bright the LED is
int fadeAmount = 5;   // How many units to change the brightness each loop

// The setup routine runs once when the Teensy starts up
void setup() {
  // Pin 5 is a valid PWM pin on the Teensy 4.1
  // We don't explicitly need to set it as OUTPUT because analogWrite() handles it.
  // However, digitalWrite/analogWrite pins must be supported by the board.
  // Optional: Start Serial communication for debugging
  Serial.begin(115200); 
  delay(100);
  Serial.println("Teensy 4.1 PWM Fade Program Initialized.");
}

// The loop routine runs over and over again forever
void loop() {
  // 1. Write the current brightness value (0-255) to the PWM pin.
  // analogWrite uses 8-bit resolution (0-255) by default on Teensy.
  analogWrite(PWM_PIN, brightness);

  // 2. Change the brightness for the next time through the loop
  brightness = brightness + fadeAmount;

  // 3. Reverse the direction of the fade at the limits
  // If we reach the maximum brightness (255) or minimum brightness (0),
  // we reverse the fadeAmount to fade in the opposite direction.
  if (brightness <= 0 || brightness >= 255) {
    fadeAmount = -fadeAmount;
  }
  
  // 4. Wait for 30 milliseconds to control the speed of the fade effect
  // A lower delay makes the fade faster.
  delay(30); 
}