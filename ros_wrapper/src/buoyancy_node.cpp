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
  _depthSub = this->create_subscription<geometry_msgs::msg::Pose>(
    "/depth",
    rclcpp::SensorDataQoS(),
    std::bind(&BuoyancyNode::depth_callback, this, std::placeholders::_1));
}

BuoyancyNode::~BuoyancyNode()
{
  RCLCPP_INFO(this->get_logger(), "Buoyancy Node Shutdown");
}

void BuoyancyNode::depth_callback(const geometry_msgs::msg::Pose::SharedPtr msg)
{
  float depth = msg->position.z;
  _VBS->update_depth(depth);
}

int main(int argc, char * argv[])
{
  rclcpp::init(argc, argv);
  rclcpp::spin(std::make_shared<BuoyancyNode>());
  rclcpp::shutdown();
  return 0;
}

