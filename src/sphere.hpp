#ifndef SPHERE_H
#define SPHERE_H

#include "shader.hpp"

#include <cmath>
static void gen_sphere(unsigned int nodes) {
  double phi, theta;
  float *data = new float[3 * nodes * (nodes + 1) * sizeof(float)];
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
  int *tree = new int[vertices * 6 * sizeof(int)];
  for (int i = 0; i < vertices; i += 1) {
    tree[i * 6] = i;
    tree[i * 6 + 1] = i + 1;
    tree[i * 6 + 2] = i + nodes;
    tree[i * 6 + 3] = i + nodes;
    tree[i * 6 + 4] = i + nodes + 1;
    tree[i * 6 + 5] = i + 1;
  }

  delete[] tree;	
  delete[] data;
}

class vertexObject {
  shader* shader_id;
  unsigned int VBO;
  unsigned int EBO;
  void bindVertexArray();
  void bindElementArray();
public:
  vertexObject() {
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

  }
  void bufferData();
};

class sphere : vertexObject {
  float radius;
public:
  sphere() {

  }
};

#endif // !SPHERE_H
