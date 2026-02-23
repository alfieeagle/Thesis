/** ------------------------ vbs.cpp ------------------------

Author:			Alfie Eagleton

Date:			20/2/26

Description:
This file contains the implementation of the VBS class.

Dependencies:	vbs.hpp

**/

#include "vbs.hpp"

VBS::VBS(float screwLead, float systemVolume, float referenceDepth)
: _screwLead(screwLead),
_systemVolume(systemVolume),
_referenceDepth(referenceDepth)
{
    _Controller.initialize();

   _pistonVolume = 0.0f;
   _depth = 0.0f;
}

VBS::~VBS()
{
    _Controller.terminate();
}

void VBS::step()
{
    // Create local input and output variables
    VBSController::ExtU_VBSController_T inputs;
    VBSController::ExtY_VBSController_T outputs;

    inputs.moorx = _depth;
    inputs.ReferenceDepthm = _referenceDepth;

    static bool OverrunFlag{ false };

    // Check for overrun
    if (OverrunFlag) {
        _Controller.getRTM()->setErrorStatus("Overrun");
        return;
    }

    OverrunFlag = true;

    _Controller.setExternalInputs(&inputs);

    // Step the model
    _Controller.step();

    outputs = _Controller.getExternalOutputs();
    _pistonVolume = outputs.mv;

    // Indicate task complete
    OverrunFlag = false;
}

// Update VBS depth
void  VBS::update_depth(float depth)
{
    _depth = depth;
}

// Update VBS current volume
void VBS::update_piston_volume(float pistonVolume)
{
    _pistonVolume = pistonVolume;
}