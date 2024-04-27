#include "shader.hpp"
  /* create shader program */
// const unsigned int mvp_location = glGetUniformLocation(shader_program, "u_mvp");
shader* shader::main;
shader* shader::single_colour;
void shader::bind() {
    glUseProgram(m_program);
}
void shader::unbind() {
    glUseProgram(GL_NONE);
}
unsigned int shader::get_uniform_location(std::string name) {
    return glGetUniformLocation(m_program, name.c_str());
}
