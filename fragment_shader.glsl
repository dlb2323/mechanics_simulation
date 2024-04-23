#version 330 core
out vec4 FragColor;

uniform float u_time;

in vec4 vertexColor;

void main()
{
    FragColor = vec4((vertexColor.x+12.f)/200.f, (vertexColor.y+12.f)/200.f, (vertexColor.z+12.f)/200.f, 1.0f);
} 
