#ifndef UTILS_HPP

#define UTILS_HPP

#include "imgui.h"
#include "implot.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "messages.pb.h"

#include <GLFW/glfw3.h>
#include <fcntl.h>
#include <termios.h>
#include <pb_encode.h>
#include <pb_decode.h>
#include <stdio.h>
#include <errno.h>
#include <unistd.h>

#define BUFFER_SIZE 1000

typedef char SerialBuffer[BUFFER_SIZE];

void configure_termios(int* fileDescriptor);

int init_ImGUI(GLFWwindow** window);

void read_from_teensy(int fileDescriptor);

void write_to_teensy(int fileDescriptor, void* buffer, size_t count);

#endif

