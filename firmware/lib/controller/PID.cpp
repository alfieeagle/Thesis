/** ------------------------ PID.cpp ------------------------

Author:         Alfie Eagleton

Date:           25/3/26

Description:
This file contains the implementation of the PID controller
for the variable buoyancy system.

Dependencies:   PID.hpp 

**/

#include "ros_wrapper/PID.hpp"

PID::PID(float kp, float kd, float ki, float timeConst):
_kp(kp),
_kd(kd),
_ki(ki),
_timeConst(timeConst)
{
    _error = 0.0f;
    _prevError = 0.0f;
    _integralError = 0.0f;
    _derivativeError = 0.0f;
    _maxIntegralError = 10;
    _elapsedTime = 0.0f;
    _ref = 0.0f;
}

float PID::compute_error(float ref, float signal)
{
    _error = ref - signal;
    return _error;
}

float PID::integrate_error()
{
    float finalError;

    if(_integralError < _maxIntegralError)
    {
        _integralError += _error;
        finalError = _integralError;
    }
    else
    {
        _integralError = _maxIntegralError;
        finalError = _integralError;
    }

    return finalError;
}

float PID::calculate_error_derivative()
{
    float finalError;

    if(_elapsedTime == 0.0)
    {
        _derivativeError = 0.0f;
        _prevError = 0.0f;
        finalError = _derivativeError;
    }
    else
    {
        _derivativeError = (_error - _prevError) / _timeConst;
        _prevError = _error;
        finalError = _derivativeError;
    }

    return finalError;
}

float PID::compute_control_signal()
{
    // Summing the terms using the camelCase internal variables
    float u = (_kp * _error) + (_kd * _derivativeError) + (_ki * _integralError);
    return u; 
}

float PID::step(float ref, float signal)
{
    float controlSignal;

    _ref = ref;

    _error = compute_error(_ref, signal);
    _integralError = integrate_error();
    _derivativeError = calculate_error_derivative();

    controlSignal = compute_control_signal();
    return controlSignal;
}