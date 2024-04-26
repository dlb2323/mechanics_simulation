#include "sphere.hpp"
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

void sphere::gen_vertex_data(unsigned int nodes, mesh &sphere_mesh) {
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


  sphere_mesh.write_begin();
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
  sphere_mesh.write_end();
  sphere_mesh.set_elements(vertices*6);
}
void mesh::set_elements(unsigned int elements) {
  m_elements = elements; 
}

void mesh::draw() {
  glDrawElements(GL_TRIANGLES, m_elements, GL_UNSIGNED_INT, 0);
}
