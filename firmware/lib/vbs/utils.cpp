#include "utils.hpp"

TMC2209 Driver;
MS5837 DepthSensor; 
HardwareSerial &serial_stream = SERIAL_PORT;
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

void handle_max_extension()
{
	// Disable motor
	Driver.disable();
    _VBS.disable();
}

void handle_max_retraction()
{
	// Disable motor
	Driver.disable();
    _VBS.disable();
}

void send_motor_command(const std::vector<float>& motorCommand)
{
    float freq = motorCommand[0];
    int dir = (int)motorCommand[1];
    int enable = (int)motorCommand[2];
    int stepsPerPeriod = (int)freq/0.715f;

	if(enable == 0)
    {
        Driver.disable();
    }
    else
    {
        Driver.enable();
        if(dir > 0)
        {
            Driver.moveAtVelocity(stepsPerPeriod);
        }
        else if (dir < 0)
        {
           Driver.moveAtVelocity(-stepsPerPeriod);
        }
        else
        {
            Driver.moveAtVelocity(0);
        }
    }
}

void stealth_chop_automatic_tuning()
{
    // Keep the motor on and stationary for > 130 ms
    Driver.enable();
    Driver.moveAtVelocity(0);
    delay(200);

    // Move the motor at a mid RPM
    Driver.moveAtVelocity(MAX_STEPS_PER_PERIOD/2);
    delay(1000);

    Driver.disable();
    Serial.println("Stealth chop automatic tuning complete.");
}
