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
#include "std_msgs/msg/float32.hpp"
#include "geometry_msgs/msg/pose.hpp"
#include "vbs.hpp"

class BuoyancyNode : public rclcpp::Node
{
    public:
        BuoyancyNode();
        ~BuoyancyNode();

    private:
        // Publishers
        rclcpp::Publisher<std_msgs::msg::Float32>::SharedPtr _pistonVolPub;

        // Subscribers
        rclcpp::Subscription<geometry_msgs::msg::Pose>::SharedPtr _depthSub;

        // Callback
        void depth_callback(const geometry_msgs::msg::Pose::SharedPtr msg);

        // Node knows the VBS class
        std::unique_ptr<VBS> _VBS;
};

#endif