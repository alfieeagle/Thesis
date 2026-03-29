/** ------------------------ motor.cpp ------------------------

Author:			Alfie Eagleton

Date:			29/3/26

Description:
This file contains the implementation of the motor class for the VBS.
This class deals with the calculation and enforcement of the variable 
slew rate for the motor

Dependencies:	    

**/

#include "ros_wrapper/actuator.hpp"

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

    _gearboxTorque = gearTorque;
    return gearTorque;
}

float Actuator::calculate_max_gearbox_torque(float force)
{
        // Calculate torque when extending 
        _maxTorque = (force * _screwPitchDiam/2)*((_screwLead + M_PI * _screwFriction * _screwPitchDiam)/(M_PI * _screwPitchDiam - _screwFriction * _screwLead));
        return _maxTorque;
}

float Actuator::calculate_motor_torque(float gearboxTorque)
{
    _motorTorque = (gearboxTorque)/(_gearboxEfficiency * _gearRatio);
    return _motorTorque;
}

float Actuator::calculate_max_motor_speed(float motorTorque)
{
    // Calculate rpm based on linearised torque curve
    float rpm = -_torqueCurveGrad * motorTorque + _torqueCurveIntercept;
    rpm = std::clamp(rpm, _minSpeedRPM, _maxSpeedRPM);

    // Convert to rad/s
    float omega = (rpm * _screwLead)/60;
    _motorSpeedRAD = omega;
    return omega;
}

float Actuator::calculate_slew(float speedRAD, float pistonArea)
{
    float slewRate = (speedRAD/_gearRatio) * pistonArea;
    return slewRate;
}

float Actuator::calculate_motor_power(float motorTorque,  float rotVel)
{
    _powerConsumption = (std::abs(motorTorque) * std::abs(rotVel) * _FS)/_motorEfficiency;
    return _powerConsumption;
}

float Actuator::step(float force, int dir)
{
    float gearboxTorque = calculate_gearbox_torque(force, dir);
    float maxGearboxTorque = calculate_max_gearbox_torque(force);
    float maxMotorTorque = calculate_motor_torque(maxGearboxTorque);
    float maxMotorSpeed = calculate_max_motor_speed(maxMotorTorque);
    float maxSlew = calculate_slew(maxMotorSpeed, _pistonArea);

    return maxSlew;

}

float Actuator::get_piston_area()
{
    return _pistonArea;
}
