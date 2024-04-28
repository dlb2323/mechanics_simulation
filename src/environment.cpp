#include "environment.hpp"
#include "tree.hpp"
#include "object.hpp"


// camera

void camera::focus(const glm::vec3 &point) {
  m_timestamp.begin();
  m_focus_point = point;
  m_start = m_position;
  m_mode = camera::MODE::FOCUS;
}

void camera::track(glm::vec3 *target) {
  m_timestamp.begin();
  m_p_target = target;
  m_start = m_position;
  m_mode = camera::MODE::TRACK;
}

void camera::update() {
  switch (m_mode) {
  case camera::MODE::FOCUS: {
    if (m_timestamp.get_elapsed_time() > m_total_time) {
      m_position = m_focus_point;
      m_mode = camera::MODE::STILL;
    }
    m_position = lerp(m_start, m_focus_point,
                      smooth(m_timestamp.get_elapsed_time() / m_total_time));
    break;
  }
  case camera::MODE::TRACK: {
    if (!m_p_target) {
      m_mode = camera::MODE::STILL;
    } else {
      if (m_timestamp.get_elapsed_time() > m_total_time) {
        m_position = *m_p_target;
      } else {
        m_position =
            lerp(m_start, *m_p_target,
                 smooth(m_timestamp.get_elapsed_time() / m_total_time));
      }
    }
    break;
  }
  case camera::MODE::STILL:
  default:
    break;
  }
}

// environment
environment::environment(GLFWwindow *window)
    : window(window) {
  objects = tree_node<object*>::create_new(new world(plane::plane_mesh, 1.0f));
  selection = NULL;
}
environment::~environment() {
  auto traverse = objects->get_traversal_state(traversal_state<object*>::MODE::PREORDER);
  while(traverse.next())
    delete traverse.get_item();
  tree_node<object*>::destroy(objects);
};

camera environment::current_camera;

void environment::update(float delta) {
  auto itr = objects->get_traversal_state(traversal_state<object*>::MODE::PREORDER);
  while(itr.next()) {
    itr.get_item()->update(delta);
  }
}

tree_node<object*>* environment::create(object* o) {
  tree_node<object*>* node = tree_node<object*>::create_new(o);
  if (selection)
    selection->insert_node(node);
  else
    objects->insert_node(node);
  // messy
  return node;
}

void environment::draw() {
  int width, height;
  glfwGetFramebufferSize(window, &width, &height);
  GLFWmonitor* monitor = glfwGetWindowMonitor(window);
  if (monitor) {
    const GLFWvidmode* mode = glfwGetVideoMode(monitor);
    width = mode->width;
    height = mode->height;
  }
  proj =
      glm::perspective((float)(M_PI / 4), (float)width / height, 0.1f, 300.0f);
  // shift centre
  float of = (float)(((500.0 + (width-500.0)/2.0)-width/2.0)/(width/2.0));
  glm::mat4 offset = glm::translate(glm::mat4(1.0f), glm::vec3(of, 0.0f, 0.0f));
  glm::mat4 view = environment::current_camera.get_view_matrix();
  glm::mat4 vp_matrix = offset * proj * view;

  {
    auto itr = objects->get_traversal_state(traversal_state<object*>::MODE::PREORDER);
    if (selection) {
      while(itr.next()) {
          // messy
          if (itr.get_item() == selection->get_data()) {
            itr.leave_branch();
            continue;
          }
          itr.get_item()->draw(vp_matrix);
      }
    } else {
      while(itr.next()) {
          itr.get_item()->draw(vp_matrix);
      }
    }
  }

  if (selection) {
      glStencilFunc(GL_ALWAYS, 1, 0xFF);
      {
        auto itr = selection->get_traversal_state(traversal_state<object*>::MODE::PREORDER);
        while(itr.next()) {
          itr.get_item()->draw(vp_matrix);
        }
      }
      glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
      {
        glDisable(GL_DEPTH_TEST);
        auto itr = selection->get_traversal_state(traversal_state<object*>::MODE::PREORDER);
        while(itr.next()) {
          itr.get_item()->draw(vp_matrix, 1.1f);
        }
        glEnable(GL_DEPTH_TEST);
      }
      glStencilFunc(GL_ALWAYS, 0, 0xFF);
  }
}
