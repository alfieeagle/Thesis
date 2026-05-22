#ifndef VBS_CONFIG_H

#define VBS_CONFIG_H

// Setup directions
#define EXTEND 1
#define RETRACT -1
#define HOLD 0

// Controller params
#define KP 1.0f      
#define KD 0.3f   
#define KI 0.7f   

// VBS Physical params
#define DT 0.1f
#define VBS_LENGTH 0.2f
#define VBS_RADIUS 0.056f
#define VBS_HALF_STROKE 0.037f

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
#define MAX_ACCELERATION 1500   
#define MIN_PULSE_WIDTH_MS 10
#define PISTON_AREA_M_2 0.0029f
#define STEPS_PER_REV 200
#define MAX_VOLUME_ONE_WAY_ML 107.0f

#define M_3_TO_ML 1000000

// Buffer setup
#define BUFFER_SIZE = 2048

// Timing
const int DEBOUNCE_TIME_MS = 2500;
const int TIMER_INTERVAL_MILLIS = 100;

// Driver parameters
const long SERIAL_BAUD_RATE = 230400; 

// Logic parameters
const float DEADZONE_THRESHOLD = 0.05f; 
const float BUOYANCY_RESOLUTION_GRAMS = 1.0f;

extern bool startup;

#endif

