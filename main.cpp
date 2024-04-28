#include <iostream>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <glad/glad.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"
#include "imgui.h"

#include "src/environment.hpp"
#include "src/gui.hpp"
#include "tree.hpp"

void APIENTRY glDebugOutput(GLenum source, GLenum type, unsigned int id,
                            GLenum severity, GLsizei length,
                            const char *message, const void *userParam);

void key_callback(GLFWwindow *window, int key, int scancode, int action,
                  int mods) {
  if (key == GLFW_KEY_F11 && action == GLFW_PRESS) {
    int width = 1200, height = 800;
    GLFWmonitor *monitor = glfwGetWindowMonitor(window);
    if (!monitor) {
      const GLFWvidmode *mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
      width = mode->width;
      height = mode->height;
    }
    glfwSetWindowMonitor(window, monitor ? NULL : glfwGetPrimaryMonitor(), 50,
                         50, width, height, 60);
  }
  if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
    glfwSetWindowShouldClose(window, GLFW_TRUE);
}

void error_callback(int error, const char *description) {
  std::cerr << "Error: %s\n" << description;
}

void framebuffer_size_callback(GLFWwindow *window, int width, int height) {
  glViewport(0, 0, width, height);
}

int main() {
  // seed
  std::srand(std::time(0));

  // exit if glfw fails to initialise
  if (!glfwInit())
    exit(EXIT_FAILURE);

  // set up window information and register debug context
  GLFWwindow *window;
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_TRUE);
  glfwWindowHint(GLFW_FLOATING, GLFW_TRUE);

  // find the primary monitor and enter fullscreen
  int width, height;
  GLFWmonitor *monitor = glfwGetPrimaryMonitor();
  if (monitor) {
    const GLFWvidmode *mode = glfwGetVideoMode(monitor);
    width = mode->width;
    height = mode->height;
  } else {
    // terminate if primary monitor isn't found
    glfwTerminate();
    exit(EXIT_FAILURE);
  }
  window = glfwCreateWindow(width, height, "ms",
                            glfwGetPrimaryMonitor(), NULL);
  // terminate if glfw failed to create the window
  if (!window) {
    glfwTerminate();
    exit(EXIT_FAILURE);
  }
  // register glfw callbacks
  glfwSetKeyCallback(window, key_callback);
  glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
  glfwMakeContextCurrent(window);
  glfwSetErrorCallback(error_callback);

  // opengl setup
  gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);

  // enable depth and stencil testing
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_STENCIL_TEST);
  glStencilFunc(GL_ALWAYS, 0, 0xFF);
  glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);

#ifdef DRAW_WIREFRAME
  /* set drawing to wireframe mode */
  glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
#endif

  // register opengl debugging
  int flags;
  glGetIntegerv(GL_CONTEXT_FLAGS, &flags);
  if (flags & GL_CONTEXT_FLAG_DEBUG_BIT) {
    glEnable(GL_DEBUG_OUTPUT);
    glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
    glDebugMessageCallback(glDebugOutput, nullptr);
    glDebugMessageControl(GL_DEBUG_SOURCE_API, GL_DEBUG_TYPE_ERROR,
                          GL_DEBUG_SEVERITY_HIGH, 0, nullptr, GL_TRUE);
  }

  // Setup Dear ImGui context
  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGuiIO &io = ImGui::GetIO();
  io.ConfigFlags |=
      ImGuiConfigFlags_NavEnableKeyboard; // Enable Keyboard Controls

  // Setup Platform/Renderer backends
  ImGui_ImplGlfw_InitForOpenGL(
      window, true); // Second param install_callback=true will install GLFW
                     // callbacks and chain to existing ones.
  ImGui_ImplOpenGL3_Init();
  ImGui::StyleColorsClassic();
  io.Fonts->AddFontFromFileTTF("res/IBMPlexMono-Light.otf", 20.0f);

  // load and store data
  shader main("vertex_shader.glsl", "fragment_shader.glsl");
  shader::main = &main;
  shader single_colour("vertex_shader.glsl", "single_colour_shader.glsl");
  shader::single_colour = &single_colour;

  mesh particle_mesh(&main);
  particle::gen_vertex_data(12, particle_mesh);
  particle::particle_mesh = &particle_mesh;

  mesh plane_mesh(&main);
  plane::gen_vertex_data(plane_mesh);
  plane::plane_mesh = &plane_mesh;

  // create simulation environment
  environment env(window);

  // initialise delta timestamp
  timestamp delta;
  delta.begin();

  while (!glfwWindowShouldClose(window)) {

    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
    // show gui
    GUI::show(env);

    // update
    env.update(delta.get_elapsed_time());
    environment::current_camera.update();

    // timestamp before drawing the frame
    delta.begin();

    // draw
    // set background colour
    glClearColor(0.529, 0.808, 0.98, 0.7);
    // clear opengl buffers
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

    // draw environment
    env.draw();

    // render imgui
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    // update the window with glfw
    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  // clean up data
  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplGlfw_Shutdown();
  ImGui::DestroyContext();

  glfwTerminate();
  exit(EXIT_SUCCESS);
}

