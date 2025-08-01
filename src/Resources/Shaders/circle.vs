#version 460 core

layout (location = 0) in vec2 aPos;
uniform mat4 u_mvp;
out vec2 vTexCoords;

void main()
{
    gl_Position = u_mvp * vec4(aPos, 0.0, 1.0);
    vTexCoords = aPos * 0.5 + 0.5; // maps [-0.5,0.5] to [0,1]
}