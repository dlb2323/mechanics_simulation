#include "environment.hpp"

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
camera environment::current_camera;

void environment::update(float delta) {}

sphere *environment::create(std::string &name, unsigned int radius) {
  sphere *n = new sphere(name, &sphere_mesh, radius);
  objects.push_back(n);
  // messy
  return n;
}

void environment::draw() {
  int width, height;
  glfwGetFramebufferSize(window, &width, &height);
  proj =
      glm::perspective((float)(M_PI / 4), (float)width / height, 0.1f, 300.0f);
  // draw
  glm::mat4 view = environment::current_camera.get_view_matrix();
  glm::mat4 vp_matrix = proj * view;

  for (int i = 0; i < objects.size(); i++) {
    objects[i]->draw(vp_matrix);
  }
}
