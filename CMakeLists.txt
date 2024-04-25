cmake_minimum_required(VERSION 3.27.1)
set(PROJECT_NAME mechanics_simulation)
# set(CMAKE_C_COMPILER Ninja)
project(${PROJECT_NAME} CXX C)
add_executable(${PROJECT_NAME} main.cpp)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(GLFW_BUILD_WAYLAND ON)

add_subdirectory(glad)
add_subdirectory(glfw)
add_subdirectory(glm)
add_subdirectory(src)


target_include_directories(${PROJECT_NAME} PRIVATE
    "${PROJECT_BINARY_DIR}"
    "${PROJECT_SOURCE_DIR}/glfw/include"
    "${PROJECT_SOURCE_DIR}/glad/include"
    "${PROJECT_SOURCE_DIR}/glm"
    "${PROJECT_SOURCE_DIR}/imgui"
    "${PROJECT_SOURCE_DIR}/src"
    )

target_link_libraries(${PROJECT_NAME} PRIVATE 
    glfw
    glad
    glm
    )

# imgui
set(IMGUI_SOURCES
    imgui/backends/imgui_impl_glfw.h
    imgui/backends/imgui_impl_glfw.cpp
    imgui/backends/imgui_impl_opengl3.cpp
    imgui/backends/imgui_impl_opengl3.h
    imgui/backends/imgui_impl_opengl3_loader.h
    imgui/imconfig.h
    imgui/imgui.cpp
    imgui/imgui_demo.cpp
    imgui/imgui_draw.cpp
    imgui/imgui.h
    imgui/imgui_internal.h
    imgui/imgui_tables.cpp
    imgui/imgui_widgets.cpp
    imgui/imstb_rectpack.h
    imgui/imstb_textedit.h
    imgui/imstb_truetype.h
    )
target_sources(${PROJECT_NAME} PRIVATE ${IMGUI_SOURCES})
