cmake_minimum_required(VERSION 3.10)
project(VBS_Dashboard)

set(CMAKE_CXX_STANDARD 17)
list(APPEND CMAKE_PREFIX_PATH "/opt/homebrew")

find_package(glfw3 REQUIRED)

file(GLOB IMGUI_CORE_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/libs/imgui/*.cpp")
file(GLOB BACKEND_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/backends/*.cpp")
file(GLOB IMPLOT_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/libs/implot/*.cpp")
file(GLOB UTILS_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/libs/utils/*.cpp")
file(GLOB USER_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")

set(ALL_SOURCES ${IMGUI_CORE_SOURCES} ${USER_SOURCES} ${BACKEND_SOURCES} ${IMPLOT_SOURCES} ${UTILS_SOURCES})

add_executable(app ${ALL_SOURCES})

target_include_directories(app PRIVATE 
    "${CMAKE_CURRENT_SOURCE_DIR}/src"
    "${CMAKE_CURRENT_SOURCE_DIR}/libs/imgui"    
    "${CMAKE_CURRENT_SOURCE_DIR}/libs/implot"
    "${CMAKE_CURRENT_SOURCE_DIR}/libs/utils"
    "${CMAKE_CURRENT_SOURCE_DIR}/messages"
    "${CMAKE_CURRENT_SOURCE_DIR}/backends"  
    "/opt/homebrew/Cellar/nanopb/0.4.9.1_6/include/nanopb"
)

if(APPLE)
    target_link_libraries(app glfw "-framework Cocoa" "-framework OpenGL" "-framework IOKit")
    add_definitions(-DGL_SILENCE_DEPRECATION)
else()
    target_link_libraries(app glfw GL)
endif()