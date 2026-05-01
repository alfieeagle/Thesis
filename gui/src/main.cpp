#include "ui_utils.hpp"

int filedesc = -1;
bool is_connected = false;

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
                std::thread s_thread(read_serial, filedesc);
                s_thread.detach();
                is_connected = true; 
            }
        }

        // --- Application Window ---
        // Create a copy of the data to render
        data_mutex.lock();
        SystemStatus toPlot = latest_telemetry;
        data_mutex.unlock();

        ImGui::Begin("VBS Status", &my_window_active);
        render_depth_plot();
        if(filedesc < 0)
        {
            ImGui::Text("Serial port not available. Is the device plugged in and switched on?\n");
        }

        ImGui::Text("Depth: %.2f\n", toPlot.depth);
        ImGui::Text("Ref Depth: %.2f\n", toPlot.ref_depth);
        ImGui::Text("Control Vol: %.2f\n", toPlot.control_volume);
        ImGui::Text("Piston Vol: %.2f\n", toPlot.piston_pos);
        ImGui::Text("Status: %s\n", toPlot.status ? "Enabled" : "Disabled");

        ImGui::End();
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