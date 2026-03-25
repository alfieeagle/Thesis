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
  _VBS = std::make_unique<VBS>(2.0, 1.0, 1.0, 1.0, 0.1);

  auto qos = rclcpp::QoS(rclcpp::KeepLast(10)).reliable();

  _pistonVolPub = this->create_publisher<std_msgs::msg::Float64>("/piston_volume", qos);

  // Subscribers to robot sensor topics
  _depthSub = this->create_subscription<geometry_msgs::msg::Pose>(
    "/depth",
    qos,
    std::bind(&BuoyancyNode::depth_callback, this, std::placeholders::_1));

    #ifndef CORE_TEENSY
        _simTimer = this->create_wall_timer(
            std::chrono::milliseconds(100),
            [this]()
            { 
              _VBS->step(); 
              auto msg = std_msgs::msg::Float64();
              double volume = _VBS->get_piston_volume(); 

              msg.data = 0.0033929 + volume;
              RCLCPP_INFO(this->get_logger(), 
                "Controller output: %.5f\nFinal volume input: %.5f",  
                volume, msg.data);
              
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
  _VBS->update_depth(msg->position.z);

  // RCLCPP_INFO(this->get_logger(), 
  //               "Depth: %.5f",  
  //               _VBS->get_current_depth());
}

int main(int argc, char * argv[])
{
  rclcpp::init(argc, argv);
  rclcpp::spin(std::make_shared<BuoyancyNode>());
  rclcpp::shutdown();
  return 0;
}

