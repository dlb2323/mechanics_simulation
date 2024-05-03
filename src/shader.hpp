#include "GLFW/glfw3.h"

class shader() {
    const unsigned int shader_program;
public:
    shader() {
        shader_program = glCreateProgram();
    }
}
