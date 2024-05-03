#include "object.hpp"
#include <cmath>
void mesh::bind() {
  m_shader->bind();
  glBindVertexArray(m_VAO);
}
void mesh::unbind() {
  m_shader->unbind();
  glBindVertexArray(GL_NONE);
}
void mesh::write_begin() {
  glBindVertexArray(m_VAO);
  glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);
}
void mesh::write_end() {
  glBindVertexArray(GL_NONE);
  glBindBuffer(GL_ARRAY_BUFFER, GL_NONE);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, GL_NONE);
}

void mesh::set_shader(shader *s) { m_shader = s; }

void mesh::set_elements(unsigned int elements) {
  m_elements = elements; 
}
void mesh::draw() {
  glDrawElements(GL_TRIANGLES, m_elements, GL_UNSIGNED_INT, 0);
}
// object
void object::update(float delta) {
  if (m_mode == MODE::ACTIVE) {
    if (m_timestamp.get_elapsed_time() > m_total_time) {
      position = m_end;
      m_mode = MODE::STILL;
    }
    position = lerp(m_start, m_end, smooth(m_timestamp.get_elapsed_time() / m_total_time));
  }
}

// world

void world::draw(glm::mat4 &vp_matrix) const {};
void world::draw(glm::mat4 &vp_matrix, float scale) const {};

void world::update(float delta) {
  this->object::update(delta);
}

void world::show() const {
  ImGui::SliderFloat("friction", (float*)&friction, 0.0f, 4.0f);
  ImGui::SliderFloat("distance", (float*)&distance, 1.0f, 7.0f);
}

// point
void point::draw(glm::mat4& vp_matrix) const {
  if (selected) {
        glStencilFunc(GL_ALWAYS, 1, 0xFF);
  }

  m_mesh->bind();

  glm::mat4 model = glm::mat4(1.0f);
  model = glm::translate(model, position);
  model = glm::rotate(model, (float)glfwGetTime() / 20,
                      glm::vec3(1.0f, 0.0f, 1.0f));
  model = glm::scale(
      model, glm::vec3(1.0f)*m_scale);

  glm::mat4 mvp = vp_matrix * model;
  glUniformMatrix4fv(m_mesh->get_shader()->mvp_location(), 1, GL_FALSE, glm::value_ptr(mvp));
  glUniform1f(m_mesh->get_shader()->time_location(), (float)glfwGetTime());

  m_mesh->draw();

  m_mesh->unbind();

  if (selected) {
      glStencilFunc(GL_ALWAYS, 0, 0xFF);
  }
}

void point::draw(glm::mat4& vp_matrix, float scale) const {
    if (selected) {
      glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
    }
    shader* mesh_shader = m_mesh->get_shader();
    m_mesh->set_shader(shader::single_colour);
    m_mesh->bind();

    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, position);
    model = glm::rotate(model, (float)glfwGetTime() / 20,
                        glm::vec3(1.0f, 0.0f, 1.0f));
    model = glm::scale(
        model, glm::vec3(1.0f)*m_scale*scale);

    glm::mat4 mvp = vp_matrix * model;
    glUniformMatrix4fv(m_mesh->get_shader()->mvp_location(), 1, GL_FALSE, glm::value_ptr(mvp));
    glUniform1f(m_mesh->get_shader()->time_location(), (float)glfwGetTime());
    m_mesh->draw();
    m_mesh->unbind();
    m_mesh->set_shader(mesh_shader);
    if (selected) {
      glStencilFunc(GL_ALWAYS, 0, 0xFF);
    }
}

void point::show() const {
  ImGui::SliderFloat("scale", (float *)&m_scale, 0.0f, 30.0f);
}

// plane
mesh* plane::plane_mesh;

void plane::gen_vertex_data(mesh &plane_mesh) {
  int data_locations = 4*3;
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

  plane_mesh.write_begin();
  glEnableVertexAttribArray(0);
  glBufferData(GL_ARRAY_BUFFER, data_locations * sizeof(float), data,
               GL_STATIC_DRAW);
  /* use the element array buffer to indicate which indices to draw */
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, vertices * sizeof(int), tree,
               GL_STATIC_DRAW);
  /* set the vertex attributes pointers */
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);

  delete[] tree;
  delete[] data;
  plane_mesh.write_end();
  plane_mesh.set_elements(vertices);
}
void plane::draw(glm::mat4& vp_matrix) const {
  if (selected) {
        glStencilFunc(GL_ALWAYS, 1, 0xFF);
  }

  m_mesh->bind();

  glm::mat4 model = glm::mat4(1.0f);
  model = glm::translate(model, position);
  model = glm::rotate(model, (float)glfwGetTime() / 20,
                      glm::vec3(1.0f, 0.0f, 1.0f));
  model = glm::scale(
      model, glm::vec3(1.0f)*m_scale);

  glm::mat4 mvp = vp_matrix * model;
  glUniformMatrix4fv(m_mesh->get_shader()->mvp_location(), 1, GL_FALSE, glm::value_ptr(mvp));
  glUniform1f(m_mesh->get_shader()->time_location(), (float)glfwGetTime());

  m_mesh->draw();

  m_mesh->unbind();

  if (selected) {
      glStencilFunc(GL_ALWAYS, 0, 0xFF);
  }
}

