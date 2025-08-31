#version 460 core

struct Material 
{
    sampler2D diffuse;
    sampler2D specular;
    sampler2D normal;
    float shininess; // treated as Blinn exponent; Phong ≈ 4x
};

struct DirLight 
{
    bool enabled;
    vec3 direction;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct PointLight 
{
    bool enabled;
    vec3 position;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float att_constant;
    float att_linear;
    float att_quadratic;
};

#define MAX_LIGHTS 8

in VS_OUT 
{
    vec3 FragPos;
    vec2 TexCoords;
    vec3 NormalW;
    mat3 TBN;
} fs_in;

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

uniform bool useMaterial;
uniform bool hasDiffuseMap;
uniform bool hasSpecularMap;
uniform bool hasNormalMap;
uniform bool useBlinnPhong;

vec3 CalcDirLight(DirLight L, vec3 N, vec3 V, vec3 albedo, vec3 specTint);
vec3 CalcPointLight(PointLight L, vec3 N, vec3 P, vec3 V, vec3 albedo, vec3 specTint);

void main()
{
    // Normal: from normal map if available, else interpolated world normal
    vec3 N = fs_in.NormalW;
    if (useMaterial && hasNormalMap) 
    {
        vec3 n_ts = texture(material.normal, fs_in.TexCoords).xyz * 2.0 - 1.0;
        N = normalize(fs_in.TBN * n_ts);
    } else {
        N = normalize(N);
    }

    vec3 V = normalize(viewPos - fs_in.FragPos);

    // Sample albedo/spec with graceful fallbacks
    vec3 albedo   = (useMaterial && hasDiffuseMap)  ? texture(material.diffuse,  fs_in.TexCoords).rgb
                                                    : objectColor;
    // If there is no specular map, use a neutral tint (1,1,1). You can reduce this if highlights are too strong.
    vec3 specTint = (useMaterial && hasSpecularMap) ? texture(material.specular, fs_in.TexCoords).rgb
                                                    : vec3(1.0);

    vec3 color = vec3(0.0);

    if (dirLight.enabled)
        color += CalcDirLight(dirLight, N, V, albedo, specTint);

    for (int i = 0; i < numPointLights; ++i)
        if (point_lights[i].enabled)
            color += CalcPointLight(point_lights[i], N, fs_in.FragPos, V, albedo, specTint);

    FragColor = vec4(color, 1.0);
}

float SpecularTerm(vec3 N, vec3 L, vec3 V)
{
    // Map shininess so toggling keeps roughly similar lobe widths:
    // Phong exponent ≈ 4 * Blinn exponent
    float blinnExp = max(material.shininess, 1.0);
    float phongExp = blinnExp * 4.0;

    if (useBlinnPhong) 
    {
        vec3 H = normalize(L + V);
        return pow(max(dot(N, H), 0.0), blinnExp);
    } 
    else 
    {
        vec3 R = reflect(-L, N);
        return pow(max(dot(V, R), 0.0), phongExp);
    }
}

vec3 CalcDirLight(DirLight L, vec3 N, vec3 V, vec3 albedo, vec3 specTint)
{
    vec3 Ldir = normalize(-L.direction);

    float diff = max(dot(N, Ldir), 0.0);
    float spec = SpecularTerm(N, Ldir, V);

    vec3 ambient  = L.ambient  * albedo;                
    vec3 diffuse  = L.diffuse  * diff    * albedo;
    vec3 specular = L.specular * spec    * specTint;

    return ambient + diffuse + specular;
}

vec3 CalcPointLight(PointLight L, vec3 N, vec3 P, vec3 V, vec3 albedo, vec3 specTint)
{
    vec3 Ldir = normalize(L.position - P);

    float diff = max(dot(N, Ldir), 0.0);
    float spec = SpecularTerm(N, Ldir, V);

    float dist = length(L.position - P);
    float att  = 1.0 / (L.att_constant + L.att_linear * dist + L.att_quadratic * dist * dist);

    vec3 ambient  = L.ambient  * albedo;                  
    vec3 diffuse  = L.diffuse  * diff    * albedo  * att;
    vec3 specular = L.specular * spec    * specTint * att;

    return ambient + diffuse + specular;
}