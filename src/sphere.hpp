#ifndef SPHERE_H
#define SPHERE_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "shader.hpp"
#include "gui.hpp"

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
const float m_scale;
public:
  glm::vec3 position;

  object(std::string& name, mesh* mesh, float scale) : m_name(name), m_mesh(mesh), m_scale(scale) {}

  std::string get_name() const { return m_name; }
  void set_shader(shader* shader) const { m_mesh->set_shader(shader); };
  virtual void draw(glm::mat4& modelview_matrix) const = 0;
  virtual void select() = 0;
  virtual void deselect() = 0;
};


class particle : public object, public GUIitem {
public:
  void draw(glm::mat4& viewprojection_matrix) const override;
  void show() const override;
};

class sphere : public object, public GUIitem {
public:
  sphere(std::string& name, mesh* mesh, float scale) : object(name, mesh, scale) {}
  static void gen_vertex_data(unsigned int nodes, mesh& mesh);
  float get_radius() const { return m_scale; };
  void draw(glm::mat4& viewprojection_matrix) const override;
  void select() override { selected = true; };
  void deselect() override { selected = false; };
};


#endif // !SPHERE_H
