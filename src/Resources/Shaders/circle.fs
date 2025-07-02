#version 460 core

#ifdef GL_ES
precision mediump float;
#endif

out vec4 FragColor;

in vec2 vTexCoord;

uniform vec3 objectColor;

float circle(in vec2 _st, in float _radius){
    vec2 dist = _st-vec2(0.5);
    return 1.0 - smoothstep(_radius-(_radius*0.01),
                            _radius+(_radius*0.01),
                            dot(dist,dist)*4.0);
}

void main() {

    float alpha = circle(vTexCoord, 0.1);
    FragColor = vec4(objectColor, alpha);
}
