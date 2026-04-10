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
#include "std_msgs/msg/int8.hpp"
#include "geometry_msgs/msg/vector3.hpp"
#include <chrono>
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
        rclcpp::Subscription<std_msgs::msg::Float32>::SharedPtr _depthSub;
        rclcpp::Subscription<std_msgs::msg::Int8>::SharedPtr _indexSub;

        // Callbacks
        void depth_callback(const std_msgs::msg::Float32::SharedPtr msg);
        void index_callback(const std_msgs::msg::Int8::SharedPtr msg);
        void control_callback();
        void actuator_callback();

        // Node knows the VBS class
        std::unique_ptr<VBS> _VBS;

        #ifndef CORE_TEENSY
            void setup_timers();
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