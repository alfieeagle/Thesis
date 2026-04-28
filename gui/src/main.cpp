#include "imgui.h"
#include "implot.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include <GLFW/glfw3.h>
// #include <fcntl.h>
// #include <termios.h>

int main(int, char**) 
{
    // 1. Setup GLFW
    if (!glfwInit()) return 1;

    // Apple-specific OpenGL requirements
    const char* glsl_version = "#version 150";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    GLFWwindow* window = glfwCreateWindow(1280, 720, "VBS Control Dashboard", nullptr, nullptr);
    if (window == nullptr) return 1;
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1); // VSync

    // 2. Setup ImGui
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImPlot::CreateContext();
    ImGui::StyleColorsDark();

    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);

    bool my_window_active;

    // Setup serial coms
    // int fileDescriptor = open("/dev/tty.usbmodem157757901", O_RDWR | O_NOCTTY | O_NONBLOCK);
    // configureTermios(&fileDescriptor);

    // 3. Main Loop
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
        ImGui::Begin("VBS Status", &my_window_active);
        ImGui::Text("System: Online");
        if (ImGui::Button("Disable"))
        {
            // Add your trigger logic here
        }
        if(ImGui::Button("Enable"))
        {
            
        }

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