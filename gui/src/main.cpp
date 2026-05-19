#include "ui_utils.hpp"

int filedesc = -1;

static ScrollingBuffer depth, ref_depth, control, piston;
static float piston_vol = 0.0f;
static int manual = 1;
static float target_depth = -0.4f;
static int status = 0;
const float max_piston_vol = 107.0f;

int main(int, char**) 
{
    // Get current time
    time_t rawtime;
    struct tm * timeinfo;
    char buffer [80];

    time (&rawtime);
    timeinfo = localtime (&rawtime);

    strftime (buffer,80,"telemetry_log_%d-%m-%Y_%H:%M:%S",timeinfo);

    std::string plotName(buffer);
    std::string telDir = "../logs/telemetry/";

    // Create telemetry log file stream
    std::ofstream PlotFile(telDir + plotName + ".csv");

    // Clear the buffer and create debug file stream
    memset(buffer, 0, sizeof(buffer));
    strftime (buffer,80,"debug_log_%d-%m-%Y_%H:%M:%S",timeinfo);
    std::string logName(buffer);
    std::string debugDir = "../logs/debug/";
    std::ofstream LogFile(debugDir + logName + ".csv");


    GLFWwindow* window = NULL;
    int init = init_ImGUI(&window);

    bool my_window_active;

    std::string ttyPort = "/dev/ttyvbs";

    // Try to open the port. setup_serial uses O_NONBLOCK so it won't hang the UI.
    filedesc = setup_serial(ttyPort);
    
    if (filedesc >= 0)
    {
        // Start a thread for reading and writing serial
        std::thread read_thread(read_serial, &filedesc, std::ref(PlotFile), std::ref(LogFile), &depth, &ref_depth, &control, &piston);
        std::thread write_thread(write_serial, &filedesc, &target_depth, &piston_vol, &manual, &status);
        read_thread.detach();
        write_thread.detach();
    }

    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();

        // Start Frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        auto io = ImGui::GetIO();
        ImVec2 sizeVec = {io.DisplaySize.x, io.DisplaySize.y};
        ImGui::SetNextWindowSize(sizeVec);
        ImGui::SetNextWindowPos(ImVec2(0,0));
        ImGui::GetStyle().WindowRounding = 0.0f;

        // --- Application Window ---
        // Create a copy of the data to render
        data_mutex.lock();
        SystemStatus toPlot = latest_telemetry;
        data_mutex.unlock();

        ImGui::Begin("VBS", &my_window_active);
        double currentTime = glfwGetTime();
        bool is_stale = (currentTime - last_packet_time > 1.0);

        ImGui::BeginMenuBar();
            if (ImGui::BeginMenu("Menu"))
            {
                ImGui::MenuItem("Serial Connection", NULL, false, false);
                if (ImGui::MenuItem("Reconnect Serial"))
                {
                    filedesc = setup_serial(ttyPort);
                    if(filedesc >= 0)
                    {
                        std::thread read_thread(read_serial, &filedesc, std::ref(PlotFile), std::ref(LogFile), &depth, &ref_depth, &control, &piston);
                        std::thread write_thread(write_serial, &filedesc, &target_depth, &piston_vol, &manual, &status);
                        read_thread.detach();
                        write_thread.detach();
                    }
                }
                if (ImGui::MenuItem("Disconnect Serial"))
                {
                    disconnect_serial(filedesc);
                }
                ImGui::EndMenu();
            }

        // Connection status info
        ImGui::SeparatorText("Connection Info");
        {
            ImGui::PushStyleColor(ImGuiCol_ChildBg, ImGui::GetStyleColorVec4(ImGuiCol_FrameBg));
            
            // Set height to 0.0f and add ImGuiChildFlags_AlwaysAutoResize
            ImVec2 child_size = ImVec2(-FLT_MIN, 0.0f); 
            ImGuiChildFlags child_flags = ImGuiChildFlags_Borders | ImGuiChildFlags_AlwaysAutoResize | ImGuiChildFlags_AutoResizeY;

            if (ImGui::BeginChild("ResizableConnection", child_size, child_flags))
            {
                if (filedesc >= 0 && !is_stale)
                    ImGui::TextColored(ImVec4(0, 1, 0, 1), "STATUS: ONLINE");
                else
                    ImGui::TextColored(ImVec4(1, 0, 0, 1), "STATUS: OFFLINE");

                ImGui::Text("Packets Received: %d", total_packets_received);
                ImGui::Text("Time since last packet: %.2f s", currentTime - last_packet_time);
                
                if(filedesc < 0)
                    ImGui::Text("Serial port not available. Is the device plugged in and switched on?");
            }
            ImGui::EndChild();
            ImGui::PopStyleColor();
        }

        // Control panel
        ImGui::Separator();
        ImGui::Spacing();
        ImGui::PushFont(NULL, 15.0f);
        ImGui::Text("Control Panel");
        ImGui::PopFont();

        ImGui::BeginGroup();
            ImGui::RadioButton("Auto", &manual, 0);
            ImGui::Text("Reference Depth");
            ImGui::PushItemWidth(200.0f);
            ImGui::SliderFloat("##DepthSlider", &target_depth, -2.0f, 0.0f, "%.2f m");
            ImGui::PopItemWidth();

            ImGui::RadioButton("Enable", &status, 1); 
            ImGui::SameLine();
            ImGui::RadioButton("Disable", &status, 0);
        ImGui::EndGroup();

        ImGui::SameLine(ImGui::GetCursorPosX() + 300.0f); 

        ImGui::BeginGroup();
            static int counter = 0;
            float arrow_text_offset = ImGui::GetStyle().ItemInnerSpacing.x;

            ImGui::PushItemFlag(ImGuiItemFlags_ButtonRepeat, true);
            ImGui::RadioButton("Manual", &manual, 1); 
            if (ImGui::ArrowButton("##up", ImGuiDir_Up))
            {
                if(manual)
                {
                    piston_vol += 10;
                    if (piston_vol > max_piston_vol)
                        piston_vol = max_piston_vol;
                }
                else
                {
                    AddLog("[INFO] Switch to manual mode to use piston controls");  
                }
            }
            ImGui::SameLine(0.0f, arrow_text_offset);
            ImGui::Text("Extend");

            ImGui::Text("Piston Volume: %.1f mL", piston_vol);
            if (ImGui::ArrowButton("##down", ImGuiDir_Down))
            {
                if(manual)
                {
                    piston_vol -= 10.0f;
                    if (piston_vol < -max_piston_vol)
                        piston_vol = -max_piston_vol;
                }
                else
                {
                    AddLog("[INFO] Switch to manual mode to use piston controls");  
                }
            }
            ImGui::SameLine(0.0f, arrow_text_offset);
            ImGui::Text("Retract");
            if(ImGui::Button("Neutral Pos"))
            {
                if(manual)
                {
                    piston_vol = 0.0f;
                }
                else
                {
                    AddLog("[INFO] Switch to manual mode to use piston controls");  
                }
            }
            ImGui::PopItemFlag();  
        ImGui::EndGroup();

        ImVec2 canvasSize = ImGui::GetContentRegionAvail();

        // Plotting
        real_time_depth_plot(&depth, &ref_depth); 
        ImGui::SameLine((canvasSize.x)/2, 10);
        real_time_piston_plot(&control, &piston);

        // Messages
        ImGui::Separator();
        ImGui::PushFont(NULL, 15.0f);
        ImGui::Text("Messages");
        ImGui::PopFont();
        render_messages(toPlot.has_message, toPlot.message);

        ImGui::End();

        // ImPlot::ShowDemoWindow();
        // --------------------------

        // Rendering
        ImGui::Render();
        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f); // Dark background
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
    }

    // 4. Cleanup
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImPlot::DestroyContext();
    ImGui::DestroyContext();
    glfwDestroyWindow(window);
    glfwTerminate();
    PlotFile.close();
    LogFile.close();

    return 0;
}
