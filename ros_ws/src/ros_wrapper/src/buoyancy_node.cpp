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
  int stepsPerRev = 200;

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
    pistonArea,
    stepsPerRev
  );

  auto qos = rclcpp::QoS(rclcpp::KeepLast(10)).reliable();

  _motorCmdPub = this->create_publisher<geometry_msgs::msg::Vector3>("/motor_command", qos);

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
              _VBS->motor_command(_VBS->get_piston_volume());

              auto msg = geometry_msgs::msg::Vector3();
              std::vector<float> motor_command = _VBS->get_motor_command();

              msg.x = motor_command[0]; // freq
              msg.y = motor_command[1]; // dir
              msg.z = motor_command[2]; // enable

              _motorCmdPub->publish(msg);
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

