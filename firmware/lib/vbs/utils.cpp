#include "utils.hpp"

MS5837 DepthSensor; 
Metro ControlTimer = Metro(TIMER_INTERVAL_MILLIS);
IntervalTimer msgTimer;
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
    noInterrupts();
	motor.disableOutputs();
    _VBS.disable();
    interrupts();
    encode_data_and_send("[INFO] Fully Extended");
}

void handle_max_retraction()
{
	// Disable motor
    noInterrupts();
	motor.disableOutputs();
    _VBS.disable();
    _VBS.set_home(true);
    interrupts();
    encode_data_and_send("[INFO] Fully retracted");
}

void timer_callback()
{
    encode_data_and_send(NULL);
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
    // int enable = std::abs(_VBS.get_piston_volume() - _VBS.get_control_volume()) < DEADZONE_THRESHOLD ? 0 : 1;

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

void step()
{
    motor.enableOutputs();
    // Step the motor
    if(motor.runSpeed())
    {
        // Update the VBS class to reflect new volume each step
        _VBS.update_volume();
    };
    
}

// Go to the fully retracted position
void homing_sequence()
{
    motor.enableOutputs();
    encode_data_and_send("[INFO] Performing homing sequence");

    // Ensure that the piston reaches the fully retracted position
    long steps = distance_to_steps(VBS_HALF_STROKE * 4);
    motor.move(steps);

    // Drive the motor to the most retracted position
    while(_VBS.get_home() == false)
    {
        if(motor.run())
        {
            _VBS.update_volume();
        }
    }

    // Set the home position
    motor.setCurrentPosition((long)0);
    digitalWrite(RESET_PIN, LOW);
    delayMicroseconds(100);
    digitalWrite(RESET_PIN, HIGH);
}

// Go to the neutrally buoyant point
void neutral_point()
{
    encode_data_and_send("[INFO] Moving to neutral position");
    motor.enableOutputs();

    // Neutrally buoyant point should be one half stroke from the 
    // fully retracted position
    long steps = distance_to_steps(VBS_HALF_STROKE);
    
    // Negative steps equals extension
    motor.moveTo(-steps);
    while(motor.distanceToGo() != 0)
    {
        if(motor.run())
        {
            _VBS.update_volume();
        }
    }

    encode_data_and_send("[INFO] At neutral position");
}

// Convert a distance in meteres to the number of steps required by the motor
long distance_to_steps(float distance_m)
{
    long steps = (distance_m/S_L) * GEAR_RATIO * STEPS_PER_REV;
    return steps;
}

void encode_data_and_send(const char* msg)
{
    // Create a clean buffer
    uint8_t local_buffer[1000];
    memset(local_buffer, 0, sizeof(local_buffer));

    // static bool is_encoding = false;
    // if (is_encoding) return;
    // is_encoding = true;

    // Setup the protobuf stream
    SystemStatus message = SystemStatus_init_zero;
    pb_ostream_t stream = pb_ostream_from_buffer(local_buffer, sizeof(local_buffer));

    // Encode the message and get the length of encoded bytes
    message.control_volume = _VBS.get_control_volume() * 1000000;  // Convert m^3 to mL
    message.depth = _VBS.get_current_depth();
    message.piston_pos = _VBS.get_piston_volume() * 1000000; // Convert m^3 to mL
    message.ref_depth = _VBS.get_reference_depth();
    message.status = _VBS.get_status();
    if(msg != NULL)
    {
        message.has_message = true;
        strncpy(message.message, msg, sizeof(message.message) - 1);
        message.message[sizeof(message.message) - 1] = '\0';
    }
    else
    {
        message.has_message = false;
    }
    
    message.has_depth = true;
    message.has_ref_depth = true;
    message.has_piston_pos = true;
    message.has_control_volume = true;
    message.has_status = true;
    

    bool status = pb_encode(&stream, SystemStatus_fields, &message);
    size_t message_length = stream.bytes_written;
        
    // Check for encoding errors
    if (!status)
    {
        // std::string base = "[ERROR] Encoding failed:";
        // std::string error = PB_GET_ERROR(&stream);
        // std::string final = base + error;
        // encode_data_and_send(final.c_str());
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
        // encode_data_and_send("[ERROR] Error encoding previous message");
    }

    // is_encoding = false;
}

void decode_data_and_read(Command* telemetry)
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
                    std::string base = "[ERROR] Decoding failed:";
                    std::string error = PB_GET_ERROR(&stream);
                    std::string final = base + error;
                    encode_data_and_send(final.c_str());
                    telemetry = NULL; // Returning empty message
                }
                 *telemetry = message;
            }
        }
    }
}
