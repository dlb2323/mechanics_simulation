#include <iostream>
#define GLFW_INCLUDE_NONE
#include "GLFW/glfw3.h"
#include "glad/glad.h"

#include "src/shader.hpp"
#include "src/sphere.hpp"
#include "src/window.hpp"

// gen sphere
// gl stuff
// abstraction
#include <cmath>
void error_callback(int error, const char *description) {
  std::cerr << "Error: %s\n" << description;
}

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
}

int main() {
  if (!glfwInit())
    exit(EXIT_FAILURE);

  window mainWindow(std::string("mechanics_simulation"), 900, 900);
  mainWindow.makeCurrent();
  glfwSetErrorCallback(error_callback);

  if (!mainWindow.exists()) {
    glfwTerminate();
    exit(EXIT_FAILURE);
  }
  gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
  shader sphere_shader(std::string("vertex_shader.glsl"),
                       std::string("fragment_shader.glsl"));
  vertexObject sphere_object(&sphere_shader);

  int flags;
  glGetIntegerv(GL_CONTEXT_FLAGS, &flags);
  if (flags & GL_CONTEXT_FLAG_DEBUG_BIT) {
    glEnable(GL_DEBUG_OUTPUT);
    glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
    glDebugMessageCallback(glDebugOutput, nullptr);
    glDebugMessageControl(GL_DEBUG_SOURCE_API, GL_DEBUG_TYPE_ERROR,
                          GL_DEBUG_SEVERITY_HIGH, 0, nullptr, GL_TRUE);
  }

  unsigned int VAO;
  glGenVertexArrays(1, &VAO);
  glBindVertexArray(VAO);
  glEnableVertexAttribArray(0);
  gen_sphere_vertex_data(12, &sphere_object);
  glBindVertexArray(GL_NONE);

  sphere test(&sphere_object, 5);

  while (!mainWindow.shouldClose()) {
    if (glfwGetKey(mainWindow.getWindow(), GLFW_KEY_ESCAPE) == GLFW_PRESS) {
      glfwSetWindowShouldClose(mainWindow.getWindow(), GLFW_TRUE);
    }
    glClearColor(0.2, 0.3, 0.3, 1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    float floats[] = {
      0.5f, 0.5f, 0.0f,
      0.5f, -0.5f, 0.0f,
      -0.5f, -0.5f, 0.0f
    };

    sphere_shader.bind();
    unsigned int VBO;
    glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
    unsigned int _VAO;
    glGenVertexArrays(1, &_VAO);
    glBindVertexArray(_VAO);
    glEnableVertexAttribArray(0);
    glBufferData(GL_ARRAY_BUFFER, 9*sizeof(float), floats, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
    glDrawArrays(GL_TRIANGLES, 0, 3);

    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, 144 * 6, GL_UNSIGNED_INT, 0);
    mainWindow.swapBuffers();
    glfwPollEvents();

    glGetError();  
  }

  glfwTerminate();
  exit(EXIT_SUCCESS);
}
