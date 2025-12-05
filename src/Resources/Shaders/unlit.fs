#version 460 core

in VS_OUT 
{
    vec3 FragPos;
    vec2 TexCoords;
    vec3 NormalW;
    mat3 TBN;
} fs_in;

in vec3 FragPos;


out vec4 FragColor;

uniform vec3 viewPos;
uniform vec3 objectColor;


void main()
{
    vec3 albedo = objectColor;
    
    FragColor = vec4(albedo, 1.0);
}
