/** ------------------------ PID.hpp ------------------------

Author:			Alfie Eagleton

Date:			25/3/26

Description:
This file contains the interface for the PID controller used in the variable
buoyancy system.

Dependencies:   cmath, algorithm   

**/
#include <cmath>
#include <algorithm>

class PID
{
    public:
        // Constructor
        PID(double kp, double kd, double ki);

        double step(float ref, float signal, float dt);

        void set_saturation(double sat);

    private:
        // Gain values
        double _kp;
        double _kd;
        double _ki;

        // Errors
        double _error;
        double _prevError;
        double _integralError;
        double _derivativeError;

        // Anti-Integrall Windup
        double _maxIntegralError;

        // Timing
        float _elapsedTime;

        double _saturation;

        // Compute the error
        void compute_error(float ref, float signal);

        // Find integral and derivative error terms
        void integrate_error();
        void calculate_error_derivative(float dt);

        // Compute the controller output
        double compute_control_signal();
};

