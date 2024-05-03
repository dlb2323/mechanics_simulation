#include "object.hpp"
#include "GLFW/glfw3.h"
#include "simulation.hpp"
#include <cmath>
#include "utils.h"
void mesh::bind() {
  // bind shader and vertex array buffer for drawing
  m_shader->bind();
  glBindVertexArray(m_VAO);
}
void mesh::unbind() {
  // unbind 
  m_shader->unbind();
  glBindVertexArray(GL_NONE);
}
void mesh::write_begin() {
  // begin writing to the vertex buffer
  // vertex array buffer first to capture vertex and element order data
  glBindVertexArray(m_VAO);
  glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);
}
void mesh::write_end() {
  // end writing to mesh, unbind in order of binding
  glBindVertexArray(GL_NONE);
  glBindBuffer(GL_ARRAY_BUFFER, GL_NONE);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, GL_NONE);
}

// switch main shader
void mesh::set_shader(shader *s) { m_shader = s; }

void mesh::set_elements(unsigned int elements) {
  m_elements = elements; 
}

void mesh::draw() {
  // draw elements
  glDrawElements(GL_TRIANGLES, m_elements, GL_UNSIGNED_INT, 0);
}

void line_mesh::draw() {
  // draw elements
  glDrawElements(GL_LINES, m_elements, GL_UNSIGNED_INT, 0);
}

// object
void object::update(float delta) {
  if (m_mode == MODE::ACTIVE) {
    // take the proportion of time left and smooth it
    // linear interpolate the result to get the position between the start and end points
    position = lerp3f(m_start, m_end, smooth(m_timestamp.get_elapsed_time() / m_total_time));
    if (m_timestamp.get_elapsed_time() > m_total_time) {
      // snap to final position after the movement time is over, and exit motion state
      position = m_end;
      m_mode = MODE::STILL;
    }
  }
}

// main draw function
void object::draw(glm::mat4& vp_matrix) const {
  // bind mesh for opengl draw state 
  m_mesh->bind();
  // take view projection matrix and transform it into a modelviewprojection matrix to pass into the shader 
  glm::mat4 mvp = vp_matrix * model_matrix();
  // pass uniforms to shader
  glUniformMatrix4fv(m_mesh->get_shader()->mvp_location(), 1, GL_FALSE, glm::value_ptr(mvp));
  glUniform3fv(m_mesh->get_shader()->colour_location(), 1, glm::value_ptr(m_colour));
  glUniform1f(m_mesh->get_shader()->time_location(), (float)glfwGetTime());
  // opengl draw vertices
  m_mesh->draw();
  // unbind mesh
  m_mesh->unbind();

}

// additional draw function used to draw outlines
void object::draw(glm::mat4& vp_matrix, float scale) const {
    // swap out shader for single colour
    shader* mesh_shader = m_mesh->get_shader();
    m_mesh->set_shader(shader::single_colour);
    m_mesh->bind();

    glm::mat4 model = model_matrix();
    // further scale the model matrix
    model = glm::scale(model, glm::vec3(1.0f)*scale);
    glm::mat4 mvp = vp_matrix * model;
    glUniformMatrix4fv(m_mesh->get_shader()->mvp_location(), 1, GL_FALSE, glm::value_ptr(mvp));
    glUniform3fv(m_mesh->get_shader()->colour_location(), 1, glm::value_ptr(m_colour));
    glUniform1f(m_mesh->get_shader()->time_location(), (float)glfwGetTime());
    m_mesh->draw();
    m_mesh->unbind();
    // return to original shader
    m_mesh->set_shader(mesh_shader);
}

// world
line_mesh* world::world_mesh;

glm::mat4 world::model_matrix() const {
  glm::mat4 model = glm::mat4(1.0f);
  model = glm::translate(model, position);
  model = glm::rotate(model, (float)glfwGetTime()/20, glm::vec3(0.0f, 1.0f, 0.0f));
  model = glm::scale(model, glm::vec3(1.0f)*m_scale);
  return model;
}

void world::update(float delta) {
  object::update(delta);
  if (GUI::get_state() == GUI::SIMULATE) {
    current_simulation->update();
  }
}


