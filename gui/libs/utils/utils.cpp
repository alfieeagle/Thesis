#include "utils.hpp"

void configure_termios(int* fileDescriptor) {

    struct termios options;
    tcgetattr(*fileDescriptor, &options);               // Get the current options
    cfsetispeed(&options, B9600);                       // Set input baud rate to 9600
    cfsetospeed(&options, B9600);                       // Set output baud rate to 9600
    options.c_cflag &= ~PARENB;                         // Disable parity checking
    options.c_cflag &= ~CSTOPB;                         // Use 1 stop bit
    options.c_cflag &= ~CSIZE;                          // Mask the character size
    options.c_cflag |= CS8;                             // Select 8-bit data size
    options.c_cflag |= (CLOCAL | CREAD);                // Enable receiver and ignore modem control lines
    options.c_cflag &= ~CRTSCTS;                        // Disable hardware flow control
    options.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG); // Disable canonical, echoing, signal generation (raw input)
    options.c_iflag &= ~(IXON | IXOFF | IXANY);         // Disable software flow control
    options.c_oflag &= ~OPOST;                          // Select raw output
    options.c_cc[VMIN]  = 0;                            // Return immediately if data is available
    options.c_cc[VTIME] = 10;                           // Return 0 if no data arrives within 1 second
    tcsetattr(*fileDescriptor, TCSANOW, &options);      // Set the new options

    return;
}

int init_ImGUI(GLFWwindow** window) {
    // 1. Setup GLFW
    if (!glfwInit())
    {
        return -1;
    }

    if (!window)
    {
        glfwTerminate();
	    return -1;
    }

    // Apple-specific OpenGL requirements
    const char* glsl_version = "#version 150";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    *window = glfwCreateWindow(1280, 720, "VBS Control Dashboard", nullptr, nullptr);
    if (window == nullptr) return 1;
    glfwMakeContextCurrent(*window);
    glfwSwapInterval(1); // VSync


    // 2. Setup ImGui
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImPlot::CreateContext();
    ImGui::StyleColorsDark();

    ImGui_ImplGlfw_InitForOpenGL(*window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);

    if (!ImPlot::CreateContext())
    {
        return -1;
    }

    return 1;
}

void read_from_serial(int fileDescriptor)
{

}

void write_to_serial(int , void* buffer, size_t count)
{
    
}