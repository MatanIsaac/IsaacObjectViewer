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

uniform DirLight    dirLight;
uniform PointLight  point_lights[MAX_LIGHTS];
uniform int         numPointLights;

uniform Material material;

// Controls whether we use material samplers at all, and if those samplers are bound
uniform bool useMaterial;
uniform bool hasDiffuseMap;
uniform bool hasSpecularMap;
uniform bool useBlinnPhong;

// ---- Helpers
vec3 CalcDirLight(DirLight light, vec3 N, vec3 V, vec3 albedo, vec3 specTint);
vec3 CalcPointLight(PointLight light, vec3 N, vec3 P, vec3 V, vec3 albedo, vec3 specTint);

void main()
{
    vec3 normal = normalize(Normal);
    vec3 V = normalize(viewPos - FragPos);

    // Sample once (with graceful fallbacks)
    vec3 albedo   = (useMaterial && hasDiffuseMap)  ? texture(material.diffuse,  TexCoords).rgb : objectColor;
    vec3 specTint = (useMaterial && hasSpecularMap) ? texture(material.specular, TexCoords).rgb : vec3(1.0);

    vec3 color = CalcDirLight(dirLight, normal, V, albedo, specTint);
    for (int i = 0; i < numPointLights; ++i)
        color += CalcPointLight(point_lights[i], normal, FragPos, V, albedo, specTint);

    FragColor = vec4(color, 1.0);
}

vec3 CalcDirLight(DirLight L, vec3 normal, vec3 V, vec3 albedo, vec3 specTint)
{
    vec3 Ldir = normalize(-L.direction);

    float diff = max(dot(normal, Ldir), 0.0);
    float spec = 0.0;
    if(useBlinnPhong)
    {
        vec3 H = normalize(Ldir + V);
        spec = pow(max(dot(normal, H), 0.0), material.shininess);
    }
    else
    {
        vec3 R = reflect(-Ldir, normal);
        spec = pow(max(dot(V, R), 0.0), material.shininess);
    }

    vec3 ambient  = L.ambient  * albedo;
    vec3 diffuse  = L.diffuse  * diff    * albedo;
    vec3 specular = L.specular * spec    * specTint;

    return ambient + diffuse + specular;
}

vec3 CalcPointLight(PointLight L, vec3 normal, vec3 P, vec3 V, vec3 albedo, vec3 specTint)
{
    vec3 lightDir = normalize(L.position - P);

    float diff = max(dot(normal, lightDir), 0.0);

    float spec = 0.0;

    if (useBlinnPhong)
    {
        vec3 halfwayDir = normalize(lightDir + V);
        spec = pow(max(dot(normal, halfwayDir), 0.0), material.shininess);
    }
    else
    {
        vec3 reflectDir = reflect(-lightDir, normal);
        spec = pow(max(dot(V, reflectDir), 0.0), material.shininess);
    }

    float dist = length(L.position - P);
    float att  = 1.0 / (L.constant + L.linear * dist + L.quadratic * (dist * dist));

    vec3 ambient  = L.ambient  * albedo;
    vec3 diffuse  = L.diffuse  * diff    * albedo;
    vec3 specular = L.specular * spec    * specTint;

    return (ambient + diffuse + specular) * att;
}