void world::gen_vertex_data(line_mesh &plane_mesh) {
  int data_locations = 8*3;
  // vertices form a square in 2d space
  float* data = new float[data_locations] {
    -0.5f, -0.5f, 0.5f,
    0.5f, -0.5f, 0.5f,
    0.5f, 0.5f, 0.5f,
    -0.5f, 0.5f, 0.5f,
    -0.5f, -0.5f, -0.5f,
    0.5f, -0.5f, -0.5f,
    0.5f, 0.5f, -0.5f,
    -0.5f, 0.5f, -0.5f
  };
  int vertices = 3*4*2;
  int* tree = new int[vertices] {
    0, 1, 1, 2, 2, 3, 3, 0, // front face 
    0, 4, 1, 5, 2, 6, 3, 7, // connections
    4, 5, 5, 6, 6, 7, 7, 4, // back face
  };

  // opengl bind vertex buffers for writing
  plane_mesh.write_begin();
  glEnableVertexAttribArray(0);
  glBufferData(GL_ARRAY_BUFFER, data_locations * sizeof(float), data,
               GL_STATIC_DRAW);
  /* use the element array buffer to indicate which indices to draw */
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, vertices * sizeof(int), tree,
               GL_STATIC_DRAW);
  /* set the vertex attributes pointers */
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

  // clean up data from heap
  delete[] tree;
  delete[] data;
  // stop writing to vertex buffers
  plane_mesh.write_end();
  // set the number of elements to draw
  plane_mesh.set_elements(vertices);
}

void world::child_added(object* child) {
  // update info 
  DEBUG_TEXT("child added to world")
  switch (child->get_type_code()) {
    case 1: {
      if (!simulation_objects.pl) {
        simulation_objects.pl = static_cast<plane*>(child);
        child->set_modified_callback(static_cast<void (*)(GUIitem*)>(&world::reset_simulation));
        child->set_callback_node(this);
        DEBUG_TEXT("plane added to simulation state")
      }
      break;
    }
    case 3: {
      if (!simulation_objects.pa) {
        simulation_objects.pa = static_cast<particle*>(child);
        child->set_modified_callback(static_cast<void (*)(GUIitem*)>(&world::reset_simulation));
        child->set_callback_node(this);
        DEBUG_TEXT("particle added to simulation state")
      }
      break;
    }
    case 0:
    default:
    break;
  }
  create_simulation();
}

bool world::create_simulation() {
  if (simulation_objects.pa && simulation_objects.pl) {
    DEBUG_TEXT("simulation state set to particle and plane")
    current_simulation = new pp(this, simulation_objects.pa, simulation_objects.pl);
  } else {
    return false;
  }
  return true;
}

void world::child_removed(object* child) {
  // update info 
  if (child == simulation_objects.pa)
    simulation_objects.pa = NULL;
  else if (child == simulation_objects.pl)
    simulation_objects.pl = NULL;

  if (current_simulation)
    delete current_simulation;
  if (!create_simulation()) {
    current_simulation = NULL;
  }
}

void world::show() const {
  // display simulation options with imgui
  float old_distance = distance;
  if (ImGui::InputFloat("x", (float*)&distance, 1.0f, 10.0f))
    reset_simulation((GUIitem*)this);
  ImGui::InputFloat("m", (float*)&mass, 0.0f, 10.0f);
  ImGui::InputFloat("g", (float*)&gravity, 0.0f, 10.0f);
  ImGui::InputFloat("f", (float*)&force, 0.0f, 10.0f);
  ImGui::InputFloat("u", (float*)&u_velocity, 0.0f, 10.0f);
}

// point
// generate point model matrix
glm::mat4 point::model_matrix() const {
  glm::mat4 model = glm::mat4(1.0f);
  model = glm::translate(model, position);
  model = glm::scale(
      model, glm::vec3(1.0f)*m_scale);
  return model;
}

void point::show() const {
}

// plane
mesh* plane::plane_mesh;

void plane::gen_vertex_data(mesh &plane_mesh) {
  int data_locations = 4*3;
  // vertices form a square in 2d space
  float* data = new float[data_locations] {
    -0.5f, -0.5f, 0.0f,
    0.5f, -0.5f, 0.0f,
    0.5f, 0.5f, 0.0f,
    -0.5f, 0.5f, 0.0f
  };
  int vertices = 6;
  int* tree = new int[vertices] {
    0, 1, 2, 2, 3, 0
  };

  // opengl bind vertex buffers for writing
  plane_mesh.write_begin();
  glEnableVertexAttribArray(0);
  glBufferData(GL_ARRAY_BUFFER, data_locations * sizeof(float), data,
               GL_STATIC_DRAW);
  /* use the element array buffer to indicate which indices to draw */
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, vertices * sizeof(int), tree,
               GL_STATIC_DRAW);
  /* set the vertex attributes pointers */
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);

  // clean up data from heap
  delete[] tree;
  delete[] data;
  // stop writing to vertex buffers
  plane_mesh.write_end();
  // set the number of elements to draw
  plane_mesh.set_elements(vertices);
}

