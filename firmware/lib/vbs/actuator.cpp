/** ------------------------ motor.cpp ------------------------

Author:			Alfie Eagleton

Date:			29/3/26

Description:
This file contains the implementation of the motor class for the VBS.
This class deals with the calculation and enforcement of the variable 
slew rate for the motor

Dependencies:	    

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
    float torqueCuruveGrad, 
    float torqueCurveInt,
    float maxMotorSpeed,
    float minMotorSpeed,
    float pistonArea
):
_gearboxEfficiency(e_g),
_motorEfficiency(e_m),
_gearRatio(GR),
_screwLead(s_l),
_screwPitchDiam(d_m),
_screwFriction(mu_s),
_holdingTorque(T_hold),
_FS(FS),
_torqueCurveGrad(torqueCuruveGrad),
_torqueCurveIntercept(torqueCurveInt),
_maxSpeedRPM(maxMotorSpeed),
_minSpeedRPM(minMotorSpeed),
_pistonArea(pistonArea)
{
    
}

float Actuator::calculate_gearbox_torque(float force, int dir)
{
    float gearTorque;

    if (dir == 1)
    {
        // Calculate torque when extending 
        gearTorque = (force * _screwPitchDiam/2)*((_screwLead + M_PI * _screwFriction * _screwPitchDiam)/(M_PI * _screwPitchDiam - _screwFriction * _screwLead));
    }
    else if (dir == -1)
    {
        // Calculate torque when retracting 
        gearTorque = (force * _screwPitchDiam/2)*((M_PI * _screwFriction * _screwPitchDiam - _screwLead)/(M_PI * _screwPitchDiam + _screwFriction * _screwLead));
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
        float maxTorque = (force * _screwPitchDiam/2)*((_screwLead + M_PI * _screwFriction * _screwPitchDiam)/(M_PI * _screwPitchDiam - _screwFriction * _screwLead));
        return maxTorque;
}

float Actuator::calculate_motor_torque(float gearboxTorque)
{
    float motorTorque = (gearboxTorque)/(_gearboxEfficiency * _gearRatio);
    return motorTorque;
}

float Actuator::calculate_max_motor_speed(float motorTorque)
{
    // Calculate rpm based on linearised torque curve
    float rpm = -_torqueCurveGrad * motorTorque + _torqueCurveIntercept;
    rpm = std::clamp(rpm, _minSpeedRPM, _maxSpeedRPM);

    // Convert to rad/s
    float omega = (rpm * 2.0 * M_PI) / 60.0;
    return omega;
}

double Actuator::calculate_slew(float speedRAD)
{
    double piston_linear_velocity = (speedRAD / _gearRatio) * (_screwLead / (2.0 * M_PI));
    double slewRateVolume = piston_linear_velocity * _pistonArea; 
    return slewRateVolume;
}

float Actuator::calculate_motor_power(float motorTorque,  float rotVel)
{
    float powerConsumption = (std::abs(motorTorque) * std::abs(rotVel) * _FS)/_motorEfficiency;
    return powerConsumption;
}

float Actuator::step(float force, int dir)
{
    float gearboxTorque = calculate_gearbox_torque(force, dir);
    float maxGearboxTorque = calculate_max_gearbox_torque(force);
    float maxMotorTorque = calculate_motor_torque(maxGearboxTorque);
    float maxMotorSpeed = calculate_max_motor_speed(maxMotorTorque);
    float maxSlew = calculate_slew(maxMotorSpeed);

    return maxSlew;

}

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

float Actuator::get_torque_curve_grad()
{
    return _torqueCurveGrad;
}

float Actuator::get_torque_curve_int()
{
    return _torqueCurveIntercept;
}

float Actuator::get_max_speed_rpm()
{
    return _maxSpeedRPM;
}
 
float Actuator::get_min_speed_rpm()
{
    return _minSpeedRPM;
}

int Actuator::get_gear_ratio()
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
