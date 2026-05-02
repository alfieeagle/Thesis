#include "ui_utils.hpp"

int filedesc = -1;
bool is_connected = false;

static ScrollingBuffer depth, ref_depth, control, piston;

int main(int, char**) 
{
    GLFWwindow* window = NULL;
    int init = init_ImGUI(&window);

    bool my_window_active;

    // Check that the serial port is open
    std::string ttyPort = "/dev/tty.usbmodem167597201";

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

        if (!is_connected)
        {
            // Try to open the port. setup_serial uses O_NONBLOCK so it won't hang the UI.
            filedesc = setup_serial(ttyPort);
            
            if (filedesc >= 0)
            {
                // We found it! Start ONE thread.
                std::thread s_thread(read_serial, filedesc, &depth, &ref_depth, &control, &piston);
                s_thread.detach();
                is_connected = true; 
            }
        }

        // --- Application Window ---
        // Create a copy of the data to render
        data_mutex.lock();
        SystemStatus toPlot = latest_telemetry;
        data_mutex.unlock();

        ImGui::Begin("VBS", &my_window_active);
        double currentTime = glfwGetTime();
        bool is_stale = (currentTime - last_packet_time > 1.0);

        // Connection status info
        ImGui::SeparatorText("Connection Info");
        {
            ImGui::PushStyleColor(ImGuiCol_ChildBg, ImGui::GetStyleColorVec4(ImGuiCol_FrameBg));
            
            // Set height to 0.0f and add ImGuiChildFlags_AlwaysAutoResize
            ImVec2 child_size = ImVec2(-FLT_MIN, 0.0f); 
            ImGuiChildFlags child_flags = ImGuiChildFlags_Borders | ImGuiChildFlags_AlwaysAutoResize | ImGuiChildFlags_AutoResizeY;

            if (ImGui::BeginChild("ResizableConnection", child_size, child_flags))
            {
                if (is_connected && !is_stale)
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

        ImGui::Separator();
        ImGui::PushFont(NULL, 15.0f);
        ImGui::Text("Control Panel");
        ImGui::PopFont();
    
        // Interactive Elements
        static float f1 = 0.0f;
        ImGui::PushItemWidth(200.0f);
        ImGui::SliderFloat("Reference Depth", &f1, -10.0f, 0.0f, "Depth (m) = %.2f");
        static int enable = 0;
        ImGui::RadioButton("Enable", &enable, 0); ImGui::SameLine();
        ImGui::RadioButton("Disable", &enable, 1);

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

    return 0;
}