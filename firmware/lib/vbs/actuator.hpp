#ifndef MOTOR_HPP

#define MOTOR_HPP

#define _USE_MATH_DEFINES

/** ------------------------ motor.hpp ------------------------

Author:			Alfie Eagleton

Date:			29/3/26

Description:
This file contains the interface for the actuator class which handles
the variable slew rate of the VBS. It also handles  the power and
energy calculations. The actuator encompasses both the gearbox, motor 
and screw.

Dependencies:	cmath, algorithm

**/     

#include <cmath>
#include <algorithm>

class Actuator
{
    public:
        Actuator(
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
        
        // Gearbox calcs
        float calculate_gearbox_torque(float force, int dir);
        float calculate_max_gearbox_torque(float force);

        // Motor calcs
        float calculate_motor_torque(float gearboxTorque);
        float calculate_max_motor_speed(float motorTorque);
        float calculate_motor_power(float motorTorque,  float rotVel);

        // Volume change
        double calculate_slew(float maxSpeed);

        // Step
        double step(float depth, int dir);

        // getters
        double get_piston_area();
        float get_motor_efficiency();
        float get_holding_torque();
        float get_torque_curve_grad();
        float get_torque_curve_int();
        float get_max_speed_rpm();
        float get_min_speed_rpm();
        float get_gear_ratio();
        float get_gearbox_efficiency();
        float get_screw_lead();
        float get_screw_pitch_diam();
        float get_screw_friction();
        float get_factor_of_safety();
        

    private:
        // Motor parameters
        float _motorEfficiency;
        float _holdingTorque;
        float _torqueCurveGrad;
        float _torqueCurveIntercept;
        float _maxSpeedRPM;
        float _minSpeedRPM;

        // Gearbox parameters
        float _gearRatio;
        float _gearboxEfficiency;

        // Screw parameters
        float _screwLead;
        float _screwPitchDiam;
        float _screwFriction;

        float _FS; // Factor of safety
        double _pistonArea;
        
};

#endif
