#include "window.hpp"

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

GLFWwindow* window::getWindow() {
	return window_id;
}
