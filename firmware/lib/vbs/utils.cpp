#include "utils.hpp"

MS5837 DepthSensor; 
Metro ActuatorTimer = Metro(ACTUATOR_TIMER_INTERVAL_MILLIS);
Metro ControlTimer = Metro(CONTROL_TIMER_INTERVAL_MILLIS);
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
	digitalWrite(EN_PIN, HIGH);
    _VBS.disable();
}

void handle_max_retraction()
{
	// Disable motor
	digitalWrite(EN_PIN, HIGH);
    _VBS.disable();
    _VBS.set_home(true);
}

// void send_motor_command(const std::vector<float>& motorCommand)
// {
//     float freq = motorCommand[0];
//     int dir = (int)motorCommand[1];
//     int enable = (int)motorCommand[2];
//     float period = 1/freq;

// 	if(enable == 0)
//     {
//         digitalWrite(EN_PIN, HIGH);
//     }
//     else
//     {
//         digitalWrite(EN_PIN, LOW);
//         if(dir > 0)
//         {
//             digitalWrite(DIR_PIN, HIGH);
//             digital
//         }
//         else if (dir < 0)
//         {
//            Driver.moveAtVelocity(-stepsPerPeriod);
//         }
//         else
//         {
//             Driver.moveAtVelocity(0);
//         }
//     }
// }

// void stealth_chop_automatic_tuning()
// {
//     // Keep the motor on and stationary for > 130 ms
//     Driver.enable();
//     Driver.moveAtVelocity(0);
//     delay(200);

//     // Move the motor at a mid RPM
//     Driver.moveAtVelocity(MAX_STEPS_PER_PERIOD/2);
//     delay(1000);

//     Driver.disable();
//     Serial.println("Stealth chop automatic tuning complete.");
// }

void homing_sequence()
{
    digitalWrite(EN_PIN, LOW);
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

void neutral_point()
{
    digitalWrite(EN_PIN, LOW);
    long steps = distance_to_steps(VBS_HALF_STROKE);
    motor.moveTo(-steps);
    motor.runToPosition();
}

long distance_to_steps(float distance_m)
{
    long steps = (distance_m/S_L)*GEAR_RATIO*STEPS_PER_REV;
    return steps;
}