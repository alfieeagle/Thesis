#include "ui_utils.hpp"

int total_packets_received = 0;
double last_packet_time = 0;

// Create mutex object for threading
std::mutex data_mutex;

// Global telemetry
SystemStatus latest_telemetry = SystemStatus_init_zero;

ImGuiTextFilter       Filter;
bool                  AutoScroll = true;
bool                  ScrollToBottom = false;
ImVector<char*>       Items;

int setup_serial(std::string ttyPort)
{
    int filedesc = open(ttyPort.c_str(), O_RDWR | O_NOCTTY | O_NONBLOCK);

    if (filedesc>= 0)
    {
        errno = 0;
        configure_termios(&filedesc);
        tcflush(filedesc, TCIOFLUSH);
        AddLog("[INFO] Successfully opened serial port: %s", ttyPort.c_str());
    } 
    else
    {
        AddLog("[ERROR] Failed to open serial port: %s", strerror(errno));
    }  

    return filedesc;
}

void read_serial(int* filedesc, struct ScrollingBuffer* depth, struct ScrollingBuffer* ref_depth, struct ScrollingBuffer* control, struct ScrollingBuffer* piston) {
    while (*filedesc >= 0)
    {
        SystemStatus incoming;
        // Check if the serial port is open
        int result = decode_data_and_read(*filedesc, &incoming);
        if (result == 0)
        {
            // Lock the data so only this thread can access
            data_mutex.lock();

            // Update the global telemetry
            latest_telemetry = incoming;

            // Log any message from the firmware
            if (incoming.has_message && incoming.message[0] != '\0')
            {
                AddLog("%s", incoming.message);
            }

            // Update connection details
            total_packets_received++;
            last_packet_time = glfwGetTime();

            depth->AddPoint(glfwGetTime(), incoming.depth);
            ref_depth->AddPoint(glfwGetTime(), incoming.ref_depth);
            control->AddPoint(glfwGetTime(), incoming.control_volume);
            piston->AddPoint(glfwGetTime(), incoming.piston_pos);

            // Unlock for other threads
            data_mutex.unlock();
        } 
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
}

void write_serial(int* filedesc, float* target_depth, int* status)
{
    // Local storage for the last sent state
    float last_sent_depth = -999.0f; // Initialize with impossible values
    int last_sent_status = -1;

    while(*filedesc >= 0)
    {
        data_mutex.lock();
        float current_depth = *target_depth;
        int current_status = *status;
        data_mutex.unlock();

        // Check if anything has changed
        bool depth_changed = std::abs(current_depth - last_sent_depth) > 0.001f;
        bool status_changed = (current_status != last_sent_status);

        if (depth_changed || status_changed)
        {
            // Only lock and send if there is new info
            data_mutex.lock();
            encode_data_and_send(*filedesc, current_depth, current_status);
            data_mutex.unlock();

            // Update the last known state
            last_sent_depth = current_depth;
            last_sent_status = current_status;
        }

        // Still include a small sleep to prevent CPU pegging
        std::this_thread::sleep_for(std::chrono::milliseconds(20));
    }
}

void configure_termios(int* filedesc) {
    struct termios options;
    if (tcgetattr(*filedesc, &options) != 0) {
        perror("Error from tcgetattr");
        return;
    }

    cfsetispeed(&options, B230400);
    cfsetospeed(&options, B230400);

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

    // Disable all depth_special processing on input/output
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

    // Apple-depth_specific OpenGL requirements
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

int encode_data_and_send(int filedesc, float target_depth, int enable)
{
        SerialBuffer buffer;

        // Setup the protobuf stream
        Command message = Command_init_zero;
        pb_ostream_t stream = pb_ostream_from_buffer(buffer, sizeof(buffer));

        // Encode the message and get the length of encoded bytes
        message.enable = enable;
        message.target_depth = target_depth;
        message.has_enable = true;
        message.has_target_depth = true;
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
            AddLog("[ERROR] Writing to device failed: %s", strerror(errno));
        }
    
    return 0;
}

void disconnect_serial(int filedesc)
{
    if(close(filedesc) == 0)
    {
        AddLog("[INFO] Successfully closed serial port");
    }
    else
    {
        AddLog("[ERROR] Error closing serial port: %s", strerror(errno));
    }
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
                AddLog("[DEBUG] Received packet with 0 length byte\n");
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
                        AddLog("[DEBUG] Timed out waiting for packet body (Got %d/%d)\n", bytes_received, len);
                        return 1;
                    }
                }
            }

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
                AddLog("[ERROR] Protobuf Decode Failed: %s\n", PB_GET_ERROR(&stream));
                // If decoding fails, the stream is likely out of sync. Flush.
                tcflush(filedesc, TCIFLUSH);
                return 1;
            }
        }
    }
    return 1;
}

