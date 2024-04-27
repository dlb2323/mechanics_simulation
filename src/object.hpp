#ifndef SPHERE_H
#define SPHERE_H

#include <chrono>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "gui.hpp"
#include "shader.hpp"

class timestamp {
  std::chrono::time_point<std::chrono::system_clock> start;
public:
  timestamp() {}
  void begin() { start = std::chrono::system_clock::now(); }
  double get_elapsed_time() {
    return ((std::chrono::duration<double>)(std::chrono::system_clock::now() - start)).count();
  }
};

inline glm::vec3 lerp(glm::vec3 x, glm::vec3 y, float t) {
  return x * (1.f - t) + y * t;
}

inline double smooth(double x) { 
  return tanh(sqrt(x) * 6 - M_PI) / 2 + 0.503; 
}

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
  shader *get_shader() const { return m_shader; }
  void bind();
  void unbind();
  void write_begin();
  void write_end();
  void draw();
};

class object : public GUIitem {
  glm::vec3 m_start;
  glm::vec3 m_end;
  timestamp m_timestamp;
  const double m_total_time = 0.25;
  enum MODE { STILL, ACTIVE };
  object::MODE m_mode;
protected:
  mesh *m_mesh;
  float m_scale;
public:
  glm::vec3 position;

  object(std::string &name, mesh *mesh, float scale)
      : GUIitem(name), m_mesh(mesh), m_scale(scale) {}

  object(const char * name, mesh *mesh, float scale)
      : GUIitem(name), m_mesh(mesh), m_scale(scale) {}

  void set_shader(shader *shader) const { m_mesh->set_shader(shader); };
  virtual void draw(glm::mat4 &vp_matrix) const = 0; 
  virtual void draw(glm::mat4 &vp_matrix, float scale) const = 0;
  virtual void update(float delta);
  void select() { selected = true; };
  void deselect() { selected = false; };
  void move_to(glm::vec3& location) {
    m_timestamp.begin();
    m_end = location;
    m_mode = MODE::ACTIVE;
  };
};

class world : public object {
public:
  float friction;
  float distance;
  world(mesh *mesh, float scale, shader *select_shader)
      : object("world", mesh, scale) {}

  void update(float delta) override;

  void draw(glm::mat4 &vp_matrix) const override;
  void draw(glm::mat4 &vp_matrix, float scale) const override;
  void show() const override;
};

class point : public object {
public:
  point(std::string& name, mesh *mesh, float scale, shader *select_shader)
      : object(name, mesh, scale) {}
  void draw(glm::mat4 &vp_matrix) const override;
  void draw(glm::mat4 &vp_matrix, float scale) const override;
  void show() const override;
};

class plane : public object {
public:
  plane(std::string& name, mesh *mesh, float scale, shader *select_shader)
      : object(name, mesh, scale) {}
  void draw(glm::mat4 &vp_matrix) const override;
  void draw(glm::mat4 &vp_matrix, float scale) const override;
  void show() const override;
};

class particle : public object {
public:
  static mesh* particle_mesh;
  particle(std::string &name, float scale)
      : object(name, particle_mesh, scale) {}
  static void gen_vertex_data(unsigned int nodes, mesh &mesh);
  float get_radius() const { return m_scale; };

  void update(float delta) override;

  void draw(glm::mat4 &vp_matrix) const override;
  void draw(glm::mat4 &vp_matrix, float scale) const override;
  void show() const override;
};

#endif // !SPHERE_H
