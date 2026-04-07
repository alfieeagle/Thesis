/** ------------------------ buoyancy_node.cpp ------------------------

Author:			Alfie Eagleton

Date:			23/2/26

Description:
This file contains the implementation of the buoyancy_node
that talks to the gazebo buoyancy simulation.

Dependencies:	buoyancy_node.hpp

**/

#include "buoyancy_node.hpp"

BuoyancyNode::BuoyancyNode()
: Node("buoyancy_node")
{
  // Controller params
  float kp = 1.5f;
  float kd = 0.01f;
  float ki = 6.0f;

  // VBS params
  float dt = 0.1f;
  float length = 0.2f;
  float radius = 0.056f;

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

  _VBS = std::make_unique<VBS>(
    kp, 
    kd, 
    ki, 
    dt, 
    length, 
    radius, 
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

  auto qos = rclcpp::QoS(rclcpp::KeepLast(10)).reliable();

  _pistonVolPub = this->create_publisher<std_msgs::msg::Float64>("/piston_volume", qos);

  _lastControlTime = this->get_clock()->now();
  _lastActuatorTime = this->get_clock()->now();

  // Subscribers to robot sensor topics
  _depthSub = this->create_subscription<geometry_msgs::msg::Pose>(
    "/depth",
    qos,
    std::bind(&BuoyancyNode::depth_callback, this, std::placeholders::_1));

    // Control update timer
    #ifndef CORE_TEENSY
        _controlTimer = this->create_wall_timer(
          std::chrono::milliseconds(100),
          [this]()
          { 
              // auto currentControlTime = this->get_clock()->now();
              
              // // Calculate actual dt in seconds
              // float dt_u = (float)(currentControlTime - _lastControlTime).seconds();
              
              // // Guard against the first step or a zero dt
              // if (_firstControlStep || dt_u <= 0.0) {
              //     dt_u = 0.1f;
              //     _firstControlStep = false;
              // }

              // _lastControlTime = currentControlTime;

              float dt_u = 0.1f;

              _VBS->update_control(dt_u); 

              RCLCPP_INFO(this->get_logger(), 
                  "Control Output: %.3f\nPiston Volume: %.3f\n",  
                  _VBS->get_control_volume()*1000000, _VBS->get_piston_volume()*1000000);
          }
      );
    #endif

    // Actuation update timer
    #ifndef CORE_TEENSY
        _actuatorTimer = this->create_wall_timer(
          std::chrono::milliseconds(10),
          [this]()
          { 
              // auto currentActuatorTime = this->get_clock()->now();
              
              // // Calculate actual dt in seconds
              // float dt_act = (float)(currentActuatorTime - _lastActuatorTime).seconds();
              
              // // Guard against the first step or a zero dt
              // if (_firstActuatorStep || dt_act <= 0.0) {
              //     dt_act = 0.01f;
              //     _firstActuatorStep = false;
              // }

              // _lastActuatorTime = currentActuatorTime;

              float dt_act = 0.01f;

              _VBS->update_piston(dt_act);

              auto msg = std_msgs::msg::Float64();
              double pistonVolume = _VBS->get_piston_volume(); 

              msg.data = pistonVolume;
              // msg.data = 0.0019704 - 0.00012053;
              
              // RCLCPP_INFO(this->get_logger(), 
              //     "dt_act: %.3f | Piston: %.1f | Total: %.1f",  
              //     dt_act, pistonVolume * 1000000, msg.data * 1000000);
              
              // _pistonVolPub->publish(msg);

              _pistonVolPub->publish(msg);
          }
      );
    #endif
}

BuoyancyNode::~BuoyancyNode()
{
  RCLCPP_INFO(this->get_logger(), "Buoyancy Node Shutdown");
}

void BuoyancyNode::depth_callback(const geometry_msgs::msg::Pose::SharedPtr msg)
{ 
  _VBS->update_depth((float)msg->position.z);
  // RCLCPP_INFO(this->get_logger(), 
  //               "Depth: %.1f\n",  
  //               msg->position.z);
}

int main(int argc, char * argv[])
{
  rclcpp::init(argc, argv);
  rclcpp::spin(std::make_shared<BuoyancyNode>());
  rclcpp::shutdown();
  return 0;
}

