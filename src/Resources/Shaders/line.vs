#version 460
layout(std430, binding = 0) buffer TVertex
{
    vec4 vertex[]; // two endpoints
};
uniform mat4 u_mvp;
uniform vec2 u_resolution;
uniform float u_thickness;
void main()
{
    int tri_i = gl_VertexID % 6;
    vec4 va = u_mvp * vertex[0];
    vec4 vb = u_mvp * vertex[1];
    va.xyz /= va.w;
    vb.xyz /= vb.w;
    vec2 a = (va.xy + 1.0) * 0.5 * u_resolution;
    vec2 b = (vb.xy + 1.0) * 0.5 * u_resolution;
    vec2 lineDir = normalize(b - a);
    vec2 normal = vec2(-lineDir.y, lineDir.x);
    vec2 p[4];
    p[0] = a + normal * (u_thickness * 0.5);
    p[1] = a - normal * (u_thickness * 0.5);
    p[2] = b + normal * (u_thickness * 0.5);
    p[3] = b - normal * (u_thickness * 0.5);
    vec2 pos;
    if (tri_i == 0) pos = p[0];
    if (tri_i == 1) pos = p[1];
    if (tri_i == 2) pos = p[2];
    if (tri_i == 3) pos = p[2];
    if (tri_i == 4) pos = p[1];
    if (tri_i == 5) pos = p[3];
    vec2 ndc = pos / u_resolution * 2.0 - 1.0;
    float z = mix(va.z, vb.z, (tri_i < 3) ? 0.0 : 1.0);
    gl_Position = vec4(ndc, z, 1.0);
}