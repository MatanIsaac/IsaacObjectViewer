#version 330 core 

in vec2 fragmentTexCoord;

out vec4 FragColor;

// texture samplers
uniform sampler2D texture1;

void main()
{
    FragColor = texture(texture1,fragmentTexCoord);
}

