#include "ui_utils.hpp"

int main(int, char**) 
{
    GLFWwindow* window = NULL;
    int init = init_ImGUI(&window);

    bool my_window_active;

    // Setup serial coms
    std::string ttyPort = "/dev/tty.usbmodem167597201";
    int filedesc = setup_serial(ttyPort);

    // Setup serial thread to run in the background
    std::thread s_thread(read_serial, filedesc);
    s_thread.detach();

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

        ImGui::Begin("VBS Status", &my_window_active);
        render_depth_plot();

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