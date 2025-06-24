#version 460 core
out vec4 FragColor;

in vec3 FragPos;
in vec3 Normal;

uniform vec3 objectColor;

void main()
{
    vec3 result = objectColor;
    FragColor = vec4(result, 1.0);
}