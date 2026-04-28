#include "utils.hpp"

void configure_termios(int* serialPort) {

    struct termios options;
    tcgetattr(*serialPort, &options);               // Get the current options
    cfsetispeed(&options, B115200);                       // Set input baud rate to 9600
    cfsetospeed(&options, B115200);                       // Set output baud rate to 9600
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
    tcsetattr(*serialPort, TCSANOW, &options);      // Set the new options

    return;
}

int init_ImGUI(GLFWwindow** window) {
    // 1. Setup GLFW
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
        return 1;
    }

    return 0;
}

int encode_data_and_send(int serialPort, Command msg)
{
    SerialBuffer buffer;

    // Setup the protobuf stream
    _command message = command_init_zero;
    pb_ostream_t stream = pb_ostream_from_buffer(buffer, sizeof(buffer));

    // Encode the message and get the length of encoded bytes
    message.enable = msg.enable;
    message.target_depth = msg.target_depth;
    bool status = pb_encode(&stream, command_fields, &message);
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
    uint8_t byte;
    
    // 1. HUNT for the start sequence (0xAA followed by 0xBB)
    // We loop to discard any "junk" bytes until we find our header
    while (read(serialPort, &byte, 1) > 0) 
    {
        if (byte == 0xAA) 
        {
            // Peek at the next byte
            uint8_t nextByte;
            if (read(serialPort, &nextByte, 1) > 0) 
            {
                if (nextByte == 0xBB) 
                {
                    // Success! Found 0xAABB. Now read the length.
                    uint8_t len;
                    if (read(serialPort, &len, 1) > 0) 
                    {
                        SerialBuffer buffer;
                        int total_read = 0;
                        
                        // 2. Safety Read Loop for Payload
                        while (total_read < len) {
                            int n = read(serialPort, buffer + total_read, len - total_read);
                            if (n > 0) total_read += n;
                            else if (n < 0 && errno != EAGAIN) break;
                        }

                        if (total_read == len) {
                            _system_status message = system_status_init_zero;
                            pb_istream_t stream = pb_istream_from_buffer(buffer, len);
                            
                            if (pb_decode(&stream, system_status_fields, &message)) {
                                // SUCCESS - Populate and return
                                StatusMessage msg;
                                msg.depth = message.depth;
                                msg.ref_depth = message.ref_depth;
                                msg.status = message.status;
                                msg.piston_pos = message.piston_pos;
                                msg.control_volume = message.control_volume;
                                return msg;
                            } else {
                                printf("Protobuf Decode Error: %s\n", PB_GET_ERROR(&stream));
                            }
                        }
                    }
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
        ImGui::SetNextWindowPos(ImVec2(0, 70), ImGuiCond_Always);
        ImGui::SetNextWindowSize(ImVec2(
                                        ImGui::GetIO().DisplaySize.x-0,
                                        ImGui::GetIO().DisplaySize.y-70),
                                        ImGuiCond_Always);
        ImGui::Begin("plot_container", NULL,
                    ImGuiWindowFlags_NoMove |
                    ImGuiWindowFlags_NoCollapse |
                    ImGuiWindowFlags_NoResize |
                    ImGuiWindowFlags_NoTitleBar);

        double y_min = -5;
        double y_max = 5;

        if (ImPlot::BeginPlot("Reference Tracking Performance", ImVec2(-1, ImGui::GetContentRegionAvail().y)))
        {
            ImPlot::SetupAxis(ImAxis_X1, "Samples");
            ImPlot::SetupAxis(ImAxis_Y1, "Depth (m)");
            ImPlot::SetupAxisLimits(ImAxis_Y1, y_min, y_max, ImGuiCond_Always);

            // Plot the Target/Reference Depth 
            ImPlot::PlotLine("Target", ref_history, PLOT_HISTORY_SIZE, offset);

            // Plot the Actual Depth
            ImPlot::PlotLine("Actual", depth_history, PLOT_HISTORY_SIZE, offset);
            ImPlot::EndPlot();
        }

        ImGui::End();
        return 1;
    }