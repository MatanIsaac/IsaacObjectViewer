#version 460 core 

layout(location=0) in vec3 vertexPos;
layout(location=1) in vec3 vertexNormal;
layout(location=2) in vec2 vertexTexCoords;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform vec3 viewPos;

out vec3 FragPos;
out vec3 Normal;
out vec2 TexCoords;

void main()
{ 
    FragPos = vec3(model * vec4(vertexPos, 1.0));
    Normal = mat3(transpose(inverse(model))) * vertexNormal;
    TexCoords = vertexTexCoords;
    
    gl_Position = projection * view * vec4(FragPos, 1.0);
}
