#version 460
layout(std430, binding = 0) buffer TVertex { vec4 vertex[]; };
uniform mat4  u_mvp;
uniform vec2  u_resolution;
uniform float u_thickness;

void main()
{
    int tri_i = gl_VertexID % 6;

    // 1) Work in CLIP space first (NO divide yet)
    vec4 aC = u_mvp * vertex[0];
    vec4 bC = u_mvp * vertex[1];

    // Clip against near plane: z + w >= 0  (OpenGL)
    float fa = aC.z + aC.w;
    float fb = bC.z + bC.w;

    // If both behind, kill by collapsing
    if (fa < 0.0 && fb < 0.0) {
        gl_Position = vec4(2e9,2e9,1,1); // off-screen degenerate
        return;
    }

    // If A behind, move it to the intersection with near plane
    if (fa < 0.0) {
        float t = fa / (fa - fb);       // solve mix(aC,bC,t) so z+w==0
        aC = mix(aC, bC, t);
    }
    // If B behind, move it to the intersection with near plane
    if (fb < 0.0) {
        float t = fb / (fb - fa);
        bC = mix(bC, aC, t);
    }

    // 2) Now do perspective divide to NDC
    vec3 aN = aC.xyz / aC.w;
    vec3 bN = bC.xyz / bC.w;

    // 3) Go to screen space (pixels)
    vec2 a = (aN.xy + 1.0) * 0.5 * u_resolution;
    vec2 b = (bN.xy + 1.0) * 0.5 * u_resolution;

    // Guard against degeneracy
    vec2 dir = b - a;
    float len = max(length(dir), 1e-6);
    dir /= len;
    vec2 n = vec2(-dir.y, dir.x);

    float halfT = 0.5 * u_thickness;
    vec2 p0 = a + n * halfT;
    vec2 p1 = a - n * halfT;
    vec2 p2 = b + n * halfT;
    vec2 p3 = b - n * halfT;

    vec2 pos; float z;
    if (tri_i == 0) { pos = p0; z = aN.z; }
    if (tri_i == 1) { pos = p1; z = aN.z; }
    if (tri_i == 2) { pos = p2; z = bN.z; }
    if (tri_i == 3) { pos = p2; z = bN.z; }
    if (tri_i == 4) { pos = p1; z = aN.z; }
    if (tri_i == 5) { pos = p3; z = bN.z; }

    vec2 ndc = pos / u_resolution * 2.0 - 1.0;
    gl_Position = vec4(ndc, z, 1.0);
}
