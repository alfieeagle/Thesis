#ifndef UTILS_HPP

#define UTILS_HPP

#include "imgui.h"
#include "implot.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "messages.pb.h"
#include <GLFW/glfw3.h>

#ifdef __cplusplus
extern "C" {
#endif

#include <fcntl.h>
#include <termios.h>
#include <pb_encode.h>
#include <pb_decode.h>
#include <stdio.h>
#include <errno.h>
#include <unistd.h>

#define BUFFER_SIZE 1000
#define PLOT_HISTORY_SIZE 500

typedef uint8_t SerialBuffer[BUFFER_SIZE];
static float depth_history[PLOT_HISTORY_SIZE] = {0};
static float ref_history[PLOT_HISTORY_SIZE] = {0};
static int offset = 0;

typedef struct StatusMessage
{
    float depth; 
    float ref_depth;
    bool status;
    float piston_pos;
    float control_volume;
}StatusMessage;

typedef struct Command
{
    float target_depth;
    bool enable;
}Command;

// UI functions
int init_ImGUI(GLFWwindow** window);
int render_depth_plot();

// Serial functions
void configure_termios(int* serialport);
int encode_data_and_send(int serialPort, Command msg);
StatusMessage decode_data_and_read(int serialPort);
void clean_serial(int serialPort);

#ifdef __cplusplus
}
#endif

#endif

