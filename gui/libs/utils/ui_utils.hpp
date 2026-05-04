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
#include <string.h>

#define BUFFER_SIZE 2048
#define PLOT_HISTORY_SIZE 500

typedef uint8_t SerialBuffer[BUFFER_SIZE];

// Create mutex object for threading
extern std::mutex data_mutex;

// Global telemetry
extern SystemStatus latest_telemetry;

extern bool is_connected;
extern int total_packets_received;
extern double last_packet_time;

// utility structure for realtime plot
struct ScrollingBuffer {
    int MaxSize;
    int Offset;
    ImVector<ImVec2> Data;
    ScrollingBuffer(int max_size = PLOT_HISTORY_SIZE) {
        MaxSize = max_size;
        Offset  = 0;
        Data.reserve(MaxSize);
    }
    void AddPoint(float x, float y) {
        if (Data.size() < MaxSize)
            Data.push_back(ImVec2(x,y));
        else {
            Data[Offset] = ImVec2(x,y);
            Offset =  (Offset + 1) % MaxSize;
        }
    }
    void Erase() {
        if (Data.size() > 0) {
            Data.shrink(0);
            Offset  = 0;
        }
    }
};

// UI functions
int init_ImGUI(GLFWwindow** window);
int render_messages(bool has_message, char* msg);
void ClearLog();
void AddLog(const char* fmt, ...);
void real_time_depth_plot(struct ScrollingBuffer* depth, struct ScrollingBuffer* ref_depth);
void real_time_piston_plot(struct ScrollingBuffer* control, struct ScrollingBuffer* piston);

// Serial functions
int setup_serial(std::string ttyPort);
void read_serial(int filedesc, struct ScrollingBuffer* depth, struct ScrollingBuffer* ref_depth, struct ScrollingBuffer* control, struct ScrollingBuffer* piston);
void write_serial(int filedesc, float* target_depth, int* status);
void configure_termios(int* filedesc);
int encode_data_and_send(int filedesc, float target_depth, int enable);
int decode_data_and_read(int filedesc, SystemStatus* telemetry);
void disconnect_serial(int filedesc);

#ifdef __cplusplus
}
#endif

#endif

