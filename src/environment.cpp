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
  // if (is_simulation_legal()) {
  //   // update simulation state, if all simulation objects are present
  //   // move plane to zero vector
  //   subjects.pl->move_to(glm::vec3(0.0f, 0.0f, 0.0f));
  //   // using matrix transformations to obtain a vector on the line parallel to the plane
  //   // create matrix t to perform rotations 
  //   glm::mat4 t(1.0f);
  //   // rotate t parallel to the plane
  //   t = glm::rotate(t, (subjects.pl->rotation), glm::vec3(0.0f, 0.0f, -1.0f));
  //   // translate a distance across the plane 
  //   t = glm::translate(t, glm::vec3(subjects.w->distance, 0.0f, 0.0f));
  //   // take the resultant position of the matrix 
  //   glm::vec3 offset = t[3];
  //   // move to this position with an extra offset to appear above the plane
  //   subjects.pa->move_to(offset+glm::vec3(0.0f, 10.0f, 0.0f));
  // }
  // if (GUI::get_state() == GUI::SIMULATE) {
  //   // update particle position when simulating
  //   subjects.pa->position = simulation_func();
  // }
}

// creates a new branch from an object and adds it to the tree 
void environment::create(object* o) {
  // random vector displacement to give a scattering effect
  glm::vec3 pos(std::rand() % 100 - 50, std::rand() % 100 - 50,
              std::rand() % 100 - 50);
  // create tree node from object
  tree_node<object*>* node = tree_node<object*>::create_new(o);
  // if a node is selected, add branch as a leaf of this node
  if (selection) {
    if (node->get_parent()->get_data()->get_type_code() == 0)
      static_cast<world*>(node->get_parent()->get_data())->child
    selection->insert_node(node);
    // translate object to selection position
    // updates destination with selection position
    // changes scatter source to selection
    glm::vec3 select_pos = selection->get_data()->position;
    pos += select_pos;
    o->position = select_pos;
  } else
    objects->insert_node(node);
  o->move_to(pos); 
  // add to object to simulation state based on type
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
}

// check if a simulation is possible given the simulation state
bool environment::is_simulation_legal() {
  // simulation is legal if none of the objects are NULL
  return subjects.w && subjects.pl && subjects.pa;
}

// draw environment
// draws object tree
void environment::draw() {
  // get window width and height
  int width, height;
  glfwGetFramebufferSize(window, &width, &height);
  GLFWmonitor* monitor = glfwGetWindowMonitor(window);
  if (monitor) {
    // get screen dimensions if fullscreen
    const GLFWvidmode* mode = glfwGetVideoMode(monitor);
    width = mode->width;
    height = mode->height;
  }
  // update projection matrix for draw phase
  proj = glm::perspective((float)(M_PI / 4), (float)width / height, 0.1f, 300.0f);
  // shift screen 'centre' to account for gui
  float of = (float)(((500.0 + (width-500.0)/2.0)-width/2.0)/(width/2.0));
  glm::mat4 offset = glm::translate(glm::mat4(1.0f), glm::vec3(of, 0.0f, 0.0f));
  glm::mat4 view = environment::current_camera.get_view_matrix();
  // view projection matrix
  glm::mat4 vp_matrix = offset * proj * view;

  {
    // traverse tree in preorder mode to skip branches if necessary
    auto itr = objects->get_traversal_state(traversal_state<object*>::MODE::PREORDER);
    if (selection) {
      while(itr.next()) {
          // if nodes are selected, skip them for now 
          if (itr.get_item() == selection->get_data()) {
            itr.leave_branch();
            continue;
          }
          itr.get_item()->draw(vp_matrix);
      }
    } else {
      // draw tree completely 
      while(itr.next()) {
          itr.get_item()->draw(vp_matrix);
      }
    }
  }

  if (selection) {
      // TODO 
      glStencilFunc(GL_ALWAYS, 1, 0xFF);
      {
        // draw skipped nodes with updated stencil state
        auto itr = selection->get_traversal_state(traversal_state<object*>::MODE::PREORDER);
        while(itr.next()) {
          itr.get_item()->draw(vp_matrix);
        }
      }
      glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
      {
        // disable depth testing for outlines
        glDisable(GL_DEPTH_TEST);
        auto itr = selection->get_traversal_state(traversal_state<object*>::MODE::PREORDER);
        while(itr.next()) {
          // draw outlines scaled 
          itr.get_item()->draw(vp_matrix, 1.1f);
        }
        glEnable(GL_DEPTH_TEST);
      }
      glStencilFunc(GL_ALWAYS, 0, 0xFF);
  }
}
