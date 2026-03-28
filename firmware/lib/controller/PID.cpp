/** ------------------------ PID.cpp ------------------------

Author:         Alfie Eagleton

Date:           25/3/26

Description:
This file contains the implementation of the PID controller
for the variable buoyancy system.

Dependencies:   PID.hpp 

**/

#include "ros_wrapper/PID.hpp"

PID::PID(double kp, double kd, double ki):
_kp(kp),
_kd(kd),
_ki(ki)
{
    _error = 0.0;
    _prevError = 0.0;
    _integralError = 0.0;
    _derivativeError = 0.0;
    _maxIntegralError = 0.0;
    _saturation = 1.0;
    _elapsedTime = 0.0f;
}

void PID::compute_error(float ref, float signal)
{
    _error = ref - signal;
}

void PID::integrate_error()
{
    _integralError += _error;

    _integralError = std::clamp(_integralError, -_maxIntegralError, _maxIntegralError);
}

void PID::calculate_error_derivative(float dt)
{
    if(_elapsedTime == 0.0)
    {
        _derivativeError = 0.0f;
    }
    else
    {
        _derivativeError = (_error - _prevError) / dt;
        _prevError = _error;
    }

}

double PID::compute_control_signal()
{
    double u = (_kp * _error) + (_kd * _derivativeError) + (_ki * _integralError);

    return u = std::clamp(u, -_saturation, _saturation);
}

double PID::step(float ref, float signal, float dt)
{
    _elapsedTime += dt;

    compute_error(ref, signal);
    integrate_error();
    calculate_error_derivative(dt);

    return compute_control_signal();
}

void PID::set_saturation(double sat)
{
    _saturation = sat;
    _maxIntegralError = _saturation * 0.5;
}