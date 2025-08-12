#version 460 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D texture1;

void main()
{
   vec4 color = texture(texture1, TexCoords);
   if (color.a == 0.0) {
      discard;
   }
   FragColor = color;
}