#version 460 core 

layout(location=0) in vec3 vertexPos;
layout(location=1) in vec3 vertexNormal;
layout(location=2) in vec2 vertexTexCoords;
layout(location=3) in vec3 vertexTangent;
layout(location=4) in vec3 vertexBitangent;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;


out VS_OUT 
{
    vec3 FragPos;
    vec2 TexCoords;
    vec3 NormalW;
    mat3 TBN;
} vs_out;

void main()
{ 
    vec3 P = vec3(model * vec4(vertexPos, 1.0));
    mat3 Nrm = transpose(inverse(mat3(model)));

    vec3 N = normalize(Nrm * vertexNormal);
    vec3 T = normalize(Nrm * vertexTangent);
         T = normalize(T - dot(T, N) * N);      // Gram-Schmidt
    vec3 B = cross(N, T);

    vs_out.FragPos   = P;
    vs_out.TexCoords = vertexTexCoords;
    vs_out.NormalW   = N;                       // fallback if no normal map
    vs_out.TBN       = mat3(T, B, N);

    gl_Position = projection * view * vec4(P, 1.0);
}