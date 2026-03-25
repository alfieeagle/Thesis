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
#include "PID.hpp"
#include <cmath>

class VBS
{
    public:
        // Constructor and destructor
        VBS(float cutoffFrequency, float kp, float kd, float ki, float time_const);

        // Function to update the controller inputs and calculate output
        void step();

        // Setters
        void update_depth(double depth);

        // Getters
        double get_piston_volume();
        double get_reference_depth();
        double get_current_depth();

    private:	

        // Depths
        double _referenceDepth;
        double _currentDepth;

        // Piston Volume
        double _pistonVolume;

        // Controller
        PID _Controller;

        // Low pass filter parameters
        float _alpha;
        float _cutoffFrequency;

        #ifdef CORE_TEENSY
            IntervalTimer _controllerTimer; 
        #endif
};

#endif
