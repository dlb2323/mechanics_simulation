#ifndef WINDOW_H
#define WINDOW_H

#define GLFW_INCLUDE_NONE
#include "GLFW/glfw3.h"
#include "glad/glad.h"

static void framebuffer_size_callback(GLFWwindow *window, int width, int height) {
  glViewport(0, 0, width, height);
}

class window {
	GLFWwindow *window_id;

	public:
	window(const char * name, unsigned int size_x, unsigned int size_y) {
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
		window_id = glfwCreateWindow(size_x, size_y, name, NULL, NULL);
	}

	bool exists();
	void makeCurrent();
	bool shouldClose();
	void swapBuffers();


};

#endif // !WINDOW_H
