#ifndef PIN_DEFINITIONS_H

#define PIN_DEFINITIONS_H

#define INDEX            2  // Index each full step of the motor
#define LIM_EXT          3  // Extended limit switch
#define LIM_RET          4  // Retracted limit switch
#define EN_PIN           5  // Enable driver
#define DIR_PIN          22 // Direction
#define STEP_PIN         23 // Step
#define SERIAL_PORT Serial1 // HardwareSerial port
#define DRIVER_ADDRESS 0b00 // TMC2209 Driver address according to MS1 and MS2
#define R_SENSE 0.11f // Internal resistance

// Timing
int const DEBOUNCE_TIME = 50;
int const ACTUATOR_TIMER_INTERVAL_MILLIS = 10;
int const CONTROL_TIMER_INTERVAL_MILLIS = 100;

// Driver parameters
const long SERIAL_BAUD_RATE = 500000;
const int RUN_CURRENT_PERCENT = 50;
const int HOLD_CURRENT_PERCENT = 50;
const int STALL_GUARD_THRESHOLD = 10;

// Logic parameters
const float DEADZONE_THRESHOLD = 0.01f;

#endif
