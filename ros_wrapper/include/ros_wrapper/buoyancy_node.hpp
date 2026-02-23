#ifndef BUOYANCY_NODE_HPP

#define BUOYANCY_NODE_HPP

/** ------------------------ buoyancy_node.hpp ------------------------

Author:			Alfie Eagleton

Date:			23/2/26

Description:
This file contains the interface for the ROS node that talks to
the gazebo buoyancy simulation

Dependencies:	rclcpp.hpp

**/


#include "rclcpp/rclcpp.hpp"
#include <std_msgs/msg/float32.hpp>

class BuoyancyNode : public rclcpp::Node
{
    public:
        BuoyancyNode();
        ~BuoyancyNode();

    private:
        // Publishers
        rclcpp::Publisher<std_msgs::msg::Float32>::SharedPtr _pistonVolPub;

        // Subscribers
        rclcpp::Subscription<std_msgs::msg::Float32>::SharedPtr _depthSub;

        // Callback
        void depthCallback(const std_msgs::msg::Float32::SharedPtr msg);
};

#endif