#ifndef VBS_HPP

#define VBS_HPP
#define _USE_MATH_DEFINES

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

Dependencies:   PID.hpp, cmath   

**/
#include "PID.hpp"
#include <cmath>

class VBS
{
    public:
        // Constructor and destructor
        VBS(float cutoffFrequency, double kp, double kd, double ki, float dt, float length, float radius);

        // Function to update the controller inputs and calculate output
        void step();

        // Setters
        void update_depth(double depth);

        // Getters
        double get_piston_volume();
        float get_vbs_volume();
        double get_reference_depth();
        double get_current_depth();

    private:	

        // Depths
        double _referenceDepth;
        double _currentDepth;

        // Piston Volume
        double _pistonVolume;
        double _maxPistonVolume;

        // Controller
        PID _Controller;

        // Low pass filter parameters
        float _alpha;
        float _cutoffFrequency;
        float _dt;

        // Dimensions
        float _length;
        float _radius;
        float _volume;

        #ifdef CORE_TEENSY
            IntervalTimer _controllerTimer; 
        #endif
};

#endif