glm::mat4 plane::model_matrix() const {
    glm::mat4 model = glm::mat4(1.0f);
    float rot = M_PI/2.0f;
    model = glm::translate(model, position);
    // change model orientation to be flat horizontally in 3d space
    model = glm::rotate(model, rot, glm::vec3(0.0f, 1.0f, 0.0f));
    model = glm::rotate(model, rot, glm::vec3(1.0f, 0.0f, 0.0f));
    // rotate
    model = glm::rotate(model, rotation, glm::vec3(1.0f, 0.0f, 0.0f));
    // stretch to required length
    model = glm::scale(model, glm::vec3(1.2f, length, 1.0f));
    model = glm::scale(
        model, glm::vec3(1.0f)*m_scale);
    return model;
}

void plane::show() const {
  if (ImGui::SliderFloat("rotation", (float *)&rotation, 0.0f, M_PI/2))
    (*m_value_modified)(callback_node);
  ImGui::SliderFloat("length", (float *)&length, 0.0f, 70.0f);
}

// particle
mesh* particle::particle_mesh;

void particle::gen_vertex_data(unsigned int nodes, mesh &particle_mesh) {
  double phi, theta;
  int data_locations = 3 * (nodes * (nodes - 1) + 2);
  float *data = new float[data_locations];
  float *d_p = data;

  // top of the sphere
  *d_p++ = 0; // x
  *d_p++ = 1; // y
  *d_p++ = 0; // z
  for (int i = 0; i < nodes - 1; i++) {
    phi = M_PI * (i + 1) / nodes;
    for (int j = 0; j < nodes; j++) {
      theta = 2.0f * M_PI * j / nodes;

      *d_p++ = sin(phi) * cos(theta); // x
      *d_p++ = cos(phi);              // y
      *d_p++ = sin(phi) * sin(theta); // z
    }
  }
  // bottom of the sphere
  *d_p++ = 0;  // x
  *d_p++ = -1; // y
  *d_p++ = 0;  // z
  
  int vertices = nodes + (nodes - 2) * nodes;
  int *tree = new int[vertices * 6];
  int *t_p = tree;
  // top and bottom triangles
  for (int i = 0; i < nodes; ++i) {
    int i0 = i + 1;
    int i1 = (i + 1) % nodes + 1;
    *t_p++ = 0;
    *t_p++ = i1;
    *t_p++ = i0;
    i0 = i + nodes * (nodes - 2) + 1;
    i1 = (i + 1) % nodes + nodes * (nodes - 2) + 1;
    *t_p++ = data_locations/3-1;
    *t_p++ = i0;
    *t_p++ = i1;
  }

  for (int j = 0; j < nodes - 2; j++) {
    int j0 = j * nodes + 1;
    int j1 = (j + 1) * nodes + 1;
    for (int i = 0; i < nodes; i++) {
      int i0 = j0 + i;
      int i1 = j0 + (i + 1) % nodes;
      int i2 = j1 + (i + 1) % nodes;
      int i3 = j1 + i;
      *t_p++ = i0;
      *t_p++ = i1;
      *t_p++ = i2;
      *t_p++ = i0;
      *t_p++ = i3;
      *t_p++ = i2;
    }
  }

  // write vertex data to buffers
  particle_mesh.write_begin();
  glEnableVertexAttribArray(0);
  glBufferData(GL_ARRAY_BUFFER, data_locations * sizeof(float), data,
               GL_STATIC_DRAW);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, vertices * 6 * sizeof(int), tree,
               GL_STATIC_DRAW);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);

  // clean up
  delete[] tree;
  delete[] data;
  particle_mesh.write_end();
  particle_mesh.set_elements(vertices*6);
}

// generates model matrix
glm::mat4 particle::model_matrix() const {
  glm::mat4 model = glm::mat4(1.0f);
  model = glm::translate(model, position);
  model = glm::scale(
      model, glm::vec3(1.0f)*m_scale);
  return model;
}


void particle::show() const {
  ImGui::SliderFloat("scale", (float *)&m_scale, 0.0f, 30.0f);
    (*m_value_modified)(callback_node);
}

// spring
mesh* spring::spring_mesh;
mesh* spring::spring_mesh_highlight;
float spring::coil_width;
int spring::coils;

