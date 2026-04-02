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
#include "actuator.hpp"

#include <cmath>
#include <algorithm>

class VBS
{
    public:
        // Constructor and destructor
        VBS(
            float kp, 
            float kd, 
            float ki, 
            float dt, 
            float length, 
            float radius,
            float e_g, 
            float e_m, 
            float GR, 
            float d_m, 
            float s_l, 
            float mu_s, 
            float T_hold, 
            float FS, 
            float torqueCurveGrad, 
            float torqueCurveInt,
            float maxMotorSpeed,
            float minMotorSpeed,
            float pistonArea
        );

        // Function to update the controller inputs and calculate output
        void update_control(float dt);

        // Setters
        void update_depth(float depth);
        void update_piston(float dt);

        // Getters
        double get_piston_volume();
        float get_vbs_volume();
        float get_reference_depth();
        float get_current_depth();
        double get_control_volume();

    private:	

        // Depths
        float _referenceDepth;
        float _currentDepth;
        float _density;
        float _g;

        // Piston 
        double _controlVolume;
        double _pistonVolume;
        double _maxPistonVolume;

        // Controller
        PID _controller;

        // Actuator
        Actuator _actuator;

        // Time step
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
