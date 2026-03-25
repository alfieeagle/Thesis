/** ------------------------ PID.cpp ------------------------

Author:			Alfie Eagleton

Date:			25/3/26

Description:
This file contains the implementation of the PID controller
for the variable buoyancy system.

Dependencies:	PID.hpp 

**/

#include "PID.hpp"

PID::PID(float kp, float kd, float ki, float time_const):
_kp(kp),
_kd(kd),
_ki(ki),
_time_const(time_const)
{
    _error = 0.0f;
    _prev_error = 0.0f;
    _integral_error = 0.0f;
    _deriative_error = 0.0f;
    _max_integral_error = 10.0f;
    _elapsed_time = 0.0f;
}

float PID::computeError(float ref, float signal)
{
    _error = ref - signal;
    return _error;
}

float PID::integrateError(float error)
{
    float final_error;

    if(_integral_error < _max_integral_error)
    {
        _integral_error += _error;
        final_error = _integral_error;
    }
    else
    {
        _integral_error = _max_integral_error;
        final_error = _integral_error;
    }

    return final_error;
}

float PID::calculateDerivative(float error)
{
    float final_error;

    if(_elapsed_time == 0.0)
    {
        _deriative_error = 0.0f;
        final_error = _deriative_error;
    }
    else
    {
        _deriative_error = (_error - _prev_error)/_time_const;
        final_error = _deriative_error;
    }

    return final_error;
}

float PID::computeControlSignal()
{
    float u = (_kp * _error) + (_kd * _deriative_error) + (_ki * _integral_error);
}