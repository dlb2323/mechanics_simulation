#include <iostream>
#define GLFW_INCLUDE_NONE
#include "GLFW/glfw3.h"
#include "glad/glad.h"

#include "src/shader.hpp"
#include "src/sphere.hpp"
#include "src/window.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// gen sphere
// gl stuff
// abstraction
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
  std::cin.get();
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

  glEnable(GL_DEPTH_TEST);
#ifdef DRAW_WIREFRAME
  /* set drawing to wireframe mode */
  glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
#endif

  /* debugging */
  int flags;
  glGetIntegerv(GL_CONTEXT_FLAGS, &flags);
  if (flags & GL_CONTEXT_FLAG_DEBUG_BIT) {
    glEnable(GL_DEBUG_OUTPUT);
    glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
    glDebugMessageCallback(glDebugOutput, nullptr);
    glDebugMessageControl(GL_DEBUG_SOURCE_API, GL_DEBUG_TYPE_ERROR,
                          GL_DEBUG_SEVERITY_HIGH, 0, nullptr, GL_TRUE);
  }

  // end

  int nodes = 120;
  shader sphere_shader("vertex_shader.glsl", "fragment_shader.glsl");
  const unsigned int mvp_location = sphere_shader.get_uniform_location("u_mvp");
  const unsigned int time_location =
      sphere_shader.get_uniform_location("u_time");
  vertexObject sphere_object(&sphere_shader);
  sphere_object.set_elements(sphere::gen_vertex_data(nodes, sphere_object));
  sphere test(&sphere_object, 5);

  while (!mainWindow.shouldClose()) {
    if (glfwGetKey(mainWindow.getWindow(), GLFW_KEY_ESCAPE) == GLFW_PRESS) {
      glfwSetWindowShouldClose(mainWindow.getWindow(), GLFW_TRUE);
    }
    glClearColor(0.2, 0.3, 0.3, 1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    int width, height;
    glfwGetFramebufferSize(mainWindow.getWindow(), &width, &height);

    // draw
    glm::mat4 proj = glm::perspective((float)(M_PI / 4), (float)width / height,
                                      0.1f, 100.0f);
    glm::mat4 view = glm::mat4(1.0f);
    view = glm::translate(view, glm::vec3(-20.0f, -20.0f, -100.0f));
    sphere_object.bind();
    for (int i = 0; i < 30; i++) {
      for (int j = 0; j < 30; j++) {
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(i*5*2, j*5*2, 0.0f));
        model = glm::rotate(model, (float)glfwGetTime() / 20,
                            glm::vec3(1.0f, 0.0f, 1.0f));
        model = glm::scale(
            model, glm::vec3(test.m_radius, test.m_radius, test.m_radius));
        glm::mat4 mvp = proj * view * model;
        glUniformMatrix4fv(mvp_location, 1, GL_FALSE, glm::value_ptr(mvp));
        glUniform1f(time_location, (float)glfwGetTime());
        sphere_object.draw();
      }
    }
    // glDrawElements(GL_TRIANGLES,
    // (int)((sin(glfwGetTime()/12)+1)/2*nodes*nodes*6), GL_UNSIGNED_INT, 0);
    sphere_object.unbind();

    mainWindow.swapBuffers();
    glfwPollEvents();

    glGetError();
  }

  glfwTerminate();
  exit(EXIT_SUCCESS);
}
