#version 330 core
out vec4 FragColor;

uniform float u_time;

in vec4 vertexColor;

void main()
{
    FragColor = vec4(1.0, 0.4, 0.0, 1.0f);
} 
