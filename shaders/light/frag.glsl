#version 330 core

out vec4 color;
  
uniform vec3 object_color = vec3(1.0, 1.0, 1.0);
uniform vec3 light_color = vec3(1.0, 1.0, 1.0);
uniform vec3 light_position = vec3(1.0, 1.0, 1.0);
uniform vec3 light_intensity = vec3(1.0, 1.0, 1.0);

void main()
{
    //color = vec4(light_color * object_color, 1.0f);
    float ambientStrength = 0.1f;
    vec3 ambient = ambientStrength * light_color;

    vec3 result = ambient * object_color;
    color = vec4(result, 1.0f);
}