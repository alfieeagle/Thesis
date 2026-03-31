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
  float kp = 6.0f;
  float kd = 1.0f;
  float ki = 1.0f;

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

  _lastStepTime = this->get_clock()->now();

  // Subscribers to robot sensor topics
  _depthSub = this->create_subscription<geometry_msgs::msg::Pose>(
    "/depth",
    qos,
    std::bind(&BuoyancyNode::depth_callback, this, std::placeholders::_1));

    // Control update timer
    #ifndef CORE_TEENSY
        _simTimer = this->create_wall_timer(
          std::chrono::milliseconds(100),
          [this]()
          { 
              auto currentTime = this->get_clock()->now();
              
              // Calculate actual dt in seconds
              float dt = (float)(currentTime - _lastStepTime).seconds();
              
              // Guard against the first step or a zero dt
              if (_firstStep || dt <= 0.0) {
                  dt = 0.1f;
                  _firstStep = false;
              }

              _lastStepTime = currentTime;

              _VBS->step(dt); 

              auto msg = std_msgs::msg::Float64();
              double pistonVolume = _VBS->get_piston_volume(); 

              msg.data = _VBS->get_vbs_volume() + pistonVolume;
              
              // RCLCPP_INFO(this->get_logger(), 
              //     "dt: %.3f | Piston: %.1f | Total: %.1f",  
              //     dt, pistonVolume * 1000000, msg.data * 1000000);
              
              _pistonVolPub->publish(msg);
          }
      );
    #endif

    // Actuation update loop
    #ifndef CORE_TEENSY
        _simTimer = this->create_wall_timer(
          std::chrono::milliseconds(10),
          [this]()
          { 
              auto currentTime = this->get_clock()->now();
              
              // Calculate actual dt in seconds
              float dt_act = (float)(currentTime - _lastStepTime).seconds();
              
              // Guard against the first step or a zero dt
              if (_firstStep || dt <= 0.0) {
                  dt_act = 0.01f;
                  _firstStep = false;
              }

              _lastStepTime = currentTime;

              _VBS->step(dt_act); 

              auto msg = std_msgs::msg::Float64();
              double pistonVolume = _VBS->get_piston_volume(); 

              msg.data = _VBS->get_vbs_volume() + pistonVolume;
              
              // RCLCPP_INFO(this->get_logger(), 
              //     "dt: %.3f | Piston: %.1f | Total: %.1f",  
              //     dt, pistonVolume * 1000000, msg.data * 1000000);
              
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

