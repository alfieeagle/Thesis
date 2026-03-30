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
    double kp, 
    double kd, 
    double ki, 
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
    float torqueCurveGrad, 
    float torqueCurveInt,
    float maxMotorSpeed,
    float minMotorSpeed,
    float pistonArea
):
_controller(kp, kd, ki),
_actuator(e_g, e_m, GR, d_m, s_l, mu_s,T_hold, FS, torqueCurveGrad, torqueCurveInt, maxMotorSpeed, minMotorSpeed, pistonArea),
_dt(dt),
_length(length),
_radius(radius)
{
    // #ifdef CORE_TEENSY
    //     instance_ptr = this;
    //     _controllerTimer.begin(vbs_timer_isr, 100000); 
    // #endif

    _referenceDepth = -2.0;
    _currentDepth = _referenceDepth;

    // Set volume
    _volume = (float)std::pow(radius,2) * (float)M_PI * _length;
    _maxPistonVolume = 0.00012053;
    _pistonVolume = 0.0;
    _prevPistonVolume = 0.0;

    // Set PID saturation based on max volume
    _controller.set_saturation(1.0);

    _g = 9.81f;
    _density = 1025.0f;

}

void VBS::step(float dt)
{
    double controllerVolume = _controller.step(_referenceDepth, get_current_depth(), dt) * _maxPistonVolume;

    float depthForce = (float)std::abs(get_current_depth()) * _g * _density * (float)_actuator.get_piston_area();
    int dir;

    // Check the direction of the piston to calculate the correct torque
    if(_pistonVolume - _prevPistonVolume > 0)
    {
        dir = 1;
    }
    else if (_pistonVolume - _prevPistonVolume < 0)
    {
        dir = -1;
    }
    else
    {
        dir = 0;
    }

    // Calculate the max slew rate based on the depth in order to rate limit the actuator
    float maxSlewRate = _actuator.step(depthForce, dir);
    if(std::abs(controllerVolume - _pistonVolume)/dt > maxSlewRate)
    {
        _prevPistonVolume = _pistonVolume;
        _pistonVolume = _prevPistonVolume + maxSlewRate * dt;
    }
    else if (std::abs(controllerVolume - _pistonVolume)/dt < -maxSlewRate)
    {
        _prevPistonVolume = _pistonVolume;
        _pistonVolume = _prevPistonVolume - maxSlewRate * dt;
    }
    else
    {
        _prevPistonVolume = _pistonVolume;
        _pistonVolume = controllerVolume;
    }
}

// Update VBS depth
void VBS::update_depth(float depth)
{
    _currentDepth = depth;
}

// Update VBS current volume
double VBS::get_piston_volume()
{
    return _pistonVolume;
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