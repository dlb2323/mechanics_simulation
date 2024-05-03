#ifndef SPHERE_H
#define SPHERE_H

#include "shader.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

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
  shader* get_shader() const { return m_shader; }
  void bind();
  void unbind();
  void write_begin();
  void write_end();
  void draw();
};

class object  {
std::string m_name;
protected:
mesh* m_mesh;
public:
  glm::vec3 position;

  object(mesh* mesh) : m_mesh(mesh) {}

  std::string get_name() const { return m_name; }
  virtual void draw(glm::mat4& modelview_matrix) const { std::cout << "test\n"; };
};

class sphere : public object {
  const float m_radius;
public:
  static void gen_vertex_data(unsigned int nodes, mesh& mesh);
  sphere(mesh* mesh, float radius) : object(mesh), m_radius(radius) {} 
  float get_radius() const { return m_radius; };
  void draw(glm::mat4& viewprojection_matrix) const override;
};


#endif // !SPHERE_H
