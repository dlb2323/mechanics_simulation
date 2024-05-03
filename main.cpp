#include <iostream>
#define GLFW_INCLUDE_NONE
#include "GLFW/glfw3.h"
#include "glad/glad.h"

#include "src/shader.hpp"
#include "src/window.hpp"
#include "src/sphere.hpp"

// gen sphere
// gl stuff
// abstraction
#include <cmath>
void error_callback(int error, const char* description)
{
  std::cerr << "Error: %s\n" << description;
}

int main() {
  if (!glfwInit())
    exit(EXIT_FAILURE);

  window mainWindow("mechanics_simulation", 900, 900);
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

    glBindVertexArray(VAO);
    sphere_shader.bind();
    glDrawElements(GL_TRIANGLES, 100, GL_UNSIGNED_INT, 0);

    glClearColor(0.2, 0.3, 0.3, 1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    mainWindow.swapBuffers();
    glfwPollEvents();
  }

  glfwTerminate();
  exit(EXIT_SUCCESS);
}
