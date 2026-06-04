# Thesis VBS project
## What is it?
This repository contains the source code for a thesis project aimed at the development of a mid-depth Autonomous Underwater Vehicle (AUV) Variable Buoyancy System (VBS).

## Directory Structure
/firmware uses platformio to deploy the necessary PID controller and VBS logic to a Teensy 4.0 or 4.1

/gui contains the code to build and run a graphical user interface for communicating with the VBS via USB as well as logging for further analysis

/ros_ws contains a ROS2 package which wraps the firmware for use in a software-in-the-loop application. During simulated testing, Simulink and the Mathworks ROS2 toolbox was used as the physics engine

/simulation contains extra tools for finding added mass constants using Pythons Capytaine library. It also contains Python scripts for plotting data from a csv
