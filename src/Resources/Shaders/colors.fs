#version 460 core

#define MAX_LIGHTS 8

struct Light {
    vec3 position;
    vec3 color;
    float specular; 
};

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoord;
out vec4 FragColor;

uniform Light lights[MAX_LIGHTS];
uniform int numLights;

uniform vec3 viewPos;
uniform vec3 objectColor;
uniform sampler2D diffuseTexture; 
uniform bool useTexture;          

void main() 
{
    vec3 normal = normalize(Normal);
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 result = vec3(0.0);

    for (int i = 0; i < numLights; ++i) {
        // Diffuse
        vec3 lightDir = normalize(lights[i].position - FragPos);
        float diff = max(dot(normal, lightDir), 0.0);

        // Specular
        vec3 reflectDir = reflect(-lightDir, normal);
        float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32.0);
        spec *= lights[i].specular;

        // Combine (ambient + diffuse + specular)
        vec3 ambient = 0.1 * lights[i].color;
        vec3 diffuse = diff * lights[i].color;
        vec3 specular = spec * lights[i].color;

        result += (ambient + diffuse + specular);
    }
    vec3 baseColor = objectColor;
    if (useTexture)
    {
        baseColor = texture(diffuseTexture, TexCoord).rgb; //* objectColor;
    }

    FragColor = vec4(result * baseColor, 1.0);
}