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

Command latest_command = Command_init_zero;

void handle_max_extension()
{
	// Disable motor
	motor.disableOutputs();
    _VBS.disable();
    Serial.println("Fully Extended");
}

void handle_max_retraction()
{
	// Disable motor
	motor.disableOutputs();
    _VBS.disable();
    _VBS.set_home(true);
    Serial.println("Fully retracted");
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
    digitalWrite(RESET_PIN, LOW);
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

int encode_data_and_send()
{
    // Create the buffer
    uint8_t local_buffer[256];

    // Setup the protobuf stream
    SystemStatus message = SystemStatus_init_zero;
    pb_ostream_t stream = pb_ostream_from_buffer(local_buffer, sizeof(local_buffer));

    // Encode the message and get the length of encoded bytes
    message.control_volume = _VBS.get_control_volume(); 
    message.depth = _VBS.get_current_depth();
    message.piston_pos = _VBS.get_piston_volume();
    message.ref_depth = _VBS.get_reference_depth();
    message.status = _VBS.get_status();

    message.has_depth = true;
    message.has_ref_depth = true;
    message.has_piston_pos = true;
    message.has_control_volume = true;
    message.has_status = true;
    message.has_message = false;
    // strcpy(message.message, msg);

    bool status = pb_encode(&stream, SystemStatus_fields, &message);
    size_t message_length = stream.bytes_written;
        
    // Check for encoding errors
    if (!status)
    {
        Serial.printf("Encoding failed on teensy: %s\n", PB_GET_ERROR(&stream));
        return 1;
    }

    // Write the start byte, length and encoded message to the serial port
    char startByte = 0xAA;
    char len = (uint8_t)message_length;
    Serial.write(startByte);
    Serial.write(len);
    int num_bytes = Serial.write(local_buffer, message_length);

    // Check for writing errors
    if(num_bytes < 0)
    {
        Serial.println("Error writing to device from teensy.");
        return 1;
    }

    return 0;
}

int decode_data_and_read(Command* telemetry)
{
    uint8_t startbyte;
    
    while (Serial.readBytes((char*)&startbyte, 1) > 0) 
    {
        // Wait for the correct start byte
        if (startbyte == 0xAA) 
        {
            // Read the length
            char len;
            if (Serial.readBytes(&len, 1) > 0) 
            {
                // Create the buffer
                uint8_t local_buffer[256];
            
                // Decode the message
                Serial.readBytes((char*)local_buffer, len);
                Command message = Command_init_zero;
                pb_istream_t stream = pb_istream_from_buffer(local_buffer, len);
                
                bool status = pb_decode(&stream, Command_fields, &message);

                // Check for decode error
                if(!status)
                {
                    Serial.printf("Decoding failed on teensy: %s\n", PB_GET_ERROR(&stream));
                    telemetry = NULL; // Returning empty message
                    return 1;
                }
                 *telemetry = message;
            }
        }
    }
    return 0;
}
