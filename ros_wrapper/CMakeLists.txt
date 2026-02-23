cmake_minimum_required(VERSION 3.8)
project(ros_wrapper)

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# find dependencies
find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)

add_executable(buoyancy_node 
  src/buoyancy_node.cpp
  src/vbs.cpp
  src/VBSController.cpp
)

# This is the "Magic" ROS 2 command that handles rclcpp includes and libraries
ament_target_dependencies(buoyancy_node rclcpp)

target_include_directories(buoyancy_node PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include/${PROJECT_NAME}>
  # Add your firmware libs here if they aren't separate packages
  /home/alfieeagle/Documents/uni/Thesis/firmware/lib/vbs
  /home/alfieeagle/Documents/uni/Thesis/firmware/lib/controller
)  # Require C99 and C++17

install(TARGETS buoyancy_node
  DESTINATION lib/${PROJECT_NAME})

if(BUILD_TESTING)
  find_package(ament_lint_auto REQUIRED)
  # the following line skips the linter which checks for copyrights
  # comment the line when a copyright and license is added to all source files
  set(ament_cmake_copyright_FOUND TRUE)
  # the following line skips cpplint (only works in a git repo)
  # comment the line when this package is in a git repo and when
  # a copyright and license is added to all source files
  set(ament_cmake_cpplint_FOUND TRUE)
  ament_lint_auto_find_test_dependencies()
endif()

ament_package()
