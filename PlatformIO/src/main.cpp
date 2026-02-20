#include <Arduino.h>

#define PWM 0
#define DIR 1
#define ENA 2

void setup() {
  pinMode(PWM, OUTPUT);
  pinMode(DIR, OUTPUT);
  pinMode(ENA, OUTPUT);

  digitalWrite(PWM, LOW);
  digitalWrite(DIR, LOW);
  digitalWrite(ENA, LOW);
}

void loop() {
  digitalWrite(PWM, HIGH);
  delay(1.5);
  digitalWrite(PWM, LOW);
  delay(1.5);
}