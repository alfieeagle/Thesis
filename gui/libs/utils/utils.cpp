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

int encode_data_and_send(int serialPort, Command msg)
{
    // Initialise a serial buffer
    SerialBuffer buffer;

    // Setup the protobuf stream
    _command message = command_init_zero;
    pb_ostream_t stream = pb_ostream_from_buffer(buffer, msg.len);

    // Encode the message and get the length of encoded bytes
    message.enable = msg.enable;
    message.target_depth = msg.target_depth;
    bool status = pb_encode(&stream, command_fields, &message);
    size_t message_length = stream.bytes_written;
        
    // Check for encoding errors
    if (!status)
    {
        printf("Encoding failed: %s\n", PB_GET_ERROR(&stream));
        return -1;
    }

    // Write the encoded message to the serial port
    int num_bytes = write(serialPort, buffer, message_length);

    // Check for writing errors
    if(num_bytes < 0)
    {
        printf("Error writing to device: %s", strerror(errno));
    }
}

int decode_data_and_read(int serialPort, StatusMessage msg)
{
    // Initialise a serial buffer
    SerialBuffer buffer;
    bool status;

    // Read data from the serial port into the buffer
    int num_bytes = read(serialPort, buffer, msg.len);

    // Check if there was an error reading
    if(num_bytes < 0)
    {
        printf("Error reading: %s", strerror(errno));
    }

    // Setup the protobuf stream and decode
    _system_status message = system_status_init_zero;
    pb_istream_t stream = pb_istream_from_buffer(buffer, msg.len);
    status = pb_decode(&stream, system_status_fields, &message);

    // Check the status of decoding
    if (!status)
    {
        printf("Decoding failed: %s\n", PB_GET_ERROR(&stream));
        return -1;
    }

    // If all went well, print the result
    printf("System status:\r\n \\
        Depth: %.2f\r\n \\
        Reference Depth: %.2f\r\n \\
        Enabled: %s\r\n \\
        Piston Position: %.2f\r\n \\
        Control Volume: %.2f\r\n",
        message.depth,
        message.ref_depth,
        message.status ? "True" : "False",
        message.piston_pos,
        message.control_volume);
}

void clean_serial(int serialPort)
{
    close(serialPort);
}