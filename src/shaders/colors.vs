#version 330 core 

layout(location=0) in vec3 vertexPos;
layout(location=1) in vec3 vertexNormal;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform vec3 viewPos;
uniform float specularIntensity;

out vec3 FragPos;
out vec3 Normal;

void main()
{ 
    FragPos = vec3(model * vec4(vertexPos, 1.0));
    Normal = mat3(transpose(inverse(model))) * vertexNormal;

    gl_Position = projection * view * vec4(FragPos, 1.0);
}
