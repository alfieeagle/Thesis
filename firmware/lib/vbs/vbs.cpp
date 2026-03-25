/** ------------------------ vbs.cpp ------------------------

Author:			Alfie Eagleton

Date:			20/2/26

Description:
This file contains the implementation of the VBS class.

Dependencies:	vbs.hpp

**/

#include "ros_wrapper/vbs.hpp"

// Create static pointer for use with C style interrupts
static VBS* instance_ptr = nullptr;

// The ISR (Interrupt Service Routine)
void vbs_timer_isr() {
    if (instance_ptr) instance_ptr->step();
}

VBS::VBS(float cutoffFrequency, float kp, float kd, float ki, float timeConst):
_Controller(kp, kd, ki, timeConst),
_cutoffFrequency(cutoffFrequency)
{
    #ifdef CORE_TEENSY
        instance_ptr = this;
        _controllerTimer.begin(vbs_timer_isr, 100000); 
    #endif

    _referenceDepth = -5.0f;

    // Calculate smoothing factor based on cutoff frequency
    float y = 1 - std::cos(_cutoffFrequency);
    _alpha = -y + std::sqrt(std::pow(y, 2) + 2 * y);

}

void VBS::step()
{
    _pistonVolume = (double)_Controller.step((float)_referenceDepth, (float)get_current_depth());
}

// Update VBS depth
void VBS::update_depth(double depth)
{
    _currentDepth = (1.0f - _alpha)*_currentDepth + _alpha * depth;
}

// Update VBS current volume
double VBS::get_piston_volume()
{
    return _pistonVolume;
}

double VBS::get_reference_depth()
{
    return _referenceDepth;
}

double VBS::get_current_depth()
{
    return _currentDepth;
}
