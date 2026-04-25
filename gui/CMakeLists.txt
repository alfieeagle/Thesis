cmake_minimum_required(VERSION 3.10)
project(VBS_Dashboard)

set(CMAKE_CXX_STANDARD 17)
list(APPEND CMAKE_PREFIX_PATH "/opt/homebrew")

find_package(glfw3 REQUIRED)

# 1. Collect all .cpp files from the nested ImGui directory
# This will find imgui.cpp, imgui_draw.cpp, etc.
file(GLOB IMGUI_CORE_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/libs/imgui/*.cpp")

# 2. Collect your main.cpp from the src folder
file(GLOB USER_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")

# 3. Collect the backend glue code from your separate backends folder
file(GLOB BACKEND_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/backends/*.cpp")

set(ALL_SOURCES ${IMGUI_CORE_SOURCES} ${USER_SOURCES} ${BACKEND_SOURCES})

add_executable(app ${ALL_SOURCES})

# 4. Point the compiler to the folders containing .h files
target_include_directories(app PRIVATE 
    "${CMAKE_CURRENT_SOURCE_DIR}/src"
    "${CMAKE_CURRENT_SOURCE_DIR}/libs/imgui"     # Where imgui.h is
    "${CMAKE_CURRENT_SOURCE_DIR}/backends"       # Where imgui_impl_glfw.h is
)

# 5. Linking (macOS specific)
if(APPLE)
    target_link_libraries(app glfw "-framework Cocoa" "-framework OpenGL" "-framework IOKit")
else()
    target_link_libraries(app glfw GL)
endif()