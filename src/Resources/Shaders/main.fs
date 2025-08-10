#version 460 core

struct Material 
{
    sampler2D diffuse;
    sampler2D specular;
    float shininess;
};

struct DirLight 
{
    vec3 direction;	
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct PointLight 
{
    vec3 position;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
	
    float constant;
    float linear;
    float quadratic;
};

#define MAX_LIGHTS 8

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;

out vec4 FragColor;

uniform vec3 viewPos;
uniform vec3 objectColor;
uniform DirLight dirLight;
uniform PointLight point_lights[MAX_LIGHTS];
uniform int numPointLights;
uniform Material material;

uniform bool useMaterial;          

vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir);
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir);
vec3 sampleDiffuse();
vec3 sampleSpecular();

void main() 
{
    vec3 normal = normalize(Normal);
    vec3 viewDir = normalize(viewPos - FragPos);

    // directional lighting
    vec3 result = CalcDirLight(dirLight, normal, viewDir);
    // point lights
    for(int i = 0; i < numPointLights; i++)
        result += CalcPointLight(point_lights[i], normal, FragPos, viewDir);  
    
    FragColor = vec4(result, 1.0);
}

// calculates the color when using a directional light.
vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir)
{
    vec3 lightDir = normalize(-light.direction);
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    // combine results
    vec3 ambient = light.ambient * sampleDiffuse();
    vec3 diffuse = light.diffuse * diff * sampleDiffuse();
    vec3 specular = light.specular * spec * sampleSpecular();
    return (ambient + diffuse + specular);
}

// calculates the color when using a point light.
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
    vec3 lightDir = normalize(light.position - fragPos);
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    // attenuation
    float distance = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));    
    // combine results
    vec3 ambient = light.ambient * sampleDiffuse();
    vec3 diffuse = light.diffuse * diff * sampleDiffuse();
    vec3 specular = light.specular * spec * sampleSpecular();
    ambient *= attenuation;
    diffuse *= attenuation;
    specular *= attenuation;
    return (ambient + diffuse + specular);
}


vec3 sampleDiffuse()
{
    return useMaterial ? vec3(texture(material.diffuse, TexCoords)) : objectColor;                                 
}

vec3 sampleSpecular()
{
    return useMaterial ? vec3(texture(material.specular, TexCoords)) : vec3(1.0);
}