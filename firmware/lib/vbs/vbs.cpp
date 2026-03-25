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

VBS::VBS()
{
    _Controller.initialize();

    #ifdef CORE_TEENSY
        instance_ptr = this;
        _controllerTimer.begin(vbs_timer_isr, 100000); 
    #endif

    _referenceDepth = -5.0f;

    // Initialise inputs and outputs
    _inputs.moorx = 0.0f;
    _inputs.ReferenceDepthm = _referenceDepth;

    _outputs.mv = 0.0f;

}

VBS::~VBS()
{
    _Controller.terminate();
}

void VBS::step()
{
    #ifdef CORE_TEENSY
        noInterrupts(); 
    #endif

    static bool OverrunFlag{ false };

    // Check for overrun
    if (OverrunFlag) {
        _Controller.getRTM()->setErrorStatus("Overrun");
        return;
    }

    OverrunFlag = true;

    _Controller.setExternalInputs(&_inputs);

    // Step the model
    _Controller.step();

    _outputs = _Controller.getExternalOutputs();

    // Indicate task complete
    OverrunFlag = false;

    #ifdef CORE_TEENSY
        interrupts(); 
    #endif
}

// Update VBS depth
void VBS::update_depth(double depth)
{
    _inputs.moorx = depth;
}

// Update VBS current volume
double VBS::get_piston_volume()
{
    return _outputs.mv;
}

double VBS::get_reference_depth()
{
    return _referenceDepth;
}

double VBS::get_current_depth()
{
    return _inputs.moorx;
}