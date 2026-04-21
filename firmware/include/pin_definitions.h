#ifndef PIN_DEFINITIONS_H

#define PIN_DEFINITIONS_H

#define LIM_EXT          3  // Extended limit switch
#define LIM_RET          4  // Retracted limit switch
#define EN_PIN           5  // Enable driver
#define DIR_PIN          22 // Direction
#define STEP_PIN         23 // Step

// Timing
const int DEBOUNCE_TIME_MS = 200;
const int ACTUATOR_TIMER_INTERVAL_MILLIS = 10;
const int CONTROL_TIMER_INTERVAL_MILLIS = 100;

// Driver parameters
const long SERIAL_BAUD_RATE = 500000; 

// Logic parameters
const float DEADZONE_THRESHOLD = 0.01f;

#endif
