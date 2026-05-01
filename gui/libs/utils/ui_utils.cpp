#include "ui_utils.hpp"

// Create plotting buffers intialised to 0
std::vector<float> depth_history(PLOT_HISTORY_SIZE, 0);
std::vector<float> ref_history(PLOT_HISTORY_SIZE, 0);
int offset = 0;

// Create mutex object for threading
std::mutex data_mutex;

// Global telemetry
SystemStatus latest_telemetry = SystemStatus_init_zero;

int setup_serial(std::string ttyPort)
{
    int filedesc = open(ttyPort.c_str(), O_RDWR | O_NOCTTY | O_NONBLOCK);

    if (filedesc>= 0)
    {
        errno = 0;
        configure_termios(&filedesc);
        tcflush(filedesc, TCIOFLUSH);
    }   

    return filedesc;
}

void read_serial(int fd) {
    is_connected = true;
    while (is_connected) {
        SystemStatus incoming;
        // If read fails or port is closed, decode_data_and_read should return error
        if (decode_data_and_read(fd, &incoming) == 0) {
            std::lock_guard<std::mutex> lock(data_mutex);
            latest_telemetry = incoming;
            depth_history[offset] = incoming.depth;
            ref_history[offset] = incoming.ref_depth;
            offset = (offset + 1) % PLOT_HISTORY_SIZE;
        } else {
            // Check if the port actually closed (errno 5 is EIO - Input/output error)
            if (errno == EIO || errno == EBADF) {
                is_connected = false; 
            }
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
    close(fd); // Clean up the stale file descriptor
}

void configure_termios(int* filedesc) {
    struct termios options;
    if (tcgetattr(*filedesc, &options) != 0) {
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

    if (tcsetattr(*filedesc, TCSANOW, &options) != 0) {
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

int encode_data_and_send(int filedesc, float target_depth, bool enable)
{
    SerialBuffer buffer;

    // Setup the protobuf stream
    Command message = Command_init_zero;
    pb_ostream_t stream = pb_ostream_from_buffer(buffer, sizeof(buffer));

    // Encode the message and get the length of encoded bytes
    message.enable = enable;
    message.target_depth = target_depth;
    bool status = pb_encode(&stream, Command_fields, &message);
    size_t message_length = stream.bytes_written;
        
    // Check for encoding errors
    if (!status)
    {
        printf("Encoding failed from mac: %s\n", PB_GET_ERROR(&stream));
        return 1;
    }

    // Write the start byte, length and encoded message to the serial port
    uint8_t startByte = 0xAA;
    uint8_t len = (uint8_t)message_length;
    write(filedesc, &startByte, 1);
    write(filedesc, &len, 1);
    int num_bytes = write(filedesc, buffer, message_length);

    // Check for writing errors
    if(num_bytes < 0)
    {
        printf("Error writing to device from mac: %s", strerror(errno));
    }

    return 0;
}

int decode_data_and_read(int filedesc, SystemStatus* telemetry)
{
    uint8_t startbyte;
    
    // 1. Search for the Start Byte
    // We use a loop to clear any leading garbage bytes
    while (read(filedesc, &startbyte, 1) > 0) 
    {
        if (startbyte == 0xAA) 
        {
            uint8_t len = 0;
            int attempts = 0;

            // 2. WAIT for the Length Byte
            // USB is fast, but code is faster. We need to wait for the byte to arrive.
            while (read(filedesc, &len, 1) != 1) 
            {
                std::this_thread::sleep_for(std::chrono::microseconds(100));
                if (++attempts > 100) return 1; // Timeout (10ms)
            }

            if (len == 0) 
            {
                printf("Warning: Received packet with 0 length byte\n");
                return 1;
            }

            // 3. WAIT and COLLECT exactly 'len' bytes
            uint8_t buffer[256]; 
            int bytes_received = 0;
            attempts = 0;

            while (bytes_received < len) 
            {
                int n = read(filedesc, buffer + bytes_received, len - bytes_received);
                if (n > 0) 
                {
                    bytes_received += n;
                } 
                else 
                {
                    std::this_thread::sleep_for(std::chrono::microseconds(100));
                    if (++attempts > 1000) // 100ms timeout for the body
                    {
                        printf("Timed out waiting for packet body (Got %d/%d)\n", bytes_received, len);
                        return 1;
                    }
                }
            }

            // 4. Debug Hex Dump (Useful for verifying your 1.0f, 2.0f hardcoded values)
            // printf("Packet Received! Len: %d | Data: ", len);
            // for(int i = 0; i < len; i++) printf("%02X ", buffer[i]);
            // printf("\n");

            // 5. Decode the complete buffer
            SystemStatus message = SystemStatus_init_zero;
            pb_istream_t stream = pb_istream_from_buffer(buffer, len);
            
            if (pb_decode(&stream, SystemStatus_fields, &message)) 
            {
                *telemetry = message;
                return 0; // Success
            } 
            else 
            {
                printf("Protobuf Decode Failed: %s\n", PB_GET_ERROR(&stream));
                // If decoding fails, the stream is likely out of sync. Flush.
                tcflush(filedesc, TCIFLUSH);
                return 1;
            }
        }
    }
    return 1; // No start byte found in this call
}

int render_depth_plot()
{
    // Get maximum y axis value
    float ymax = 0.0f;
    float ymin = *std::min_element(depth_history.begin(), depth_history.end()) - 1.0f;
    printf("ymin: %.2f\n", ymin);

    // Ensure we fill the available space in the parent window
    if (ImPlot::BeginPlot("Reference Tracking Performance", ImVec2(500, 500)))
    {
        ImPlot::SetupAxis(ImAxis_X1, "Samples");
        ImPlot::SetupAxis(ImAxis_Y1, "Depth (m)");
        
        // Set axes limits 
        ImPlot::SetupAxisLimits(ImAxis_Y1, 0.0, ymin, ImGuiCond_Always);
        ImPlot::SetupAxisLimits(ImAxis_X1, 0, PLOT_HISTORY_SIZE, ImGuiCond_Always);

        ImPlot::PlotLine("Actual", depth_history.data(), PLOT_HISTORY_SIZE);
        ImPlot::PlotLine("Target", ref_history.data(), PLOT_HISTORY_SIZE);        
        
        ImPlot::EndPlot();
    }
    return 1;
}