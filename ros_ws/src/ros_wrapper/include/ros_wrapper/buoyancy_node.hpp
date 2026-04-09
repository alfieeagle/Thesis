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
#include "geometry_msgs/msg/pose.hpp"
#include "geometry_msgs/msg/vector3.hpp"
#include "vbs.hpp"

class BuoyancyNode : public rclcpp::Node
{
    public:
        BuoyancyNode();
        ~BuoyancyNode();

    private:
        // Publishers
        rclcpp::Publisher<geometry_msgs::msg::Vector3>::SharedPtr _motorCmdPub;

        // Subscribers
        rclcpp::Subscription<geometry_msgs::msg::Pose>::SharedPtr _depthSub;

        // Callback
        void depth_callback(const geometry_msgs::msg::Pose::SharedPtr msg);

        // Node knows the VBS class
        std::unique_ptr<VBS> _VBS;

        #ifndef CORE_TEENSY
            rclcpp::TimerBase::SharedPtr _controlTimer;
            rclcpp::TimerBase::SharedPtr _actuatorTimer;
        #endif

        // Create timer to align with gazebo time
        rclcpp::Time _lastControlTime;
        rclcpp::Time _lastActuatorTime;
        bool _firstControlStep = true;
        bool _firstActuatorStep = true;

};

#endif