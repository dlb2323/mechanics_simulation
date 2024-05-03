#include "sphere.hpp"
void vertexObject::bindVertexArray() {
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
}
void vertexObject::bindElementArray() {
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
}
void sphere::bufferData(void* vboData, int vboCount, void* eboData, int eboCount) {
	m_vObj->bindVertexArray();
	glBufferData(GL_ARRAY_BUFFER, vboCount, vboData,
		     GL_STATIC_DRAW);
	m_vObj->bindElementArray();
	/* use the element array buffer to indicate which indices to draw */
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, eboCount, eboData,
		     GL_STATIC_DRAW);
	/* set the vertex attributes pointers */
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
	glEnableVertexAttribArray(0);
}

void vertexObject::set_shader(shader* s) {
	m_shader = s;
}
