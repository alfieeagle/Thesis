#include "ui_utils.hpp"

float depth_history[PLOT_HISTORY_SIZE] = {0};
float ref_history[PLOT_HISTORY_SIZE] = {0};
int offset = 0;

void configure_termios(int* serialPort) {
    struct termios options;
    if (tcgetattr(*serialPort, &options) != 0) {
        perror("Error from tcgetattr");
        return;
    }

    cfsetispeed(&options, B115200);
    cfsetospeed(&options, B115200);

    // 8N1 (8 bits, no parity, 1 stop bit)
    options.c_cflag &= ~PARENB;
    options.c_cflag &= ~CSTOPB;
    options.c_cflag &= ~CSIZE;
    options.c_cflag |= CS8;

    // No Hardware Flow Control
    options.c_cflag &= ~CRTSCTS;
    options.c_cflag |= (CLOCAL | CREAD);

    // Fully Disable Canonical Input 
    options.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);

    // Disable all special processing on input/output
    options.c_iflag &= ~(IGNBRK | BRKINT | PARMRK | ISTRIP | INLCR | IGNCR | ICRNL | IXON);
    options.c_oflag &= ~OPOST;

    // Set for Pure Non-Blocking
    options.c_cc[VMIN]  = 0;
    options.c_cc[VTIME] = 0;

    if (tcsetattr(*serialPort, TCSANOW, &options) != 0) {
        perror("Error from tcsetattr");
    }
}

int init_ImGUI(GLFWwindow** window) {
    // Setup GLFW
    if (!glfwInit())
    {
        return 1;
    }

    if (!window)
    {
        glfwTerminate();
	    return 1;
    }

    // Apple-specific OpenGL requirements
    const char* glsl_version = "#version 150";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    *window = glfwCreateWindow(1280, 720, "VBS Control Dashboard", nullptr, nullptr);
    if (window == nullptr)
    {
        return 1;
    } 
    glfwMakeContextCurrent(*window);

    // VSync
    glfwSwapInterval(1);


    // Setup ImGui
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImPlot::CreateContext();
    ImGui::StyleColorsDark();

    ImGui_ImplGlfw_InitForOpenGL(*window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);

    if (!ImPlot::CreateContext())
    {
        return 1;
    }

    return 0;
}

int encode_data_and_send(int serialPort, Command msg)
{
    SerialBuffer buffer;

    // Setup the protobuf stream
    _Command message = Command_init_zero;
    pb_ostream_t stream = pb_ostream_from_buffer(buffer, sizeof(buffer));

    // Encode the message and get the length of encoded bytes
    message.enable = msg.enable;
    message.target_depth = msg.target_depth;
    bool status = pb_encode(&stream, Command_fields, &message);
    size_t message_length = stream.bytes_written;
        
    // Check for encoding errors
    if (!status)
    {
        printf("Encoding failed: %s\n", PB_GET_ERROR(&stream));
        return 1;
    }

    // Write the start byte, length and encoded message to the serial port
    uint8_t startByte = 0xAA;
    uint8_t len = (uint8_t)message_length;
    write(serialPort, &startByte, 1);
    write(serialPort, &len, 1);
    int num_bytes = write(serialPort, buffer, message_length);

    // Check for writing errors
    if(num_bytes < 0)
    {
        printf("Error writing to device: %s", strerror(errno));
    }

    return 0;
}

StatusMessage decode_data_and_read(int serialPort)
{
    uint8_t startbyte;
    
    while (read(serialPort, &startbyte, 1) > 0) 
    {
        // Wait for the correct start byte
        if (startbyte == 0xAA) 
        {
            // Read the length
            uint8_t len;
            if (read(serialPort, &len, 1) > 0) 
            {
                // Create the buffer
                SerialBuffer buffer;
            
                // Decode the message
                int n = read(serialPort, buffer, len);
                _SystemStatus message = SystemStatus_init_zero;
                pb_istream_t stream = pb_istream_from_buffer(buffer, len);
                
                if (pb_decode(&stream, SystemStatus_fields, &message))
                {
                    // Populate the received message
                    StatusMessage msg;
                    msg.depth = message.depth;
                    msg.ref_depth = message.ref_depth;
                    msg.status = message.status;
                    msg.piston_pos = message.piston_pos;
                    msg.control_volume = message.control_volume;
                    std::strcpy(msg.message, message.message);
                    return msg;
                } 
                else 
                {
                    printf("Protobuf Decode Error: %s\n", PB_GET_ERROR(&stream));
                }
            }
        }
    }
    return (StatusMessage){0};
}

void clean_serial(int serialPort)
{
    close(serialPort);
}

int render_depth_plot()
{
    // Ensure we fill the available space in the parent window
    if (ImPlot::BeginPlot("Reference Tracking Performance", ImVec2(500, 500)))
    {
        ImPlot::SetupAxis(ImAxis_X1, "Samples");
        ImPlot::SetupAxis(ImAxis_Y1, "Depth (m)");
        
        // Lock Y-axis to 0-2m for the sine wave
        ImPlot::SetupAxisLimits(ImAxis_Y1, 0.0, 2.0, ImGuiCond_Always);
        ImPlot::SetupAxisLimits(ImAxis_X1, 0, PLOT_HISTORY_SIZE, ImGuiCond_Always);

        ImPlot::PlotLine("Actual", depth_history, PLOT_HISTORY_SIZE);
        ImPlot::PlotLine("Target", ref_history, PLOT_HISTORY_SIZE);        
        
        ImPlot::EndPlot();
    }
    return 1;
}