/** ------------------------ PID.hpp ------------------------

Author:			Alfie Eagleton

Date:			25/3/26

Description:
This file contains the interface for the PID controller used in the variable
buoyancy system.

Dependencies:	    

**/

class PID
{
    public:
        // Constructor
        PID(float kp, float kd, float ki, float timeConst);

        float step(float ref, float signal);

    private:
        // Gain values
        float _kp;
        float _kd;
        float _ki;

        // Errors
        float _error;
        float _prevError;
        float _integralError;
        float _derivativeError;

        // Anti-Integrall Windup
        int _maxIntegralError;

        // Timing
        float _timeConst;
        float _elapsedTime;

        float _ref;

        // Compute the error
        float compute_error(float ref, float signal);

        // Find integral and derivative error terms
        float integrate_error();
        float calculate_error_derivative();

        // Compute the controller output
        float compute_control_signal();
};

