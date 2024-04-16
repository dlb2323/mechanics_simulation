#include <iostream>
#define GLFW_INCLUDE_NONE
#include "GLFW/glfw3.h"
#include "glad/glad.h"

#include "src/window.hpp"

int main() {
	if (!glfwInit())
		exit(EXIT_SUCCESS);

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
