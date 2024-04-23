#include "sphere.hpp"
void vertexObject::bind() {
	glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);
}

void vertexObject::unbind() {
	glBindBuffer(GL_ARRAY_BUFFER, GL_NONE);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, GL_NONE);
}


void vertexObject::set_shader(shader* s) {
	m_shader = s;
}

void gen_sphere_vertex_data(unsigned int nodes, vertexObject* vObj) {
  double phi, theta;
  float *data = new float[3 * nodes * (nodes + 1)];
  float *d_p = data;

  for (int i = 0; i <= nodes; i++) {
    for (int j = 0; j < nodes; j++) {
      phi = 2 * M_PI * i / nodes;
      theta = M_PI * j / nodes;

      *d_p++ = sin(theta) * cos(phi); // x
      *d_p++ = sin(theta) * sin(phi); // y
      *d_p++ = cos(theta);            // z
    }
  }

  int vertices = nodes * (nodes);
  int *tree = new int[vertices * 6];
  for (int i = 0; i < vertices; i += 1) {
    tree[i * 6] = i;
    tree[i * 6 + 1] = i + 1;
    tree[i * 6 + 2] = i + nodes;
    tree[i * 6 + 3] = i + nodes;
    tree[i * 6 + 4] = i + nodes + 1;
    tree[i * 6 + 5] = i + 1;
  }

  vObj->bind();

  glBufferData(GL_ARRAY_BUFFER, 3*nodes*(nodes+1)*sizeof(float), data, GL_STATIC_DRAW);
  /* use the element array buffer to indicate which indices to draw */
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, vertices*6*sizeof(int), tree, GL_STATIC_DRAW);
  /* set the vertex attributes pointers */
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);

  vObj->unbind();
  delete[] tree;
  delete[] data;
}
