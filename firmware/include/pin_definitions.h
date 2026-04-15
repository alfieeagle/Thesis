#ifndef PIN_DEFINITIONS_H

#define PIN_DEFINITIONS_H

#define INDEX            2  // Index each full step of the motor
#define LIM_RET          3  // Retracted limit switch
#define LIM_EXT          4  // Extended limit switch
#define EN_PIN           5  // Enable driver
#define DIR_PIN          22 // Direction
#define STEP_PIN         23 // Step
#define SERIAL_PORT Serial1 // HardwareSerial port
#define DRIVER_ADDRESS 0b00 // TMC2209 Driver address according to MS1 and MS2
#define R_SENSE 0.11f // Internal resistance

#endif
