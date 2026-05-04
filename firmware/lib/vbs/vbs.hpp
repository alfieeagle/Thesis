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
#include "vbs_config.h"

#include <cmath>
#include <algorithm>
#include <vector>

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
            float maxMotorSpeed,
            float minMotorSpeed,
            float pistonArea,
            int stepsPerRev
        );     

        // Setters
        void update_depth(float depth);
        void update_motor_command();
        void update_control(float dt);
        void update_volume();
        void enable();
        void disable();
        void set_home(bool flag);
        void update_direction(int dir);
        void set_volume(int vol_mL);
        void set_reference_depth(float ref_depth);

        // Getters
        // Volumes 
        float get_vbs_volume();
        double get_control_volume();
        double get_piston_volume();
        bool get_status();

        // Depth
        float get_reference_depth();
        float get_current_depth();

        bool get_home();
        
        // Command
        std::vector<float> get_motor_command();

    private:	

        // Depths
        float _referenceDepth;
        float _currentDepth;
        float _density;
        float _g;

        // Controller
        PID _controller;
        double _controlVolume;

        // Actuator
        Actuator _actuator;
        std::vector<float> _motorCommand;
        bool _home;

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
