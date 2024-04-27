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
protected:
mesh* m_mesh;
shader* m_select_shader;
const float m_scale;
public:
  glm::vec3 position;

  object(mesh* mesh, float scale, shader* select_shader) : m_mesh(mesh), m_scale(scale), m_select_shader(select_shader) {}

  void set_shader(shader* shader) const { m_mesh->set_shader(shader); };
  virtual void draw(glm::mat4& vp_matrix) const = 0;
  virtual void draw(glm::mat4& vp_matrix, float scale) const = 0;
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
  sphere(std::string& name, mesh* mesh, float scale, shader* select_shader) : object(mesh, scale, select_shader), GUIitem(name) {}
  static void gen_vertex_data(unsigned int nodes, mesh& mesh);
  float get_radius() const { return m_scale; };
  void draw(glm::mat4& viewprojection_matrix) const override;
  void draw(glm::mat4& vp_matrix, float scale) const override;
  void select() override { selected = true; };
  void deselect() override { selected = false; };
};


#endif // !SPHERE_H
