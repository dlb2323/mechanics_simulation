#ifndef SPHERE_H
#define SPHERE_H

#include "shader.hpp"

class vertexObject {
  shader *m_shader;
  unsigned int m_VAO;
  unsigned int m_VBO;
  unsigned int m_EBO;
  unsigned int m_elements;

public:
  vertexObject(shader *s) : m_shader(s) {
    glGenBuffers(1, &m_VBO);
    glGenBuffers(1, &m_EBO);
    glGenVertexArrays(1, &m_VAO);
  }
  ~vertexObject() {
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
  vertexObject *m_vObj;
public:
  const float m_radius;
  static unsigned int gen_vertex_data(unsigned int nodes, vertexObject& vObj);
  sphere(vertexObject *vObj, float radius) : m_vObj(vObj), m_radius(radius) {}
};


#endif // !SPHERE_H
