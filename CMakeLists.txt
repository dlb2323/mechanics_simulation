cmake_minimum_required(VERSION 3.27.1)
set(PROJECT_NAME mechanics_simulation)
# set(CMAKE_C_COMPILER Ninja)
project(${PROJECT_NAME} CXX C)
add_executable(${PROJECT_NAME} main.cpp)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(GLFW_BUILD_WAYLAND ON)

add_subdirectory(glad)
add_subdirectory(glfw)
add_subdirectory(src)


target_include_directories(${PROJECT_NAME} PRIVATE
    "${PROJECT_BINARY_DIR}"
    "${PROJECT_SOURCE_DIR}/glfw/include"
    "${PROJECT_SOURCE_DIR}/glad/include"
    "${PROJECT_SOURCE_DIR}/src"
    )

target_link_libraries(${PROJECT_NAME} PRIVATE 
    glfw
    glad
    )
