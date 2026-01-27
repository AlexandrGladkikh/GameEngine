#version 410 core
out vec4 FragColor;

uniform vec4 inColor = vec4(1.0, 1.0, 1.0, 1.0);

void main()
{
   FragColor = inColor;
}