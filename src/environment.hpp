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


class camera {
  enum MODE { STILL, FOCUS, TRACK };
  glm::vec3 m_position;
  glm::vec3 m_start;
  glm::vec3 m_focus_point;
  glm::vec3 *m_p_target;
  timestamp m_timestamp;
  const double m_total_time = 0.5;
  camera::MODE m_mode;

public:
  float zoom = 8.0f;
  float rotation = M_PI / 4;
  camera() {
    glm::vec3 point(0.0f);
    focus(point);
  }
  void focus(const glm::vec3 &point);
  void track(glm::vec3 *const target);

  void update();

  glm::mat4 get_view_matrix() const {
    glm::mat4 view(1.0f);
    glm::vec3 position = m_position + glm::vec3(0, 5, 10) * zoom;
    view = glm::lookAt(position, m_position, glm::vec3(0.0f, 1.0f, 0.0f));
    return view;
  }
};

template<class T>
class tree_node;

// world rendering
class environment {
  glm::mat4 proj;
  tree_node<object*>* selection;
public:
  tree_node<object*>* objects;
  GLFWwindow *const window;
  static camera current_camera;
  environment(GLFWwindow *window);
  ~environment(); 
  void update(float delta);
  void draw();
  tree_node<object*>* create(object* object);
  // int object_count() const { return objects.size(); }
  // object *object_at(unsigned int idx) {
  //   return (idx < objects.size()) ? objects[idx] : NULL;
  // };
  void select(tree_node<object*>* object) {
    selection = object;
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
