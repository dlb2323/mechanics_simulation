#ifndef SPHERE_H
#define SPHERE_H

#include "shader.hpp"

#include <glm/vec3.hpp>

class mesh {
  shader *m_shader;
  unsigned int m_VAO;
  unsigned int m_VBO;
  unsigned int m_EBO;
  unsigned int m_elements;

public:
  mesh(shader *s) : m_shader(s) {
    glGenBuffers(1, &m_VBO);
    glGenBuffers(1, &m_EBO);
    glGenVertexArrays(1, &m_VAO);
  }
  ~mesh() {
    glDeleteBuffers(1, &m_VBO);
    glDeleteBuffers(1, &m_EBO);
    glDeleteVertexArrays(1, &m_VAO);
  }
  void set_elements(unsigned int elements);
  void set_shader(shader *s);
  void bind();
  void unbind();
  void write_begin();
  void write_end();
  void draw();
};

class sphere {
public:
  glm::vec3 position;
  const float m_radius;
  static void gen_vertex_data(unsigned int nodes, mesh& mesh);
  sphere(float radius) : m_radius(radius) {}
};


#endif // !SPHERE_H
