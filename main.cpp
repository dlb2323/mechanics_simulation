#include <iostream>
#include "GLFW/glfw3.h"

class window {
	GLFWwindow *window_id;

	public:
	window(const char * name, unsigned int size_x, unsigned int size_y) {
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
		window_id = glfwCreateWindow(size_x, size_y, name, NULL, NULL);
	}

	void makeCurrent();
	bool shouldClose();

};

void window::makeCurrent() {
	glfwMakeContextCurrent(window_id);
}

bool window::shouldClose() {
	return (bool)glfwWindowShouldClose(window_id);
}

int main() {
	if (!glfwInit())
		return -1; // exit the system on the event of failure

	window mainWindow("mechanics_simulation", 900, 900);
	mainWindow.makeCurrent();

	/* if (!window) {
		glfwTerminate();
		return -1; // terminate glfw and exit system 
	} else {
		// glfwSetCursorPosCallback(window, mouse_callback);
	} */

	while(!mainWindow.shouldClose()) {
	    glfwPollEvents();
	}

  	glfwTerminate();
  	exit(EXIT_SUCCESS);
}
