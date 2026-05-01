#ifndef UI_UTILS_HPP

#define UI_UTILS_HPP

#include "imgui.h"
#include "implot.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "messages.pb.h"

#include <GLFW/glfw3.h>
#include <iostream>
#include <thread>
#include <mutex>
#include <string>
#include <cstring>
#include <algorithm>

#ifdef __cplusplus
extern "C" {
#endif

#include <fcntl.h>
#include <termios.h>
#include <pb_encode.h>
#include <pb_decode.h>
#include <errno.h>
#include <unistd.h>

#define BUFFER_SIZE 2048
#define PLOT_HISTORY_SIZE 500

typedef uint8_t SerialBuffer[BUFFER_SIZE];

// Create plotting buffers intialised to 0
extern std::vector<float> depth_history;
extern std::vector<float> ref_history;
extern int offset;

// Create mutex object for threading
extern std::mutex data_mutex;

// Global telemetry
extern SystemStatus latest_telemetry;

extern bool is_connected;

// UI functions
int init_ImGUI(GLFWwindow** window);
int render_depth_plot();

// Serial functions
int setup_serial(std::string ttyPort);
void read_serial(int filedesc);
void configure_termios(int* filedesc);
int encode_data_and_send(int filedesc, Command msg);
int decode_data_and_read(int filedesc, SystemStatus* telemetry);

#ifdef __cplusplus
}
#endif

#endif

