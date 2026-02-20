#ifndef VBS_HPP

#define VBS_HPP

/** ------------------------ vbs.hpp ------------------------

Author:			Alfie

Date:			19/2/26

Description:
This file contains the interface for the VBS class.

Dependencies:	iostream   

**/
#include <iostream>

class VBS
{
    public:
        // Constructor
        VBS(float screwLead, float systemVolume);

        // Function to calculate the required stepper position from the desired volume
        int calcStepperPos(float pistonVolume);

        // Function to apply vbs brake
        bool applyBrake(float referenceDepth, float currentDepth);

    private:
        // System parameters
        const float _screwLead;
        const float _systemVolume;
        
        // Variable parameters
        float _pistonVolume;
        float _depth;
        float _screwSpeedRot;
        float _screwSpeedLin;



};

#endif
