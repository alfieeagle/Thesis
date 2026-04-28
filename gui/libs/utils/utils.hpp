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

typedef uint8_t SerialBuffer[BUFFER_SIZE];

typedef struct StatusMessage
{
    float depth; 
    float ref_depth;
    bool status;
    float piston_pos;
    float control_volume;

    // Number of bytes in a status message
    int len = 17;
}StatusMessage;

typedef struct Command
{
    float target_depth;
    bool enable;

    // Number of bytes in a command msg
    int len = 5;
}Command;

void configure_termios(int* serialport);

int init_ImGUI(GLFWwindow** window);

int encode_data_and_send(Command msg);
int decode_data_and_read(StatusMessage msg);

void clean_serial(int serialPort);

#endif

