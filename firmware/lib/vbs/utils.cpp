#include "utils.hpp"

MS5837 DepthSensor; 
Metro ControlTimer = Metro(TIMER_INTERVAL_MILLIS);
VBS _VBS(
    KP,
    KD,
    KI,
    DT,
    VBS_LENGTH,
    VBS_RADIUS,
    E_G,
    E_M,
    GEAR_RATIO,
    D_M,
    S_L,
    MU_S,
    T_HOLD,
    SAFETY_FACTOR,
    MAX_MOTOR_SPEED_RPM,
    MIN_MOTOR_SPEED_RPM,
    PISTON_AREA,
    STEPS_PER_REV
    );

AccelStepper motor(1, STEP_PIN, DIR_PIN);

void handle_max_extension()
{
	// Disable motor
	motor.disableOutputs();
    _VBS.disable();
}

void handle_max_retraction()
{
	// Disable motor
	motor.disableOutputs();
    _VBS.disable();

    // Set the home position and trigger the reset pin
    _VBS.set_home(true);
    digitalWrite(RESET_PIN, LOW);
}

void send_motor_command(const std::vector<float>& motorCommand)
{
    // Define directions
    float extend = 1.0f;
    float retract = -1.0f;

    // Extract the frequency and direction from the command
    float freq = motorCommand[0];
    float dir = motorCommand[1];

    // Only enable the piston if it's outside the deadzone
    int enable = std::abs(_VBS.get_piston_volume() - _VBS.get_control_volume()) < DEADZONE_THRESHOLD ? 0 : 1;

	if(enable == 0)
    {
        motor.disableOutputs();
    }
    else
    {
        motor.enableOutputs();
        if(dir == extend)
        {
            motor.setSpeed(-freq);
        }
        else if (dir == retract)
        {
           motor.setSpeed(-freq);
        }
        else
        {
            motor.setSpeed(0);
        }
    }
}

void step()
{
    // Step the motor
    motor.runSpeed();

    // Update the VBS class to reflect new volume
    _VBS.update_volume();
}

// Go to the fully retracted position
void homing_sequence()
{
    motor.enableOutputs();

    // Ensure that the piston reaches the fully retracted position
    long steps = distance_to_steps(VBS_HALF_STROKE * 4);
    motor.move(steps);

    // Drive the motor to the most retracted position
    while(_VBS.get_home() == false)
    {
        motor.run();
    }

    // Set the home position
    motor.setCurrentPosition(0);
}

// Go to the neutrally buoyant point
void neutral_point()
{
    motor.enableOutputs();

    // Neutrally buoyant point should be one half stroke from the 
    // fully retracted position
    long steps = distance_to_steps(VBS_HALF_STROKE);
    
    // Negative steps equals extension
    motor.moveTo(-steps);
    motor.runToPosition();
}

// Convert a distance in meteres to the number of steps required by the motor
long distance_to_steps(float distance_m)
{
    long steps = (distance_m/S_L)*GEAR_RATIO*STEPS_PER_REV;
    return steps;
}