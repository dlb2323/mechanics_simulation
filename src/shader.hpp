#ifndef SHADER_H
#define SHADER_H

#define GLFW_INCLUDE_NONE
#include "GLFW/glfw3.h"
#include "glad/glad.h"
#include <fstream>
#include <iostream>

// local function too copy file data into string
static std::string read_file(std::string path) {
  std::string data;
  std::ifstream file;
  // open file in binary mode for windows compatability
  file.open(path, std::ios_base::out | std::ios_base::binary);
  if (file.is_open()) {
    // read in characters
    data.assign((std::istreambuf_iterator<char>(file)),
                (std::istreambuf_iterator<char>()));
  }
  file.close();
  return data;
}

// local function to compile a shader
static const unsigned int load_shader(std::string source_path,
                               const unsigned int shader_object) {
  // read in file to string
  std::string shader_source = read_file(source_path);
  // convert to const char * format to interact with opengl
  const char *p_shader_source = shader_source.c_str();
  // opengl set shader source and compile
  glShaderSource(shader_object, 1, (const GLchar *const *)&p_shader_source, NULL);
  glCompileShader(shader_object);

  /* check for compilation errors  */
  int success;
  char info_log[512];
  glGetShaderiv(shader_object, GL_COMPILE_STATUS, &success);
  if (!success) {
    glGetShaderInfoLog(shader_object, 512, NULL, info_log);
    std::cerr << "[ERROR] SHADER COMPILATION FAILED\n" << info_log;
  }
  return shader_object;
}

class shader {
  // program pointer
  const unsigned int m_program;
  // store shader uniform locations
  unsigned int m_u_mvp_location;
  unsigned int m_u_colour_location;
  unsigned int m_u_time_location;

public:
  // public access shaders
  static shader* main;
  static shader* single_colour;
  shader(std::string vertex_path, std::string fragment_path) : m_program(glCreateProgram()) {

    /* create and link vertex and fragment shaders */
    const unsigned int vertex_shader = load_shader(vertex_path, glCreateShader(GL_VERTEX_SHADER));
    const unsigned int fragment_shader = load_shader(fragment_path, glCreateShader(GL_FRAGMENT_SHADER));

    glAttachShader(m_program, vertex_shader);
    glAttachShader(m_program, fragment_shader);
    glLinkProgram(m_program);
    /* handle linking errors */
    int success;
    char info_log[512];
    glGetProgramiv(m_program, GL_LINK_STATUS, &success);
    if (!success) {
      glGetProgramInfoLog(m_program, 512, NULL, info_log);
      std::cerr << "[ERROR] LINKING SHADER PROGRAM FAILED\n" << info_log;
    }
    /* delete used shader objects */
    glDeleteShader(vertex_shader);
    glDeleteShader(fragment_shader);
    m_u_mvp_location = get_uniform_location("u_mvp");
    m_u_colour_location = get_uniform_location("u_colour");
    m_u_time_location = get_uniform_location("u_time");
  }
  ~shader() {
    // clean up program data
    glDeleteProgram(m_program);
  }

  void bind();
  void unbind();
  unsigned int get_uniform_location(std::string name);
  // uniform getters
  unsigned int mvp_location() const { return m_u_mvp_location; }
  unsigned int colour_location() const { return m_u_colour_location; }
  unsigned int time_location() const { return m_u_time_location; }
};

#endif // !SHADER_H
