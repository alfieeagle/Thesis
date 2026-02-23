#ifndef VBS_HPP

#define VBS_HPP

/** ------------------------ vbs.hpp ------------------------

Author:			Alfie

Date:			19/2/26

Description:
This file contains the interface for the VBS class.

Dependencies:   VBSController.h   

**/
#include "VBSController.h"

class VBS
{
    public:
        // Constructor and destructor
        VBS(float screwLead, float systemVolume, float referenceDepth);
        ~VBS();

        // Function to update the controller inputs and calculate output
        void step();

        // Update member variables
        void update_depth(float depth);
        void update_piston_volume(float pistonVolume);

    private:	
        // System parameters
        const float _screwLead; // mm
        const float _systemVolume; // L
        const float _referenceDepth; // m
        
        // Variable parameters
        float _pistonVolume; // m^3
        float _depth; // m

        // Controller
        VBSController _Controller;
};

#endif