void ClearLog()
{
        for (int i = 0; i < Items.Size; i++)
            ImGui::MemFree(Items[i]);
        Items.clear();
}

void AddLog(const char* fmt, ...)
{
        // FIXME-OPT
        char buf[1024];
        va_list args;
        va_start(args, fmt);
        vsnprintf(buf, IM_COUNTOF(buf), fmt, args);
        buf[IM_COUNTOF(buf)-1] = 0;
        va_end(args);
        Items.push_back(strdup(buf));
}

int render_messages(bool has_message, char* msg)
{
    // Options menu
        if (ImGui::BeginPopup("Options"))
        {
            ImGui::Checkbox("Auto-scroll", &AutoScroll);
            ImGui::EndPopup();
        }
        

    // Options, Filter
        ImGui::SetNextItemShortcut(ImGuiMod_Ctrl | ImGuiKey_O, ImGuiInputFlags_Tooltip);
        if (ImGui::Button("Options"))
            ImGui::OpenPopup("Options");
        ImGui::SameLine();
        if(ImGui::Button("Clear"))
        {
            ClearLog();
        }
        ImGui::SameLine();
        Filter.Draw("Filter Messages: [DEBUG] [INFO] [ERROR]", 180);
        ImGui::Separator();

        // Reserve enough left-over height for 1 separator + 1 input text
        ImGuiStyle& style = ImGui::GetStyle();
        const float footer_height_to_reserve = style.SeparatorSize + style.ItemSpacing.y + ImGui::GetFrameHeightWithSpacing();
        if (ImGui::BeginChild("ScrollingRegion", ImVec2(0, -footer_height_to_reserve), ImGuiChildFlags_NavFlattened, ImGuiWindowFlags_HorizontalScrollbar))
        {
            if (ImGui::BeginPopupContextWindow())
            {
                if (ImGui::Selectable("Clear")) ClearLog();
                ImGui::EndPopup();
            }
    
            ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(4, 1)); // Tighten spacing
            for (const char* item : Items)
            {
                if (!Filter.PassFilter(item))
                    continue;

                // Normally you would store more information in your item than just a string.
                // (e.g. make Items[] an array of structure, store color/type etc.)
                ImVec4 color;
                bool has_color = false;
                if (strstr(item, "[error]")) { color = ImVec4(1.0f, 0.4f, 0.4f, 1.0f); has_color = true; }
                else if (strncmp(item, "# ", 2) == 0) { color = ImVec4(1.0f, 0.8f, 0.6f, 1.0f); has_color = true; }
                if (has_color)
                    ImGui::PushStyleColor(ImGuiCol_Text, color);
                ImGui::TextUnformatted(item);
                if (has_color)
                    ImGui::PopStyleColor();
            }

            if(has_message)
            {
                AddLog("%s\n", msg);
            }

            // Keep up at the bottom of the scroll region if we were already at the bottom at the beginning of the frame.
            // Using a scrollbar or mouse-wheel will take away from the bottom edge.
            if (ScrollToBottom || (AutoScroll && ImGui::GetScrollY() >= ImGui::GetScrollMaxY()))
                ImGui::SetScrollHereY(1.0f);
            ScrollToBottom = false;

            ImGui::PopStyleVar();
        }
        ImGui::EndChild();
        ImGui::Separator();
        return 0;
}

