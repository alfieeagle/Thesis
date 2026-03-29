cmake_minimum_required(VERSION 3.8)
project(ros_wrapper)
set(OpenGL_GL_PREFERENCE GLVND)

# Default to C++17 for ROS 2 compatibility
if(NOT CMAKE_CXX_STANDARD)
  set(CMAKE_CXX_STANDARD 17)
endif()

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# Find standard ROS dependencies
find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(geometry_msgs REQUIRED)
find_package(std_msgs REQUIRED)

# Find UI dependencies (Required for ImGui)
find_package(OpenGL REQUIRED)
# Find GLFW 
find_package(PkgConfig REQUIRED)
pkg_check_modules(GLFW REQUIRED glfw3)

# --- BUOYANCY NODE ---
add_executable(buoyancy_node 
  src/buoyancy_node.cpp
  src/vbs.cpp
  src/PID.cpp
  src/actuator.cpp
)

ament_target_dependencies(buoyancy_node 
  rclcpp
  std_msgs
  geometry_msgs
)

target_include_directories(buoyancy_node PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  ${CMAKE_CURRENT_SOURCE_DIR}/../../firmware/lib/vbs
  ${CMAKE_CURRENT_SOURCE_DIR}/../../firmware/lib/controller
) 

# --- VBS UI NODE ---
add_executable(vbs_ui 
  src/gui.cpp
  third_party/imgui/imgui.cpp
  third_party/imgui/imgui_draw.cpp
  third_party/imgui/imgui_tables.cpp 
  third_party/imgui/imgui_widgets.cpp
  third_party/imgui/backends/imgui_impl_glfw.cpp
  third_party/imgui/backends/imgui_impl_opengl3.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/third_party
)

# Link ROS 2 dependencies to the UI
ament_target_dependencies(vbs_ui 
  rclcpp 
  std_msgs
)

# Link OpenGL and GLFW to the UI
target_link_libraries(vbs_ui
  ${GLFW_LIBRARIES}
  ${OPENGL_LIBRARIES}
)

target_include_directories(vbs_ui PRIVATE 
  third_party/imgui 
  third_party/imgui/backends
  ${GLFW_INCLUDE_DIRS}
)

# --- INSTALLATION ---
install(TARGETS buoyancy_node vbs_ui
  DESTINATION lib/${PROJECT_NAME})

install(DIRECTORY config
  DESTINATION share/${PROJECT_NAME})

ament_package()