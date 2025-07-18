#version 460 core

#define MAX_LIGHTS 8

struct Light {
    vec3 position;
    vec3 color;
    float specular; 
};

in vec3 FragPos;
in vec3 Normal;
out vec4 FragColor;

uniform Light lights[MAX_LIGHTS];
uniform int numLights;

uniform vec3 viewPos;
uniform vec3 objectColor;
uniform float specularIntensity;

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
        spec *= specularIntensity;

        // Combine (ambient + diffuse + specular)
        vec3 ambient = 0.1 * lights[i].color;
        vec3 diffuse = diff * lights[i].color;
        vec3 specular = spec * lights[i].color;

        result += (ambient + diffuse + specular);
    }
    FragColor = vec4(result * objectColor, 1.0);
}