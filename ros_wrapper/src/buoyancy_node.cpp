/** ------------------------ buoyancy_node.cpp ------------------------

Author:			Alfie Eagleton

Date:			23/2/26

Description:
This file contains the implementation of the buoyancy_node
that talks to the gazebo buoyancy simulation.

Dependencies:	

**/

#include "ros_wrapper/buoyancy_node.hpp"

BuoyancyNode::BuoyancyNode()
: Node("buoyancy_node")
{
  auto sensor_qos = rclcpp::SensorDataQoS();

  _pistonVolPub = this->create_publisher<std_msgs::msg::Float32>("/piston_volume", sensor_qos);

  // Subscribers to robot sensor topics
  _depthSub = this->create_subscription<std_msgs::msg::Float32>(
    "/depth",
    rclcpp::SensorDataQoS(),
    std::bind(&BuoyancyNode::depthCallback, this, std::placeholders::_1));
}