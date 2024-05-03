#ifndef ENVIRONMENT_H
#define ENVIRONMENT_H

#include <chrono>
#include <iostream>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <glad/glad.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "shader.hpp"
#include "object.hpp"

// camera class
// generates view matrix for draw phase
class camera {
  // motion modes
  enum MODE { STILL, FOCUS, TRACK };
  glm::vec3 m_position;
  glm::vec3 m_start;
  glm::vec3 m_focus_point;
  glm::vec3 *m_p_target;
  timestamp m_timestamp;
  // time taken to reach destination position
  const double m_total_time = 0.5;
  camera::MODE m_mode;

public:
  // zoom level
  float zoom = 8.0f;
  camera() {
    // focus on the zero vector on initialisation
    glm::vec3 point(0.0f);
    focus(point);
  }
  void focus(const glm::vec3 &point);
  void track(glm::vec3 *const target);

  void update();

  // generate view matrix for draw phase
  glm::mat4 get_view_matrix() const {
    glm::mat4 view(1.0f);
    // m_position holds the position of the camera's focus
    // offset from this location and scale by zoom level
    glm::vec3 position = m_position + glm::vec3(0, 5, -10) * zoom;
    // transform to focus on the target position
    view = glm::lookAt(position, m_position, glm::vec3(0.0f, 1.0f, 0.0f));
    return view;
  }
};

// forward declare tree
template<class T>
class tree_node;

// environment class
// holds object tree and simulation setting
// draws and update objects
class environment {
  // projection matrix for draw phase
  glm::mat4 proj;
  // hold pointer to currently selected object
  tree_node<object*>* selection;
public:
  // define struct to hold simulation data
  typedef struct {
    world* w;
    plane* pl;
    particle* pa;
    timestamp time;
  } simulation;
  // declare simulation struct
  simulation subjects;
  // declare object tree as type object*
  tree_node<object*>* objects;
  // glfw window pointer, constant once defined
  GLFWwindow *const window;
  // declare camera as a static member
  static camera current_camera;

  environment(GLFWwindow *window);
  ~environment(); 

  void update(float delta);
  void draw();
  bool is_simulation_legal();
  void simulation_start();
  glm::vec3 simulation_func();
  tree_node<object*>* create(object* object);

  void select(tree_node<object*>* node) {
    selection = node;
  }
  tree_node<object*>* get_selection() const { return selection; }; 

  void deselect(bool reselect = false) {
    if (selection) {
      tree_node<object*>* parent = selection->get_parent();
      if (parent && reselect) {
        if(parent->get_child_count() > 1)
          select(parent->get_child(parent->get_child(0) == selection? 1 : 0));
        else
         select(parent);
        return;
      }
    }
    selection = NULL;
  }
};

#endif // !ENVIRONMENT_H
