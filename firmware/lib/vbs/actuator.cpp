/** ------------------------ motor.cpp ------------------------

Author:			Alfie Eagleton

Date:			29/3/26

Description:
This file contains the implementation of the motor class for the VBS.
This class deals with the calculation and enforcement of the variable 
slew rate for the motor

Dependencies:	actuator.hpp   

**/

#include "actuator.hpp"

Actuator::Actuator(
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
_gearboxEfficiency(e_g),
_motorEfficiency(e_m),
_gearRatio(GR),
_screwPitchDiam(d_m),
_screwLead(s_l),
_screwFriction(mu_s),
_holdingTorque(T_hold),
_FS(FS),
_maxSpeedRPM(maxMotorSpeed),
_minSpeedRPM(minMotorSpeed),
_pistonArea(pistonArea),
_stepsPerRev(stepsPerRev)
{
    _enabled = true;

    _maxPistonVolume = 0.00012053;
    _pistonVolume = 0.0;
    _dir = 0;
}

float Actuator::calculate_gearbox_torque(float force, int dir)
{
    float gearTorque;

    if (dir == 1)
    {
        // Calculate torque when extending 
        gearTorque = (force * _screwPitchDiam/2)*((_screwLead + (float)M_PI * _screwFriction * _screwPitchDiam)/((float)M_PI * _screwPitchDiam - _screwFriction * _screwLead));
    }
    else if (dir == -1)
    {
        // Calculate torque when retracting 
        gearTorque = (force * _screwPitchDiam/2)*(((float)M_PI * _screwFriction * _screwPitchDiam - _screwLead)/((float)M_PI * _screwPitchDiam + _screwFriction * _screwLead));
    }
    else
    {
        gearTorque = _holdingTorque;
    }

    return gearTorque;
}

float Actuator::calculate_max_gearbox_torque(float force)
{
        // Calculate torque when extending 
        float maxTorque = (force * _screwPitchDiam/2)*((_screwLead + (float)M_PI * _screwFriction * _screwPitchDiam)/((float)M_PI * _screwPitchDiam - _screwFriction * _screwLead));
        return maxTorque;
}

float Actuator::calculate_motor_torque(float gearboxTorque)
{
    float motorTorque = (gearboxTorque)/(_gearboxEfficiency * _gearRatio);
    return motorTorque;
}

float Actuator::calculate_max_motor_speed(float depth)
{
    return (depth < 235) ? _maxSpeedRPM : _minSpeedRPM;
}

float Actuator::calculate_motor_power(float motorTorque,  float rotVel)
{
    float powerConsumption = (std::abs(motorTorque) * std::abs(rotVel) * _FS)/_motorEfficiency;
    return powerConsumption;
}

void Actuator::set_volume(int vol_mL)
{
    double vol_m3 = vol_mL/1000000;
    _pistonVolume = vol_m3;
}

void Actuator::increment_piston_volume()
{
    if(_dir == 0) return;

    double volumePerStep = (_screwLead / (_stepsPerRev * _gearRatio)) * _pistonArea;

    // Increment the piston volume by the amount moved in a single step
    double pistonVolume = (_dir > 0) ? (_pistonVolume + volumePerStep) 
                         : (_pistonVolume - volumePerStep);
    
    _pistonVolume = std::clamp(pistonVolume, -_maxPistonVolume, _maxPistonVolume);
}

void Actuator::update_direction(int dir)
{
    _dir = dir;
}

void Actuator::enable()
{
    _enabled = true;
}

void Actuator::disable()
{
    _enabled = false;
}

// double Actuator::step(float force, int dir)
// {
//     float gearboxTorque = calculate_gearbox_torque(force, dir);
//     float maxGearboxTorque = calculate_max_gearbox_torque(force);
//     float maxMotorTorque = calculate_motor_torque(maxGearboxTorque);
//     float maxMotorSpeed = calculate_max_motor_speed(maxMotorTorque);
//     double maxSlew = calculate_slew(maxMotorSpeed);

//     return maxSlew;

// }

double Actuator::get_piston_area()
{
    return _pistonArea;
}

float Actuator::get_motor_efficiency()
{
    return _motorEfficiency;
}

float Actuator::get_holding_torque()
{
    return _holdingTorque;
}

float Actuator::get_max_speed_rpm()
{
    return _maxSpeedRPM;
}
 
float Actuator::get_min_speed_rpm()
{
    return _minSpeedRPM;
}

float Actuator::get_gear_ratio()
{
    return _gearRatio;
}

float Actuator::get_gearbox_efficiency()
{
    return _gearboxEfficiency;
}

float Actuator::get_screw_lead()
{
    return _screwLead;
}

float Actuator::get_screw_pitch_diam()
{
    return _screwPitchDiam;
}

float Actuator::get_screw_friction()
{
    return _screwFriction;
}
 
float Actuator::get_factor_of_safety()
{
    return _FS;
}

bool Actuator::is_enabled()
{
    return _enabled;
}

int Actuator::get_steps_per_rev()
{
    return _stepsPerRev;
}

double Actuator::get_piston_volume()
{
    return _pistonVolume;
}

double Actuator::get_max_piston_volume()
{
    return _maxPistonVolume;
}

int Actuator::get_direction()
{
    return _dir;
}

bool Actuator::get_status()
{
    return _enabled;
}