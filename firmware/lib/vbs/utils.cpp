#include "utils.hpp"

MS5837 DepthSensor; 
Metro ControlTimer = Metro(TIMER_INTERVAL_MILLIS);
IntervalTimer msgTimer, stepTimer;
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
    motor.stop();
    motor.disableOutputs();
    _VBS.disable();
    _VBS.set_volume(MAX_VOLUME_ONE_WAY_ML);
    interrupts();
    encode_data_and_send("[INFO] Fully Extended");
}

void handle_max_retraction()
{
	// Disable motor
    noInterrupts();
    motor.stop();
	motor.disableOutputs();
    _VBS.disable();
    _VBS.set_volume(-MAX_VOLUME_ONE_WAY_ML);
    _VBS.set_home(true);
    interrupts();
    encode_data_and_send("[INFO] Fully retracted");
}

void msg_callback()
{
    encode_data_and_send(NULL);
}

void send_motor_command(const std::vector<float>& motorCommand)
{

    // Extract the frequency and direction from the command
    float freq = motorCommand[0];
    float dir = motorCommand[1];
    if(dir != _VBS.get_direction())
    {
        motor.stop();
        delay(100);
    }

    // Only enable the piston if it's outside the deadzone
    int deadzone = std::abs(_VBS.get_current_depth() - _VBS.get_reference_depth()) < DEADZONE_THRESHOLD ? 1 : 0;

    if(!deadzone && _VBS.is_enabled() == true)
    {
        if(dir == EXTEND)
        {
            motor.enableOutputs();
            _VBS.update_direction(EXTEND);
            motor.setSpeed(-freq);
        }
        else if (dir == RETRACT)
        {
            motor.enableOutputs();
            _VBS.update_direction(RETRACT);
            motor.setSpeed(freq);
        }
        else
        {
            motor.enableOutputs();
            _VBS.update_direction(HOLD);
            motor.setSpeed(0);
        }
    }
    else
    {
        _VBS.update_direction(HOLD);
        motor.setSpeed(0);
        _VBS.disable();
        motor.disableOutputs();
    }
}

void step()
{
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
    if(digitalRead(LIM_RET) == LOW)
    {
        motor.stop();
        motor.setCurrentPosition((long)0);
        _VBS.set_home(true);
        _VBS.set_volume(-MAX_VOLUME_ONE_WAY_ML);
        return;
    }
    motor.enableOutputs();
    encode_data_and_send("[INFO] Performing homing sequence");

    // Ensure that the piston reaches the fully retracted position
    long steps = distance_to_steps(VBS_HALF_STROKE * 2.5);
    motor.move(steps);

    _VBS.update_direction(RETRACT);

    // Drive the motor to the most retracted position
    while(_VBS.get_home() == false)
    {
        if(motor.run())
        {
            _VBS.update_volume();
        }
    }

    motor.setCurrentPosition((long)0);
    digitalWrite(RESET_PIN, LOW);
    delayMicroseconds(100);
    digitalWrite(RESET_PIN, HIGH);

    detachInterrupt(digitalPinToInterrupt(LIM_RET));
    motor.enableOutputs();
    _VBS.enable();
    motor.move(-5000);
    while(motor.distanceToGo() != 0)
    {
        motor.run();
    }
    motor.disableOutputs();
    _VBS.disable();
    attachInterrupt(digitalPinToInterrupt(LIM_RET), handle_max_retraction, FALLING);
    encode_data_and_send("[INFO] At home position");
}

// Go to the neutrally buoyant point
void neutral_point()
{
    encode_data_and_send("[INFO] Moving to neutral position");
    motor.enableOutputs();

    // Neutrally buoyant point should be one half stroke from the 
    // fully retracted position
    long steps = distance_to_steps(VBS_HALF_STROKE);
    motor.move(-steps);

    _VBS.update_direction(EXTEND);
    
    // Negative steps equals extension
    while(motor.distanceToGo() != 0)
    {
        if(motor.run())
        {
            _VBS.update_volume();
        }
    }

    motor.stop();
    motor.disableOutputs();
    _VBS.disable();

    encode_data_and_send("[INFO] At neutral position");
}

// Convert a distance in meteres to the number of steps required by the motor
long distance_to_steps(float distance_m)
{
    long steps = (distance_m/S_L) * GEAR_RATIO * STEPS_PER_REV;
    return steps;
}

int encode_data_and_send(const char* msg)
{
    // Create a clean buffer
    uint8_t local_buffer[1000];
    memset(local_buffer, 0, sizeof(local_buffer));

    static bool is_encoding = false;
    if (is_encoding) return 1;
    is_encoding = true;

    // Setup the protobuf stream
    SystemStatus message = SystemStatus_init_zero;
    pb_ostream_t stream = pb_ostream_from_buffer(local_buffer, sizeof(local_buffer));

    // Encode the message and get the length of encoded bytes
    message.control_volume = _VBS.get_control_volume() * 1000000;  // Convert m^3 to mL
    message.depth = _VBS.get_current_depth();
    message.piston_pos = _VBS.get_piston_volume() * 1000000; // Convert m^3 to mL
    message.ref_depth = _VBS.get_reference_depth();
    message.status = _VBS.is_enabled();
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
        std::string base = "[ERROR] Encoding failed:";
        std::string error = PB_GET_ERROR(&stream);
        std::string final = base + error;
        encode_data_and_send(final.c_str());
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
        encode_data_and_send("[ERROR] Error encoding previous message");
        return 1;
    }
    is_encoding = false;
    return 0;
}

int decode_data_and_read(Command* cmd)
{
    uint8_t startbyte;
    
    // Only read one message per call
    if (Serial.readBytes((char*)&startbyte, 1) > 0) 
    {
        // Wait for the correct start byte
        if (startbyte == 0xAA) 
        {
            // Read the length
            uint8_t len;
            if (Serial.readBytes((char*)&len, 1) > 0) 
            {
                // Validate length to prevent buffer overflow
                if (len > 256)
                {
                    encode_data_and_send("[ERROR] Message length exceeds buffer size");
                    return 1;
                }

                // Create the buffer
                uint8_t local_buffer[256];
                memset(local_buffer, 0, sizeof(local_buffer));
            
                // Decode the message
                int bytes_read = Serial.readBytes((char*)local_buffer, len);
                
                if (bytes_read != len)
                {
                    encode_data_and_send("[ERROR] Incomplete message received");
                    return 1;
                }

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
                    return 1;
                }
                
                *cmd = message;
                return 0;  // Success
            }
            else
            {
                encode_data_and_send("[ERROR] Failed to read message length");
                return 1;
            }
        }
        else
        {
            // Invalid start byte - skip it
            return 1;
        }
    }
    
    // No data available
    return 1;
}

void read_serial()
{
    Command incoming;

    // Check if the serial port is open
    int result = decode_data_and_read(&incoming);
    if (result == 0)
    {
        if(incoming.enable == true)
        {
            motor.enableOutputs();
            _VBS.enable();
        }
        else
        {
            motor.stop();
            motor.disableOutputs();
            _VBS.disable();
        }
        _VBS.set_reference_depth(incoming.target_depth);

    } 
    else
    {
        encode_data_and_send("[ERROR] Issue reading serial sent from PC");
    }
}