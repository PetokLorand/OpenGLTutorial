#version 410 core

struct Fragment
{
    vec3 position;
    vec3 normal;
    vec2 texCoords;
};

struct Light
{
    vec3 position;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    float constant;
    float linear;
    float quadratic;
};

uniform Light light;
uniform vec3 viewPos;

uniform sampler2D texture_diffuse1;
uniform sampler2D texture_specular1;
uniform float shininess;

in Fragment fragment;

out vec4 FragColor;

vec3 calculateLighting(Fragment fragment, Light light, vec3 viewPos)
{
    vec3 ambient = light.ambient * vec3(texture(texture_diffuse1, fragment.texCoords));

    vec3 norm = normalize(fragment.normal);
    vec3 lightDir = normalize(light.position - fragment.position);
    float diff = max(dot(norm, lightDir), 0.0f);
    vec3 diffuse = light.diffuse * diff * vec3(texture(texture_diffuse1, fragment.texCoords));

    vec3 viewDir = normalize(viewPos - fragment.position);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0f), shininess);
    vec3 specular = light.specular * spec * vec3(texture(texture_specular1, fragment.texCoords));

    vec3 phong = ambient + diffuse + specular;

    float distance = length(light.position - fragment.position);
    float attenuation = 1.0f / (light.constant + light.linear * distance + light.quadratic * (distance * distance));

    phong *= attenuation;

    return phong;
}

void main()
{
    vec3 phong = calculateLighting(fragment, light, viewPos);
    
    FragColor = vec4(phong, 1.0f);
}
