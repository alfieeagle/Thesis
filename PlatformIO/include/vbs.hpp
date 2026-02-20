#ifndef VBS_HPP

#define VBS_HPP

/** ------------------------ vbs.hpp ------------------------

Author:			Alfie

Date:			19/2/26

Description:
This file contains the interface for the VBS class.

Dependencies:	iostream   

**/
#include <iostream>

class VBS
{
    public:
        VBS();

        ~VBS();



    private:
        float _pistonVolume;
        float _depth;
        float _screwSpeedRot;
        float _screwSpeedLin;



};

#endif
