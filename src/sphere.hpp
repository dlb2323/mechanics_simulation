#ifndef SPHERE_H
#define SPHERE_H

#include "shader.hpp"

#include <cmath>

class vertexObject {
  shader *m_shader;
  unsigned int m_VBO;
  unsigned int m_EBO;

public:
  vertexObject(shader *s) : m_shader(s) {
    glGenBuffers(1, &m_VBO);
    glGenBuffers(1, &m_EBO);
  }
  void set_shader(shader *s);
  void bind();
  void unbind();
};

class sphere {
  vertexObject *m_vObj;
  float m_radius;
public:
  sphere(vertexObject *vObj, float radius) : m_vObj(vObj), m_radius(radius) {}
};

void gen_sphere_vertex_data(unsigned int nodes, vertexObject* vObj);

#endif // !SPHERE_H
