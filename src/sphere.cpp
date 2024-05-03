#include "sphere.hpp"
void vertexObject::bindVertexArray() {
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
}
void vertexObject::bindElementArray() {
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
}
void vertexObject::bufferData(void* vboData, int vboCount, void* eboData, int eboCount) {
	bindVertexArray();
	glBufferData(GL_ARRAY_BUFFER, vboCount, vboData,
		     GL_STATIC_DRAW);
	bindElementArray();
	/* use the element array buffer to indicate which indices to draw */
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, eboCount, eboData,
		     GL_STATIC_DRAW);
	/* set the vertex attributes pointers */
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
	glEnableVertexAttribArray(0);
}