void real_time_depth_plot(struct ScrollingBuffer* depth, struct ScrollingBuffer* ref_depth) {
    data_mutex.lock();
    
    // Check if we have data to avoid the crash we saw earlier
    if (depth->Data.empty()) {
        data_mutex.unlock();
        return; 
    }

    ImVec2 canvasSize = ImGui::GetContentRegionAvail();

    static ImPlotAxisFlags flags = ImPlotAxisFlags_None; // Show labels for debugging
    float history = 30.0f;
    double now = glfwGetTime();

    float ymax = 1.0f;
    auto ymin_depth = std::min_element(depth->Data.begin(), depth->Data.end(), 
    [](const ImVec2& a, const ImVec2& b) {
        return a.y < b.y;
    });
    auto ymin_ref_depth = std::min_element(ref_depth->Data.begin(), ref_depth->Data.end(), 
    [](const ImVec2& a, const ImVec2& b) {
        return a.y < b.y;
    });
    float ymin = (ymin_depth->y < ymin_ref_depth->y) ? ymin_depth->y - 2.0f: ymin_ref_depth->y - 2.0f;

    ImPlotSpec depth_spec;
    depth_spec.LineColor = ImVec4(0.19,0.76,0.27,0.8f);
    depth_spec.LineWeight = 1.5f;
    depth_spec.Stride = 2 * sizeof(float);
    depth_spec.Offset = depth->Offset;

    ImPlotSpec ref_spec;
        ref_spec.LineColor = ImVec4(0.9,0.63,0.04,0.8f);
        ref_spec.LineWeight = 1.5f;
        ref_spec.Stride = 2 * sizeof(float);
        ref_spec.Offset = ref_depth->Offset;

    // Use a fixed height so it doesn't collapse to 0 pixels
    if (ImPlot::BeginPlot("Depth Tracking", ImVec2((canvasSize.x)/2, 400))) {
        ImPlot::SetupAxes("Time (s)", "Depth (m)", flags, flags);
        ImPlot::SetupAxisLimits(ImAxis_X1, now - history, now, ImGuiCond_Always);
        ImPlot::SetupAxisLimits(ImAxis_Y1, ymin, ymax, ImGuiCond_Always); // Adjusted range

        // Arguments: Label, x-ptr, y-ptr, count, flags, offset, stride
        ImPlot::PlotLine("Depth", 
                         &depth->Data[0].x, 
                         &depth->Data[0].y, 
                         depth->Data.size(), depth_spec);

        if (!ref_depth->Data.empty()) {
            ImPlot::PlotLine("Ref Depth", 
                             &ref_depth->Data[0].x, 
                             &ref_depth->Data[0].y, 
                             ref_depth->Data.size(), ref_spec);
        }
            
        ImPlot::EndPlot();
    }
    data_mutex.unlock();
}

void real_time_piston_plot(struct ScrollingBuffer* control, struct ScrollingBuffer* piston) {
    data_mutex.lock();
    
    // Check if we have data to avoid the crash we saw earlier
    if (control->Data.empty()) {
        data_mutex.unlock();
        return; 
    }

    ImVec2 canvasSize = ImGui::GetContentRegionAvail();

    static ImPlotAxisFlags flags = ImPlotAxisFlags_None; // Show labels for debugging
    float history = 30.0f;
    double now = glfwGetTime();

    float ymax = 130.0f;
    float ymin = -ymax;

    // Customise Plotting for each line
    ImPlotSpec control_spec;
    control_spec.LineColor = ImVec4(0.19,0.71,0.76,0.8f);
    control_spec.LineWeight = 1.5f;
    control_spec.Stride = 2 * sizeof(float);
    control_spec.Offset = control->Offset;


    ImPlotSpec piston_spec;
    piston_spec.LineColor = ImVec4(0.52,0.19,0.76,0.8f);
    piston_spec.LineWeight = 1.5f;
    piston_spec.Stride = 2 * sizeof(float);
    piston_spec.Offset = piston->Offset;

    // Use a fixed height so it doesn't collapse to 0 pixels
    if (ImPlot::BeginPlot("Control Signal and Piston Volume", ImVec2((canvasSize.x), 400))) {
        ImPlot::SetupAxes("Time (s)", "Volume (mL)", flags, flags);
        ImPlot::SetupAxisLimits(ImAxis_X1, now - history, now, ImGuiCond_Always);
        ImPlot::SetupAxisLimits(ImAxis_Y1, ymin, ymax, ImGuiCond_Always); // Adjusted range

        // Arguments: Label, x-ptr, y-ptr, count, flags, offset, stride
        ImPlot::PlotLine("Control Volume", 
                         &control->Data[0].x, 
                         &control->Data[0].y, 
                         control->Data.size(), control_spec);

        if (!piston->Data.empty()) {
            ImPlot::PlotLine("Piston Volume", 
                             &piston->Data[0].x, 
                             &piston->Data[0].y, 
                             piston->Data.size(), piston_spec);
        }
            
        ImPlot::EndPlot();
    }
    data_mutex.unlock();
}