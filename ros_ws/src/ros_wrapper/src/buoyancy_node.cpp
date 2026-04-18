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
  float kd = 6.0f;
  float ki = 0.01f;

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

  // Reliable QOS
  rclcpp::QoS qos_rel(10);
  qos_rel.keep_last(10);
  qos_rel.durability_volatile();
  qos_rel.reliable();

  // Best effort QOS
  rclcpp::QoS qos_best(10);
  qos_best.keep_last(10);
  qos_best.best_effort();
  qos_best.durability_volatile();

  // Faster best effort QOS
  rclcpp::QoS qos_index = qos_rel; 
  qos_index.deadline(std::chrono::milliseconds(2)); 
  qos_index.lifespan(std::chrono::milliseconds(2));

  _motorCmdPub = this->create_publisher<geometry_msgs::msg::Vector3>("/motor_command", qos_best);

  // Subscribers to robot sensor topics
  _depthSub = this->create_subscription<std_msgs::msg::Float32>(
    "/depth",
    qos_rel,
    std::bind(&BuoyancyNode::depth_callback, this, std::placeholders::_1));

  _indexSub = this->create_subscription<std_msgs::msg::Int8>(
    "/index",
    qos_rel,
    std::bind(&BuoyancyNode::index_callback, this, std::placeholders::_1));

    _lastControlTime = this->get_clock()->now();

    // Control update timer
    #ifndef CORE_TEENSY
      BuoyancyNode::setup_timers();
    #endif
}

void BuoyancyNode::setup_timers()
{
    // Control update timer (10Hz)
    _controlTimer = this->create_timer(
        std::chrono::milliseconds(100),
        std::bind(&BuoyancyNode::control_callback, this)
    );

    // Actuation update timer (100Hz)
    _actuatorTimer = this->create_timer(
        std::chrono::milliseconds(10),
        std::bind(&BuoyancyNode::actuator_callback, this)
    );
}

void BuoyancyNode::control_callback()
{
    auto currentControlTime = this->get_clock()->now();
    float dt_u = (float)(currentControlTime - _lastControlTime).seconds();

    if (dt_u <= 0.0) return;

    _lastControlTime = currentControlTime;
    _VBS->update_control(dt_u); 

    RCLCPP_INFO(this->get_logger(), 
        "Control Output: %.3f\nPiston Volume: %.3f\n",  
        _VBS->get_control_volume()*1000000, _VBS->get_piston_volume()*1000000);
  }

void BuoyancyNode::actuator_callback()
{
    _VBS->update_motor_command();
    
    auto msg = geometry_msgs::msg::Vector3();
    auto cmd = _VBS->get_motor_command();
    msg.x = cmd[0]; msg.y = cmd[1]; msg.z = cmd[2];
    
    _motorCmdPub->publish(msg);
}

BuoyancyNode::~BuoyancyNode()
{
  RCLCPP_INFO(this->get_logger(), "Buoyancy Node Shutdown");
}

void BuoyancyNode::depth_callback(const std_msgs::msg::Float32::SharedPtr msg)
{ 
  _VBS->update_depth(msg->data);
}

void BuoyancyNode::index_callback(const std_msgs::msg::Int8::SharedPtr msg)
{
  _VBS->update_volume();
}

int main(int argc, char * argv[])
{
  rclcpp::init(argc, argv);
  rclcpp::spin(std::make_shared<BuoyancyNode>());
  rclcpp::shutdown();
  return 0;
}

