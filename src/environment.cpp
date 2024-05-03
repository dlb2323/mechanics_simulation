#include "environment.hpp"
#include "tree.hpp"

glm::vec3 lerp(glm::vec3 x, glm::vec3 y, float t) {
  return x * (1.f - t) + y * t;
}

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

double camera::smooth(double x) { return tanh(sqrt(x) * 6 - M_PI) / 2 + 0.503; }

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
    : window(window),
      main_shader("vertex_shader.glsl", "fragment_shader.glsl"),
      single_colour("vertex_shader.glsl", "single_colour_shader.glsl"),
      sphere_mesh(&main_shader) {
    std::string r = "root";
  objects = tree_node<object*>::create_new(new world(&sphere_mesh, 1, &single_colour));
  selection = NULL;
  sphere::gen_vertex_data(120, sphere_mesh);
}
environment::~environment() {
  auto traverse = objects->get_traversal_state(traversal_state<object*>::MODE::PREORDER);
  while(traverse.next())
    delete traverse.get_item();
  tree_node<object*>::destroy(objects);
};

camera environment::current_camera;

void environment::update(float delta) {}

sphere *environment::create(std::string &name, unsigned int radius) {
  sphere *n = new sphere(name, &sphere_mesh, radius, &single_colour);
  select(n);  
  objects->insert_node(tree_node<object*>::create_new(n));
  // messy
  return n;
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

  auto itr = objects->get_traversal_state(traversal_state<object*>::MODE::PREORDER);
  while(itr.next()) {
    itr.get_item()->draw(vp_matrix);
  }

  if (selection) {
      glDisable(GL_DEPTH_TEST);
      selection->draw(vp_matrix, 1.1f);
      glEnable(GL_DEPTH_TEST);
  }
}
