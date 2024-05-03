#include <iostream>
#define GLFW_INCLUDE_NONE
#include "GLFW/glfw3.h"
#include "glad/glad.h"

#include "src/window.hpp"
#include "src/shader.hpp"

// gen sphere
// gl stuff
// abstraction
#include <cmath>
void gen_sphere(unsigned int nodes) {
  double phi, theta;
  float *data = new float[3 * nodes * (nodes + 1) * sizeof(float)];
  float *d_p = data;

  for (int i = 0; i <= nodes; i++) {
    for (int j = 0; j < nodes; j++) {
      phi = 2 * M_PI * i / nodes;
      theta = M_PI * j / nodes;

      *d_p++ = sin(theta) * cos(phi); // x
      *d_p++ = sin(theta) * sin(phi); // y
      *d_p++ = cos(theta);            // z
    }
  }

  int vertices = nodes * (nodes);
  int *tree = new int[vertices * 6 * sizeof(int)];
  for (int i = 0; i < vertices; i += 1) {
    tree[i * 6] = i;
    tree[i * 6 + 1] = i + 1;
    tree[i * 6 + 2] = i + nodes;
    tree[i * 6 + 3] = i + nodes;
    tree[i * 6 + 4] = i + nodes + 1;
    tree[i * 6 + 5] = i + 1;
  }

  delete[] tree;	
  delete[] data;
}



int main() {
	if (!glfwInit())
		exit(EXIT_FAILURE);
	

	window mainWindow("mechanics_simulation", 900, 900);
	mainWindow.makeCurrent();

	if (!mainWindow.exists()) {
		glfwTerminate();
		exit(EXIT_FAILURE);
	}
	gladLoadGLLoader((GLADloadproc) glfwGetProcAddress);

	shader Shader;

	while(!mainWindow.shouldClose()) {
	    if (glfwGetKey(mainWindow.getWindow(), GLFW_KEY_ESCAPE) == GLFW_PRESS) {
	      glfwSetWindowShouldClose(mainWindow.getWindow(), GLFW_TRUE);
	    }

	    glClearColor(0.2, 0.3, 0.3, 1.0);	
	    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	    mainWindow.swapBuffers();
	    glfwPollEvents();
	}

  	glfwTerminate();
  	exit(EXIT_SUCCESS);
}
