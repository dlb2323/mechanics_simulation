#include "shader.hpp"

// define static variables
shader* shader::main;
shader* shader::single_colour;
// bind for drawing 
void shader::bind() {
    glUseProgram(m_program);
}
// unbind from drawing 
void shader::unbind() {
    glUseProgram(GL_NONE);
}
// return uniform location from given name 
unsigned int shader::get_uniform_location(std::string name) {
    return glGetUniformLocation(m_program, name.c_str());
}
