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
        float calculate_slew(float maxSpeed);

        // Step
        float step(float depth, int dir);

        // getters
        float get_piston_area();
        

    private:
        // Motor parameters
        float _motorEfficiency;
        float _holdingTorque;
        float _torqueCurveGrad;
        float _torqueCurveIntercept;
        float _maxSpeedRPM;
        float _minSpeedRPM;

        // Gearbox parameters
        float _gearboxSpeed;
        int _gearRatio;
        float _gearboxEfficiency;

        // Screw parameters
        int _screwLead;
        int _screwPitchDiam;
        float _screwFriction;

        float _FS; // Factor of safety
        double _pistonArea;
        
};


#endif