// opengl error callback
void APIENTRY glDebugOutput(GLenum source, GLenum type, unsigned int id,
                            GLenum severity, GLsizei length,
                            const char *message, const void *userParam) {
  // ignore non-significant error/warning codes
  if (id == 131169 || id == 131185 || id == 131218 || id == 131204)
    return;

  std::cout << "---------------" << std::endl;
  std::cout << "Debug message (" << id << "): " << message << std::endl;

  switch (source) {
  case GL_DEBUG_SOURCE_API:
    std::cout << "Source: API";
    break;
  case GL_DEBUG_SOURCE_WINDOW_SYSTEM:
    std::cout << "Source: Window System";
    break;
  case GL_DEBUG_SOURCE_SHADER_COMPILER:
    std::cout << "Source: Shader Compiler";
    break;
  case GL_DEBUG_SOURCE_THIRD_PARTY:
    std::cout << "Source: Third Party";
    break;
  case GL_DEBUG_SOURCE_APPLICATION:
    std::cout << "Source: Application";
    break;
  case GL_DEBUG_SOURCE_OTHER:
    std::cout << "Source: Other";
    break;
  }
  std::cout << std::endl;

  switch (type) {
  case GL_DEBUG_TYPE_ERROR:
    std::cout << "Type: Error";
    break;
  case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
    std::cout << "Type: Deprecated Behaviour";
    break;
  case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
    std::cout << "Type: Undefined Behaviour";
    break;
  case GL_DEBUG_TYPE_PORTABILITY:
    std::cout << "Type: Portability";
    break;
  case GL_DEBUG_TYPE_PERFORMANCE:
    std::cout << "Type: Performance";
    break;
  case GL_DEBUG_TYPE_MARKER:
    std::cout << "Type: Marker";
    break;
  case GL_DEBUG_TYPE_PUSH_GROUP:
    std::cout << "Type: Push Group";
    break;
  case GL_DEBUG_TYPE_POP_GROUP:
    std::cout << "Type: Pop Group";
    break;
  case GL_DEBUG_TYPE_OTHER:
    std::cout << "Type: Other";
    break;
  }
  std::cout << std::endl;

  switch (severity) {
  case GL_DEBUG_SEVERITY_HIGH:
    std::cout << "Severity: high";
    break;
  case GL_DEBUG_SEVERITY_MEDIUM:
    std::cout << "Severity: medium";
    break;
  case GL_DEBUG_SEVERITY_LOW:
    std::cout << "Severity: low";
    break;
  case GL_DEBUG_SEVERITY_NOTIFICATION:
    std::cout << "Severity: notification";
    break;
  }
  std::cout << std::endl;
  std::cout << std::endl;
  std::cin.get();
}
