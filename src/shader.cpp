#include "shader.hpp"
  /* create shader program */
// const unsigned int mvp_location = glGetUniformLocation(shader_program, "u_mvp");
void shader::bind() {
    glUseProgram(shader_program);
}
void shader::unbind() {
    glUseProgram(GL_NONE);
}
