#version 330 core
layout (location = 0) in vec4 position;

uniform mat4 u_mvp;

out vec4 vertexColor;

void main()
{
    gl_Position= position;
}
