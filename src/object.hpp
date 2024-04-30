#ifndef OBJECT_H 
#define OBJECT_H

#include <chrono>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <random>
#include "gui.hpp"
#include "shader.hpp"

// simple class to hold a point in time for reference
class timestamp {
  std::chrono::time_point<std::chrono::system_clock> start;
public:
  timestamp() {}
  // update time reference point 
  void begin() { start = std::chrono::system_clock::now(); }
  // get time in seconds since last reference point 
  double get_elapsed_time() {
    return ((std::chrono::duration<double>)(std::chrono::system_clock::now() - start)).count();
  }
};

// linear interpolate between two vectors
inline glm::vec3 lerp(glm::vec3 x, glm::vec3 y, float t) {
  return x * (1.f - t) + y * t;
}

// modify lerp value for a more organic motion
inline double smooth(double x) { 
  return tanh(sqrt(x) * 6 - M_PI) / 2 + 0.503; 
}

// store vertices to draw with opengl 
class mesh {
  shader *m_shader;
  unsigned int m_VAO;
  unsigned int m_VBO;
  unsigned int m_EBO;
  unsigned int m_elements;

public:
  mesh(shader *s) : m_shader(s) {
    // opengl initialise buffers
    glGenBuffers(1, &m_VBO);
    glGenBuffers(1, &m_EBO);
    glGenVertexArrays(1, &m_VAO);
  }
  ~mesh() {
    // opengl delete vertex data 
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

// inherit GUI functionality
class object : public GUIitem {
  // move_to data
  glm::vec3 m_start;
  glm::vec3 m_end;
  timestamp m_timestamp;
  const double m_total_time = 0.2;
  enum MODE { STILL, ACTIVE };
  object::MODE m_mode;
  // colour mod
  glm::fvec3 m_colour;

protected:
  mesh *m_mesh;
  float m_scale;

  // pure function to pass custom object transform matrix to the draw call
  virtual glm::mat4 model_matrix() const = 0;
public:
  glm::vec3 position;

  // initialise defaults, random colour
  object(std::string &name, mesh *mesh, float scale)
      : GUIitem(name), m_mesh(mesh), m_scale(scale), m_mode(MODE::STILL), position(0.0f), m_colour((float)std::rand()/RAND_MAX, (float)std::rand()/RAND_MAX, (float)std::rand()/RAND_MAX) {}

  object(const char * name, mesh *mesh, float scale)
      : GUIitem(name), m_mesh(mesh), m_scale(scale), m_mode(MODE::STILL), position(0.0f), m_colour((float)std::rand()/RAND_MAX, (float)std::rand()/RAND_MAX, (float)std::rand()/RAND_MAX){}

  // swap to another shader
  void set_shader(shader *shader) const { m_mesh->set_shader(shader); };
  // draw object normally
  virtual void draw(glm::mat4 &vp_matrix) const; 
  // draw object as a single colour with custom scalar
  virtual void draw(glm::mat4 &vp_matrix, float scale) const;
  // frame logic step
  virtual void update(float delta);
  void move_to(glm::vec3 location) {
    // store time
    m_timestamp.begin();
    // store start and end points for linear interpolation
    m_start = position;
    m_end = location;
    // switch to active state
    m_mode = MODE::ACTIVE;
  };
  // pure function, passes information to the environment used to build a simulation from tree data
  virtual int get_type_code() const = 0;
};

class simulation;
class no_simulation;

// world class, inherits object, stands at the top of the node tree
class world : public object {
  glm::mat4 model_matrix() const override;
  simulation current_simulation;
public:
  // simulation data
  float distance;
  float friction;
  float force;
  float gravity;
  float mass;
  float u_velocity;
  world(mesh *mesh, float scale)
      : object("world", mesh, scale), distance(1.0f),
        friction(0.0f),
        force(1.0f),
        gravity(9.8f),
        mass(1.0f),
        u_velocity(0.0f),
        current_simulation(no_simulation())
  {}

  void child_added(object* child);
  void child_removed();
  // override draw functions to disable drawing
  void draw(glm::mat4 &vp_matrix) const override;
  void draw(glm::mat4 &vp_matrix, float scale) const override;
  void show() const override;
  int get_type_code() const override { return 0; };
  void update(float delta) override;
};

class plane : public object {
  glm::mat4 model_matrix() const override;
public:
  // custom orientation and length
  float rotation;
  float length;
  static mesh* plane_mesh;
  plane(std::string& name, float scale)
      : object(name, plane_mesh, scale), rotation(3*M_PI/8), length(3.0f) {}
  static void gen_vertex_data(mesh &mesh);
  void show() const override;
  int get_type_code() const override { return 1; };
};

class particle : public object {
  glm::mat4 model_matrix() const override;
public:
  static mesh* particle_mesh;
  particle(std::string &name, float scale)
      : object(name, particle_mesh, scale) {}
  static void gen_vertex_data(unsigned int nodes, mesh &mesh);
  // get radius size
  float get_radius() const { return m_scale; };

  void show() const override;
  int get_type_code() const override { return 3; };
};

class point : public object {
  glm::mat4 model_matrix() const override;
public:
  point(std::string& name, float scale)
      : object(name, particle::particle_mesh, scale) {}
  void show() const override;
  int get_type_code() const override { return 2; };
};


#endif // !OBJECT_H
