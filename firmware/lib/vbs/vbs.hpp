#ifndef VBS_HPP

#define VBS_HPP

#ifdef CORE_TEENSY
    #include <IntervalTimer.h>   
    #pragma message("Building for Hardware")
#else 
    #pragma message("Building for Simulation")
#endif

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
        VBS();
        ~VBS();

        // Function to update the controller inputs and calculate output
        void step();

        // Setters
        void update_depth(double depth);

        // Getters
        double get_piston_volume();
        double get_reference_depth();
        double get_current_depth();


    private:	
        double _referenceDepth;

        // Controller
        VBSController _Controller;

        // Controller input and output structs
        VBSController::ExtU_VBSController_T _inputs;
        VBSController::ExtY_VBSController_T _outputs;

        #ifdef CORE_TEENSY
            IntervalTimer _controllerTimer; 
        #endif
};

#endif
