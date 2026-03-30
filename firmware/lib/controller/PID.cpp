/** ------------------------ PID.cpp ------------------------

Author:         Alfie Eagleton

Date:           25/3/26

Description:
This file contains the implementation of the PID controller
for the variable buoyancy system.

Dependencies:   PID.hpp 

**/

#include "PID.hpp"

PID::PID(float kp, float kd, float ki):
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
    _cutoffFrequency = 30.0;

    // Calculate smoothing factor based on cutoff frequency
    float y = 1 - (float)std::cos(_cutoffFrequency);
    _alpha = -y + (float)std::sqrt(std::pow(y, 2) + 2 * y);
}

void PID::compute_error(float ref, float signal)
{
    _error = ref - signal;
}

void PID::calculate_integral_error(float dt)
{
    _integralError += _error * dt;

    _integralError = std::clamp(_integralError, -_maxIntegralError, _maxIntegralError);
}

void PID::calculate_derivative_error(float dt) {
    float raw_derivative = (_error - _prevError) / dt;

    _derivativeError = (1.0f - _alpha) * _derivativeError + _alpha * raw_derivative;
    
    _prevError = _error;
}

double PID::compute_control_signal()
{
    float u = (_kp * _error) + (_kd * _derivativeError) + (_ki * _integralError);

    return u = (float)std::clamp(u, -_saturation, _saturation);
}

double PID::step(float ref, float signal, float dt)
{
    _elapsedTime += dt;

    compute_error(ref, signal);
    calculate_integral_error(dt);
    calculate_derivative_error(dt);

    return compute_control_signal();
}

void PID::set_saturation(float sat)
{
    _saturation = sat;
    _maxIntegralError = _saturation * 0.5f;
}