glm::mat4 spring::model_matrix() const {
  glm::mat4 model = glm::mat4(1.0f);
  float length_unit = coil_width*coils;
  float current_length = length-extension;
  model = glm::translate(model, position);
  model = glm::rotate(model, (float)glfwGetTime()/6, glm::vec3(0.0f, 1.0f, 0.0f));
  model = glm::scale(model, glm::vec3(1.0f, current_length, 1.0f));
  model = glm::scale(model, glm::vec3(1.0f)*m_scale);
  model = glm::translate(model, glm::vec3(0.0f, length_unit/2, 0.0f));
  return model;
}

void spring::gen_vertex_data(const int coils, const int nodes, const float coil_width, float thickness, mesh &mesh) {
  int data_locations = 3*nodes*coils*3;

  float* data = new float[data_locations];
  float* d_p = data;

  // generate coil
  const float step_z = coil_width/(float)nodes;
  const float twist = M_PI/8;

  for (int i = 0; i < 3; i++) {
    float z_offset = sin(2*i*M_PI/3+twist)*thickness*0.1;
    float y_offset = cos(2*i*M_PI/3+twist)*thickness*0.1 - coil_width*coils/2.0f;
    for (int j = 0; j < nodes*coils; j++) {
      *d_p++ = sin(2*M_PI*j/nodes); // x 
      *d_p++ = j*step_z+y_offset;   // y
      *d_p++ = cos(2*M_PI*j/nodes)+z_offset; // z
    }
  }

  int vertices = 18*(nodes*coils-1)+6;
  int* tree = new int[vertices];
  int* t_p = tree;
  int end = nodes*coils;
  for (int i = 0; i < end-1; i++) {
    *t_p++ = i; 
    *t_p++ = end+i; 
    *t_p++ = end+i+1; 
    *t_p++ = i; 
    *t_p++ = i+1; 
    *t_p++ = end+i+1; 
    //
    *t_p++ = i; 
    *t_p++ = 2*end+i; 
    *t_p++ = 2*end+i+1; 
    *t_p++ = i; 
    *t_p++ = i+1; 
    *t_p++ = 2*end+i+1; 
    //
    *t_p++ = end+i; 
    *t_p++ = end+i+1; 
    *t_p++ = 2*end+i+1; 
    *t_p++ = end+i; 
    *t_p++ = 2*end+i; 
    *t_p++ = 2*end+i+1; 
  }
  // base and top
  *t_p++ = 0;
  *t_p++ = end;
  *t_p++ = 2*end;
  *t_p++ = end-1;
  *t_p++ = 2*end-1;
  *t_p++ = 3*end-1;


  // opengl bind vertex buffers for writing
  mesh.write_begin();
  glEnableVertexAttribArray(0);
  glBufferData(GL_ARRAY_BUFFER, data_locations * sizeof(float), data,
               GL_STATIC_DRAW);
  /* use the element array buffer to indicate which indices to draw */
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, vertices * sizeof(int), tree,
               GL_STATIC_DRAW);
  /* set the vertex attributes pointers */
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);

  // clean up data from heap
  delete[] tree;
  delete[] data;
  // stop writing to vertex buffers
  mesh.write_end();
  // set the number of elements to draw
  mesh.set_elements(vertices);
}

// additional draw function used to draw outlines
void spring::draw(glm::mat4& vp_matrix, float scale) const {
    // swap out shader for single colour
    spring::spring_mesh_highlight->bind();

    glm::mat4 model = model_matrix();
    glm::mat4 mvp = vp_matrix * model;
    glUniformMatrix4fv(spring::spring_mesh_highlight->get_shader()->mvp_location(), 1, GL_FALSE, glm::value_ptr(mvp));
    glUniform3fv(spring::spring_mesh_highlight->get_shader()->colour_location(), 1, glm::value_ptr(m_colour));
    glUniform1f(spring::spring_mesh_highlight->get_shader()->time_location(), (float)glfwGetTime());
    spring::spring_mesh_highlight->draw();
    spring::spring_mesh_highlight->unbind();
    // return to original shader
}

void spring::show() const {
  if (ImGui::SliderFloat("extension", (float*)&extension, 0.0f, length))
    (*m_value_modified)(callback_node);
  if (ImGui::SliderFloat("length", (float*)&length, 0.0f, 20.0f))
    (*m_value_modified)(callback_node);
  if (ImGui::InputFloat("scale", (float*)&m_scale))
    (*m_value_modified)(callback_node);
}