void plane::draw(glm::mat4& vp_matrix, float scale) const {
    if (selected) {
      glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
    }
    shader* mesh_shader = m_mesh->get_shader();
    m_mesh->set_shader(shader::single_colour);
    m_mesh->bind();

    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, position);
    model = glm::rotate(model, rotation, glm::vec3(1.0f, 0.0f, 0.0f));
    model = glm::scale(
        model, glm::vec3(1.0f)*m_scale*scale);

    glm::mat4 mvp = vp_matrix * model;
    glUniformMatrix4fv(m_mesh->get_shader()->mvp_location(), 1, GL_FALSE, glm::value_ptr(mvp));
    glUniform1f(m_mesh->get_shader()->time_location(), (float)glfwGetTime());
    m_mesh->draw();
    m_mesh->unbind();
    m_mesh->set_shader(mesh_shader);
    if (selected) {
      glStencilFunc(GL_ALWAYS, 0, 0xFF);
    }
}

void plane::show() const {
  ImGui::SliderFloat("scale", (float *)&m_scale, 0.0f, 30.0f);
  ImGui::SliderFloat("rotation", (float *)&rotation, 0.0f, 2.0f);
}

// particle
mesh* particle::particle_mesh;

void particle::gen_vertex_data(unsigned int nodes, mesh &particle_mesh) {
  double phi, theta;
  int data_locations = 3 * (nodes * (nodes - 1) + 2);
  float *data = new float[data_locations];
  float *d_p = data;

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

  particle_mesh.write_begin();
  glEnableVertexAttribArray(0);
  glBufferData(GL_ARRAY_BUFFER, data_locations * sizeof(float), data,
               GL_STATIC_DRAW);
  /* use the element array buffer to indicate which indices to draw */
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, vertices * 6 * sizeof(int), tree,
               GL_STATIC_DRAW);
  /* set the vertex attributes pointers */
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);

  delete[] tree;
  delete[] data;
  particle_mesh.write_end();
  particle_mesh.set_elements(vertices*6);
}

void particle::draw(glm::mat4& vp_matrix) const {
  if (selected) {
        glStencilFunc(GL_ALWAYS, 1, 0xFF);
  }

  m_mesh->bind();

  glm::mat4 model = glm::mat4(1.0f);
  model = glm::translate(model, position);
  model = glm::rotate(model, (float)glfwGetTime() / 20,
                      glm::vec3(1.0f, 0.0f, 1.0f));
  model = glm::scale(
      model, glm::vec3(1.0f)*m_scale);

  glm::mat4 mvp = vp_matrix * model;
  glUniformMatrix4fv(m_mesh->get_shader()->mvp_location(), 1, GL_FALSE, glm::value_ptr(mvp));
  glUniform1f(m_mesh->get_shader()->time_location(), (float)glfwGetTime());

  m_mesh->draw();

  m_mesh->unbind();

  if (selected) {
      glStencilFunc(GL_ALWAYS, 0, 0xFF);
  }
}

void particle::draw(glm::mat4& vp_matrix, float scale) const {
    if (selected) {
      glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
    }
    shader* mesh_shader = m_mesh->get_shader();
    m_mesh->set_shader(shader::single_colour);
    m_mesh->bind();

    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, position);
    model = glm::rotate(model, (float)glfwGetTime() / 20,
                        glm::vec3(1.0f, 0.0f, 1.0f));
    model = glm::scale(
        model, glm::vec3(1.0f)*m_scale*scale);

    glm::mat4 mvp = vp_matrix * model;
    glUniformMatrix4fv(m_mesh->get_shader()->mvp_location(), 1, GL_FALSE, glm::value_ptr(mvp));
    glUniform1f(m_mesh->get_shader()->time_location(), (float)glfwGetTime());
    m_mesh->draw();
    m_mesh->unbind();
    m_mesh->set_shader(mesh_shader);
    if (selected) {
      glStencilFunc(GL_ALWAYS, 0, 0xFF);
    }
}

void particle::show() const {
  ImGui::SliderFloat("scale", (float *)&m_scale, 0.0f, 30.0f);
}

void particle::update(float delta) {
  this->object::update(delta);
}
