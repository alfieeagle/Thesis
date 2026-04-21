#ifndef VBS_CONFIG_H

#define VBS_CONFIG_H

// Controller params
#define KP 1.5f
#define KD 6.0f
#define KI 0.01f

// VBS Physical params
#define DT 0.1f
#define VBS_LENGTH 0.2f
#define VBS_RADIUS 0.056f
#define VBS_HALF_STROKE 0.043f

// Actuator params
#define E_G 0.9f
#define E_M 0.8f
#define GEAR_RATIO 45.0f
#define D_M 0.0075f
#define S_L 0.008f
#define MU_S 0.007f
#define T_HOLD 0.4f
#define SAFETY_FACTOR 1.2f 
#define MAX_MOTOR_SPEED_RPM 600
#define MIN_MOTOR_SPEED_RPM 130
#define MAX_MOTOR_STEPS_SEC 2000
#define MIN_MOTOR_STEPS_SEC 433
#define MAX_STEPS_PER_PERIOD 2797
#define MIN_STEPS_PER_PERIOD 606
#define MAX_ACCELERATION 2000
#define MIN_PULSE_WIDTH_MS 10
#define PISTON_AREA 0.0029f
#define STEPS_PER_REV 200


#endif

