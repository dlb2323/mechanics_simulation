cmake_minimum_required(VERSION 3.27.1)

set(PROJECT_NAME mechanics_simulation)

set(CMAKE_BUILD_TYPE Debug)

project(${PROJECT_NAME} CXX)

add_executable(${PROJECT_NAME} main.cpp)

# source
add_subdirectory(src)
# libraries 
#[[ set(GLFW_BUILD_WAYLAND ON) ]]
add_subdirectory(glfw)


target_link_libraries(${PROJECT_NAME} PRIVATE
    glfw
    iostream
    )

message("${CMAKE_CXX_COMPILER}../../include/c++")

target_include_directories(${PROJECT_NAME} PRIVATE 
    # project
    "${PROJECT_BINARY_DIR}"
    "${PROJECT_SOURCE_DIR}/glfw/include" # glfw
    "${CMAKE_CXX_COMPILER}/../../current/include/c++"
    )

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
