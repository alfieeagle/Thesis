#include "utils.hpp"

void handle_max_extension()
{
	// Disable motor
	Driver.disable();
}

void handle_max_retraction()
{
	// Disable motor
	Driver.disable();
}

void send_motor_command(std::vector<float> motorCommand)
{
	digitalWrite(EN_PIN, motorCommand[2]);
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
