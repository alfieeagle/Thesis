#ifndef VBS_CPP

#define VBS_CPP

/** ------------------------ vbs.cpp ------------------------

Author:			Alfie Eagleton

Date:			20/2/26

Description:
This file contains the implementation of the VBS class.

Dependencies:	vbs.hpp

**/

#include "vbs.hpp"

VBS::VBS(float screwLead, float systemVolume)
: _screwLead(screwLead),
_systemVolume(systemVolume)
{
   _pistonVolume = 0.0f;
   _depth = 0.0f;
   _screwSpeedLin = 0.0f;
   _screwSpeedRot = 0.0f;
}

int VBS::calcStepperPos(float pistonVolume)
{
    return 0;
}

bool VBS::applyBrake(float referenceDepth, float currentDepth)
{
    return false;
}

#endif