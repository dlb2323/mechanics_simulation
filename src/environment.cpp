#include "environment.hpp"
#include "tree.hpp"
template <class T> bool traversal_state<T>::next() {
    // preorder
    T node = NULL;
    while (!node) {
        if (m_stack_pointer < 0)
            return false;
        if (m_stack[m_stack_pointer].node_pointer->get_child_count() == 0 ||
            m_stack[m_stack_pointer].leaf_pointer >=
            m_stack[m_stack_pointer].node_pointer->get_child_count()) {
            node = m_stack[m_stack_pointer].node_pointer->get_data();
            m_stack_pointer--;
        }
        else {
            m_stack[m_stack_pointer + 1] =
                state{ m_stack[m_stack_pointer].node_pointer->get_child(
                          m_stack[m_stack_pointer].leaf_pointer),
                      0 };
            m_stack[m_stack_pointer].leaf_pointer++;
            m_stack_pointer++;
        }
    }
    m_item = node;
    return true;
}

template <class T>
traversal_state<T>
tree_node<T>::get_traversal_state(typename traversal_state<T>::MODE mode) {
    return traversal_state<T>(mode, this);
};

template <class T> void tree_node<T>::insert_node(tree_node<T>* node, int idx) {
    if (idx >= 0 && idx <= children.size()) {
        children.insert(children.begin() + idx, node);
        return;
    }
    children.push_back(node);
    return;
}

template <class T> tree_node<T>* tree_node<T>::create_new(T data) {
    return new tree_node<T>(data);
}

template <class T> int tree_node<T>::size(tree_node<T>* node) {
    int size = 0;
    if (node) {
        size += 1;
        for (int i = 0; i < node->children.size(); i++)
            size += tree_node<T>::size(node->children[i]);
    }
    return size;
}

template <class T> void tree_node<T>::destroy(tree_node<T>* node) {
    if (node->data)
        delete node->data;
    for (int i = 0; i < node->children.size(); i++)
        tree_node<T>::destroy(node->children[i]);
    delete node;
}

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
  objects = tree_node<object*>::create_new(NULL);
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
  objects->insert_node(tree_node<object*>::create_new((object*)n));
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
  while(itr.next())
    objects->get_data()->draw(vp_matrix);

  if (selection) {
      glDisable(GL_DEPTH_TEST);
      selection->draw(vp_matrix, 1.1f);
      glEnable(GL_DEPTH_TEST);
  }
}
