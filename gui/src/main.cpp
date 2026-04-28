#include "utils.hpp"

static Command cmd;

int main(int, char**) 
{
    GLFWwindow* window = NULL;
    int init = init_ImGUI(&window);

    bool my_window_active;

    // Setup serial coms
    int serialPort = open("/dev/tty.usbmodem157757901", O_RDWR | O_NOCTTY | O_NONBLOCK);

    // Check for errors
    if (serialPort < 0) {
        printf("Error %i from open: %s\n", errno, strerror(errno));
    }
    configure_termios(&serialPort);

    // 3. Main Loop
    while (!glfwWindowShouldClose(window))
    {
        // Read data
        StatusMessage status_msg = decode_data_and_read(serialPort);
        depth_history[offset] = status_msg.depth;
        ref_history[offset] = status_msg.ref_depth;
        offset = (offset + 1) % PLOT_HISTORY_SIZE;

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