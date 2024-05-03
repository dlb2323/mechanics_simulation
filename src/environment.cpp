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
  subjects = {NULL, NULL, NULL, NULL, NULL};
  objects = tree_node<object*>::create_new(new world(plane::plane_mesh, 1.0f));
  subjects.w = (world*)objects->get_data();
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
  if (is_simulation_legal()) {
    subjects.pl->move_to(glm::vec3(0.0f, 0.0f, 0.0f));
    float l = subjects.w->distance;
    subjects.pl->length = l/15;
    glm::mat4 t(1.0f);
    t = glm::rotate(t, (subjects.pl->rotation)+(float)M_PI/2.0f, glm::vec3(0.0f, 0.0f, -1.0f));
    t = glm::translate(t, glm::vec3(l, 0.0f, 0.0f));
    glm::vec3 offset = t[3];
    subjects.point_a->move_to(offset+glm::vec3(0.0f, 10.0f, 0.0f));
    subjects.point_b->move_to(-offset+glm::vec3(0.0f, 10.0f, 0.0f));
    subjects.pa->move_to(offset+glm::vec3(0.0f, 10.0f, 0.0f));
  }
}

tree_node<object*>* environment::create(object* o) {
  glm::vec3 pos(std::rand() % 100 - 50, std::rand() % 100 - 50,
              std::rand() % 100 - 50);
  tree_node<object*>* node = tree_node<object*>::create_new(o);
  if (selection) {
    selection->insert_node(node);
    glm::vec3 select_pos = selection->get_data()->position;
    pos += select_pos;
    o->position = select_pos;
  } else
    objects->insert_node(node);
  o->move_to(pos); 
  switch (o->get_type_code()) {
    case 1:
      subjects.pl = (plane*)o;
      break;
    case 2: {
      if (subjects.point_a)
        subjects.point_b = (point*)o;
      else
        subjects.point_a = (point*)o;
      }
      break;
    case 3:
      subjects.pa = (particle*)o;
    case 0:
    default:
    break;
  }
  // messy
  return node;
}

bool environment::is_simulation_legal() {
  return subjects.w && subjects.point_a && subjects.point_b && subjects.pl && subjects.pa;
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
