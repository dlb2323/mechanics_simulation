#version 330 core
out vec4 FragColor;

in vec4 vertexColor;

void main()
{
    FragColor = vec4((vertexColor.x+12.f)/200.f, (vertexColor.y+12.f)/200.f, (vertexColor.z+12.f)/200.f, 1.0f);
} 
