/** ------------------------ vbs.cpp ------------------------

Author:			Alfie Eagleton

Date:			20/2/26

Description:
This file contains the implementation of the VBS class.

Dependencies:	vbs.hpp

**/

#include "vbs.hpp"

// // Create static pointer for use with C style interrupts
// static VBS* instance_ptr = nullptr;

// // The ISR (Interrupt Service Routine)
// void vbs_timer_isr() {
//     if (instance_ptr) instance_ptr->step();
// }

VBS::VBS(
    float kp, 
    float kd, 
    float ki, 
    float dt, 
    float length, 
    float radius,
    float e_g, 
    float e_m, 
    float GR, 
    float d_m, 
    float s_l, 
    float mu_s, 
    float T_hold, 
    float FS, 
    float maxMotorSpeed,
    float minMotorSpeed,
    float pistonArea,
    int stepsPerRev
):
_controller(kp, kd, ki),
_actuator(e_g, e_m, GR, d_m, s_l, mu_s,T_hold, FS, maxMotorSpeed, minMotorSpeed, pistonArea, stepsPerRev),
_dt(dt),
_length(length),
_radius(radius),
_motorCommand(3)
{
    _referenceDepth = -0.4;
    _currentDepth = _referenceDepth;

    // Set volume
    _volume = (float)std::pow(radius,2) * (float)M_PI * _length;

    // Set PID saturation based on max volume
    _controller.set_saturation(1.0);

    _g = 9.81f;
    _density = 1025.0f;

    _motorCommand[0] = 2000.0f;
    _motorCommand[1] = 1.0f;
    _motorCommand[2] = 1.0f;

    _home = false;
}

void VBS::update_control(float dt) {
    // Get control signal
    _controlVolume = _controller.step(_referenceDepth, get_current_depth(), dt) * _actuator.get_max_piston_volume();
}

void VBS::set_volume(float vol_mL)
{
    _actuator.set_volume(vol_mL);
}

void VBS::set_reference_depth(float ref_depth)
{
    _referenceDepth = ref_depth;
}

int VBS::get_direction()
{
    return _actuator.get_direction();
}

void VBS::update_motor_command() {
    std::vector<float> motor_command;

    // Check max speed
    float maxSpeedRPM = _actuator.calculate_max_motor_speed(get_current_depth());
    
    // Determine frequency from max speed
    float freq = (maxSpeedRPM/60) * _actuator.get_steps_per_rev();
    motor_command.push_back(freq);

    double requestedChange = (_controlVolume - _actuator.get_piston_volume()) * 1000000;
    motor_command.push_back(requestedChange);

    int dir = HOLD;
    
    if(requestedChange > BUOYANCY_RESOLUTION_GRAMS) {
        dir = EXTEND;
    } else if(requestedChange < -(BUOYANCY_RESOLUTION_GRAMS)) {
        dir = RETRACT;
    } else {
        dir = HOLD;
    }
    
    motor_command.push_back((float)dir);
    
    _motorCommand = motor_command;
}

void VBS::update_direction(int dir)
{
    _actuator.update_direction(dir);
}

void VBS::update_depth(float depth)
{
    _currentDepth = depth;
}

void VBS::update_volume(long absolute_steps)
{
    _actuator.increment_piston_volume(absolute_steps);
}

float VBS::get_reference_depth()
{
    return _referenceDepth;
}

float VBS::get_current_depth()
{
    return _currentDepth;
}

float VBS::get_vbs_volume()
{
    return _volume;
}

double VBS::get_control_volume()
{
    return _controlVolume;
}

double VBS::get_piston_volume()
{
    return _actuator.get_piston_volume();
}

std::vector<float> VBS::get_motor_command()
{
    return _motorCommand;
}

void VBS::enable()
{
    _actuator.enable();
}

void VBS::disable()
{
    _actuator.disable();
}

void VBS::set_home(bool flag)
{
    _home = flag;
}

bool VBS::get_home()
{
    return _home;
}

bool VBS::is_enabled()
{
    return _actuator.is_enabled();
}