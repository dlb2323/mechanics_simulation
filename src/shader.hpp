#ifndef SHADER_H
#define SHADER_H

#define GLFW_INCLUDE_NONE
#include "GLFW/glfw3.h"
#include "glad/glad.h"
#include <fstream>
#include <iostream>

std::string read_file(std::string path) {
  std::string data = "test";
  std::ifstream file;
  file.open(path, std::ios::binary);
  if (file.is_open()) {
    data.assign((std::istreambuf_iterator<char>(file)),
                (std::istreambuf_iterator<char>()));
  }
  file.close();
  return data;
}

const unsigned int load_shader(std::string source_path,
                               const unsigned int shader_object) {
  const char *shader_source = read_file(source_path).c_str();
  glShaderSource(shader_object, 1, (const GLchar *const *)&shader_source, NULL);
  glCompileShader(shader_object);

  /* check for compilation errors  */
  int success;
  char info_log[512];
  glGetShaderiv(shader_object, GL_COMPILE_STATUS, &success);
  if (!success) {
    glGetShaderInfoLog(shader_object, 512, NULL, info_log);
    fprintf(stderr, "[ERROR] SHADER COMPILATION FAILED\n%s", info_log);
  }
  return shader_object;
}

class shader {
  const unsigned int shader_program;

public:
  shader() : shader_program(glCreateProgram()) {
    /* create and link vertex and fragment shaders */

    const unsigned int vertex_shader = load_shader("vertex_shader.glsl", glCreateShader(GL_VERTEX_SHADER));
    const unsigned int fragment_shader = load_shader("fragment_shader.glsl", glCreateShader(GL_FRAGMENT_SHADER));

    glAttachShader(shader_program, vertex_shader);
    glAttachShader(shader_program, fragment_shader);
    glLinkProgram(shader_program);
    /* handle linking errors */
    int success;
    char info_log[512];
    glGetProgramiv(shader_program, GL_LINK_STATUS, &success);
    if (!success) {
      glGetProgramInfoLog(shader_program, 512, NULL, info_log);
      fprintf(stderr, "[ERROR] LINKING SHADER PROGRAM FAILED\n%s", info_log);
    }
    /* delete used shader objects */
    glDeleteShader(vertex_shader);
    glDeleteShader(fragment_shader);
  }
};

#endif // !SHADER_H
