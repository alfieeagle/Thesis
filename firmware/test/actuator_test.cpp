#include <unity.h>
#include "../lib/vbs/actuator.hpp"

// Actuator params
float e_g = 0.9f;
float e_m = 0.8f;
float GR = 45.0f;
float d_m = 0.0075f;
float s_l = 0.008f;
float mu_s = 0.007f;
float T_hold = 0.4f;
float FS = 1.2f; 
float torqueCurveGrad  = 5995.0f;
float torqueCurveInt = 1978.0f;
float maxMotorSpeedRPM = 600.0f;
float minMotorSpeedRPM = 130.0f;
float pistonArea = 0.0029f;

Actuator actuator(
        e_g,
        e_m,
        GR,
        d_m,
        s_l,
        mu_s,
        T_hold,
        FS,
        torqueCurveGrad,
        torqueCurveInt,
        maxMotorSpeedRPM,
        minMotorSpeedRPM,
        pistonArea);

void setUP(void)
{
    // set up
}

void tearDown(void)
{
    // tear down
}

void test_actuator_member_initialiser_working_correctly()
{
    // Original three
    TEST_ASSERT_EQUAL_FLOAT(FS, actuator.get_factor_of_safety());
    TEST_ASSERT_EQUAL_FLOAT(GR, actuator.get_gear_ratio());
    TEST_ASSERT_EQUAL_FLOAT(e_g, actuator.get_gearbox_efficiency());

    // Remaining variables
    TEST_ASSERT_EQUAL_FLOAT(e_m, actuator.get_motor_efficiency());
    TEST_ASSERT_EQUAL_FLOAT(d_m, actuator.get_screw_pitch_diam());
    TEST_ASSERT_EQUAL_FLOAT(s_l, actuator.get_screw_lead());
    TEST_ASSERT_EQUAL_FLOAT(mu_s, actuator.get_screw_friction());
    TEST_ASSERT_EQUAL_FLOAT(T_hold, actuator.get_holding_torque());
    
    TEST_ASSERT_EQUAL_FLOAT(torqueCurveGrad, actuator.get_torque_curve_grad());
    TEST_ASSERT_EQUAL_FLOAT(torqueCurveInt, actuator.get_torque_curve_int());
    
    TEST_ASSERT_EQUAL_FLOAT(maxMotorSpeedRPM, actuator.get_max_speed_rpm());
    TEST_ASSERT_EQUAL_FLOAT(minMotorSpeedRPM, actuator.get_min_speed_rpm());
    TEST_ASSERT_EQUAL_FLOAT(pistonArea, actuator.get_piston_area());
}

void test_extending_more_expensive_than_retracting()
{

    float gearboxTorqueExtend = actuator.calculate_gearbox_torque(15000, 1);
    float gearboxTorqueRetract = actuator.calculate_gearbox_torque(15000, -1);

    TEST_ASSERT_TRUE(fabs(gearboxTorqueExtend) > fabs(gearboxTorqueRetract));

}

void test_torque_calc_returns_a_number()
{

    float gearboxTorqueExtend = actuator.calculate_gearbox_torque(15000, 1);
    float gearboxTorqueRetract = actuator.calculate_gearbox_torque(15000, -1);

    TEST_ASSERT_FLOAT_IS_NOT_NAN(gearboxTorqueRetract);
    TEST_ASSERT_FLOAT_IS_NOT_NAN(gearboxTorqueRetract);

}

void test_hold_torque_set_correctly()
{

    float gearboxTorqueHold = actuator.calculate_gearbox_torque(15000, 0);

    TEST_ASSERT_EQUAL_FLOAT(T_hold, gearboxTorqueHold);

}


void test_correct_motor_torque_to_speed()
{
    float torques[10] = {0.0f, 1.5f, 2.5f, 3.5f, 4.5f, 5.5f, 6.5f, 7.5f, 8.5f, 100.0f};

    for(int i = 0; i < 10; i++)
    {
        float motorSpeed = actuator.calculate_max_motor_speed(torques[i]);

        TEST_ASSERT_TRUE(motorSpeed >= 13.60 && motorSpeed <= 62.84);
    }
    
}

void test_slew_rate_calc()
{
    double slewRate = actuator.calculate_slew(35.34f);

    TEST_ASSERT_DOUBLE_WITHIN(0.0000001, 0.000002929, slewRate);
    
}



int main(int argc, char **argv)
{
    UNITY_BEGIN();

    RUN_TEST(test_actuator_member_initialiser_working_correctly);
    RUN_TEST(test_torque_calc_returns_a_number);
    RUN_TEST(test_extending_more_expensive_than_retracting);
    RUN_TEST(test_hold_torque_set_correctly);
    RUN_TEST(test_correct_motor_torque_to_speed);
    RUN_TEST(test_slew_rate_calc);
    

    UNITY_END();
}