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

// Actuator params
#define E_G 0.9f
#define E_M 0.8f
#define GEAR_RATIO 45.0f
#define D_M 0.0075f
#define S_L 0.008f
#define MU_S 0.007f
#define T_HOLD 0.4f
#define SAFETY_FACTOR 1.2f 
#define TORQUE_CURVE_GRAD 5995.0f
#define TORQUE_CURVE_INT 1978.0f
#define MAX_MOTOR_SPEED_RPM 600.0f
#define MIN_MOTOR_SPEED_RPM 130.0f
#define MAX_STEPS_PER_PERIOD 2797
#define MIN_STEPS_PER_PERIOD 606
#define PISTON_AREA 0.0029f
#define STEPS_PER_REV 200

#endif