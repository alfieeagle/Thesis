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
        PID(float kp, float kd, float ki, float time_const);

        // Compute the error
        float computeError(float ref, float signal);

        // Find integral and derivative error terms
        float integrateError(float error);
        float calculateDerivative(float error);

        // Compute the controller output
        float computeControlSignal();

    private:
        // Gain values
        float _kp;
        float _kd;
        float _ki;

        // Errors
        float _error;
        float _prev_error;
        float _integral_error;
        float _deriative_error;

        // Anti-Integrall Windup
        int _max_integral_error;

        // Timing
        float _time_const;
        float _elapsed_time;
};

