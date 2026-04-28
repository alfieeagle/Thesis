#include "utils.hpp"

static Command cmd;

int main(int, char**) 
{
    GLFWwindow* window = NULL;
    int init = init_ImGUI(&window);

    bool my_window_active;

    // Setup serial coms
    int serialPort = open("/dev/ttys000", O_RDWR | O_NOCTTY | O_NONBLOCK);

    // Check for errors
    if (serialPort < 0) {
        printf("Error %i from open: %s\n", errno, strerror(errno));
    }
    if (serialPort >= 0) {
        errno = 0;
        configure_termios(&serialPort);
        tcflush(serialPort, TCIOFLUSH); // Clear everything!
    }   

    // 3. Main Loop
    while (!glfwWindowShouldClose(window))
    {
        StatusMessage status_msg = {0};

        // ONLY try to read if the port is actually open (>= 0)
        if (serialPort >= 0) {
            status_msg = decode_data_and_read(serialPort);
            
            // If decode_data_and_read hit a hardware error, it might have set errno
            if (errno == ESRCH || errno == EBADF || errno == ENXIO || errno == ENOENT) {
                printf("Connection lost. Closing port...\n");
                close(serialPort);
                serialPort = -1; // Mark it as closed!
            }
        } else {
            // PORT IS CLOSED: Try to reconnect
            // CHANGE THIS PATH to the one currently printed by your python script!
            serialPort = open("/dev/ttys000", O_RDWR | O_NOCTTY | O_NONBLOCK);
            
            if (serialPort >= 0) {
                printf("Reconnected successfully!\n");
                errno = 0; 
                configure_termios(&serialPort);
                tcflush(serialPort, TCIOFLUSH);
            } else {
                // Still failing? Slow down the loop so we don't spam the console
                usleep(500000); 
            }
        }

        if (status_msg.depth != 0.0f || status_msg.piston_pos != 0.0f) {
            depth_history[offset] = status_msg.depth;
            ref_history[offset] = status_msg.ref_depth;
            offset = (offset + 1) % PLOT_HISTORY_SIZE;
        }

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