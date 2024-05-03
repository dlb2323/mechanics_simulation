#include <iostream>
#define GLFW_INCLUDE_NONE
#include "GLFW/glfw3.h"
#include "glad/glad.h"

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

void window::swapBuffers() {
	glfwSwapBuffers(window_id);
}

bool window::exists() {
	return (bool)window_id;
}

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

	if (!mainWindow.exists()) {
		glfwTerminate();
		return -1; // terminate glfw and exit system 
	} else {
		gladLoadGLLoader((GLADloadproc) glfwGetProcAddress);
	} 

	while(!mainWindow.shouldClose()) {
	    
	    glClearColor(0.2, 0.3, 0.3, 1.0);	
	    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	    mainWindow.swapBuffers();
	    glfwPollEvents();
	}

  	glfwTerminate();
  	exit(EXIT_SUCCESS);
}
