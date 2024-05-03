#include "environment.hpp"
#include "tree.hpp"
#include "object.hpp"


// camera

// enter focus state
// in focus state, a start and end point are defined
// the camera will move between these points for the m_total_time seconds 
void camera::focus(const glm::vec3 &point) {
  // record current time
  m_timestamp.begin();
  // set start and end positions
  m_focus_point = point;
  m_start = m_position;
  // set state
  m_mode = camera::MODE::FOCUS;
}

// enter track state
// in track state, a start point is defined, but the end point 
// is continuously updated as the target's position vector changes
// the camera will move between these the start and the target's position vector 
// for m_total_time seconds after which it will snap to the target's position each frame 
void camera::track(glm::vec3 *target) {
  m_timestamp.begin();
  // set pointer to target's position vector
  m_p_target = target;
  m_start = m_position;
  m_mode = camera::MODE::TRACK;
}

// camera update function, called every frame
void camera::update() {
  // switch state
  switch (m_mode) {
  case camera::MODE::FOCUS: {
    // take the proportion of time left and smooth it
    // linear interpolate the result to get the position between the start and end points
    m_position = lerp(m_start, m_focus_point, smooth(m_timestamp.get_elapsed_time() / m_total_time));
    if (m_timestamp.get_elapsed_time() > m_total_time) {
      // snap to end point, exit focus state
      m_position = m_focus_point;
      m_mode = camera::MODE::STILL;
    }
    break;
  }
  case camera::MODE::TRACK: {
    if (!m_p_target) {
      // if target no longer exists, exit track state
      m_mode = camera::MODE::STILL;
    } else {
      if (m_timestamp.get_elapsed_time() > m_total_time) {
        // motion time complete, snap to target position
        m_position = *m_p_target;
      } else {
        // linear interpolate between start and target position
        m_position = lerp(m_start, *m_p_target, smooth(m_timestamp.get_elapsed_time() / m_total_time));
      }
    }
    break;
  }
  case camera::MODE::STILL:
  // do nothing
  default:
    break;
  }
}

// environment constructor
environment::environment(GLFWwindow *window)
    : window(window) {
  // initialise world, plane, and particle pointers to NULL state
  subjects = {NULL, NULL, NULL};
  // initialise root node to type world 
  objects = tree_node<object*>::create_new(new world(plane::plane_mesh, 1.0f));
  // pass world to simulation data
  subjects.w = (world*)objects->get_data();
  // initialise selection to NULL state
  selection = NULL;
}
// environment destructor
environment::~environment() {
  // delete tree
  tree_node<object*>::destroy(objects);
};

// initialise static camera
camera environment::current_camera;

// environment update function, called each frame
// updates all nodes in the object tree
void environment::update(float delta) {
  // create tree iterator
  auto itr = objects->get_traversal_state(traversal_state<object*>::MODE::PREORDER);
  // update objects 
  while(itr.next()) {
    itr.get_item()->update(delta);
  }
  if (is_simulation_legal()) {
    // update simulation state, if all simulation objects are present
    // move plane to zero vector
    subjects.pl->move_to(glm::vec3(0.0f, 0.0f, 0.0f));
    // using matrix transformations to obtain a vector on the line parallel to the plane
    // create matrix t to perform rotations 
    glm::mat4 t(1.0f);
    // rotate t parallel to the plane
    t = glm::rotate(t, (subjects.pl->rotation), glm::vec3(0.0f, 0.0f, -1.0f));
    // translate a distance across the plane 
    t = glm::translate(t, glm::vec3(subjects.w->distance, 0.0f, 0.0f));
    // take the resultant position of the matrix 
    glm::vec3 offset = t[3];
    // move to this position with an extra offset to appear above the plane
    subjects.pa->move_to(offset+glm::vec3(0.0f, 10.0f, 0.0f));
  }
  if (GUI::get_state() == GUI::SIMULATE) {
    // update particle position when simulating
    subjects.pa->position = simulation_func();
  }
}

glm::vec3 environment::simulation_func() {
  float l = subjects.w->distance;
  glm::mat4 t(1.0f);
  t = glm::rotate(t, (subjects.pl->rotation), glm::vec3(0.0f, 0.0f, -1.0f));
  t = glm::translate(t, glm::vec3(l, 0.0f, 0.0f));
  glm::vec3 offset = t[3];
  glm::vec3 position = offset+glm::vec3(0.0f, 10.0f, 0.0f); 
  float r = (
    ((subjects.w->force - subjects.w->mass*subjects.w->gravity
    *sin(subjects.pl->rotation)))
    /2*subjects.w->mass)
    *subjects.time.get_elapsed_time()*subjects.time.get_elapsed_time() 
    + subjects.w->u_velocity*subjects.time.get_elapsed_time();
  return position - glm::normalize(offset)*r*subjects.pa->get_radius();
}

void environment::simulation_start() {
    deselect();
    current_camera.track(&subjects.pa->position);
    subjects.time.begin();
    subjects.pl->position = glm::vec3(0.0f, 0.0f, 0.0f);
    float l = subjects.w->distance;
    // subjects.pl->length = l/15;
    glm::mat4 t(1.0f);
    t = glm::rotate(t, (subjects.pl->rotation), glm::vec3(0.0f, 0.0f, -1.0f));
    t = glm::translate(t, glm::vec3(l, 0.0f, 0.0f));
    glm::vec3 offset = t[3];
    subjects.pa->position = offset+glm::vec3(0.0f, 10.0f, 0.0f);
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
      if (!subjects.pl)
        subjects.pl = (plane*)o;
      break;
    case 3:
      if (!subjects.pa)
        subjects.pa = (particle*)o;
    case 0:
    default:
    break;
  }
  return node;
}

bool environment::is_simulation_legal() {
  return subjects.w && subjects.pl && subjects.pa;
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
