#include "rclcpp/rclcpp.hpp"
// ... your ImGui includes ...

int main(int argc, char** argv) {
    rclcpp::init(argc, argv);
    
    // Your ImGui + ROS initialization logic here
    // e.g., create a window, setup OpenGL, spin a node
    
    rclcpp::shutdown();
    return 0;
}