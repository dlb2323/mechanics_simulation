#include <iostream>
#include "GLFW/glfw3.h"

int main() {
	GLFWwindow *window;

	if (!glfwInit())
		return -1; // exit the system on the event of failure

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	window = glfwCreateWindow(900, 900, "mechanics_simulation", NULL, NULL);

	if (!window) {
		glfwTerminate();
		return -1; // terminate glfw and exit system 
	} else {
		// glfwSetCursorPosCallback(window, mouse_callback);
	}

	glfwMakeContextCurrent(window);
	std::string r;
	std::cin >> r; 

  	glfwTerminate();
  	exit(EXIT_SUCCESS);
}
