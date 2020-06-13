#version 330 core

struct Material
{
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
};

struct Light
{
    vec3 position;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

in vec3 FragPos;
in vec3 Normal;
in vec3 LightPos;

out vec4 FragColor;

uniform Material material;
uniform Light light;

void main()
{
    vec3 ambient = light.ambient * material.ambient;

    vec3 normal           = normalize(Normal);
    vec3 lightDir         = normalize(LightPos - FragPos);
    float diffuseStrength = max(dot(normal, lightDir), 0.0);
    vec3 diffuse          = light.diffuse * (diffuseStrength * material.diffuse);

    vec3 viewDir           = normalize(-FragPos);
    vec3 reflectDir        = reflect(-lightDir, normal);
    float specularStrength = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec3 specular          = light.specular * (specularStrength * material.specular);

    vec3 result = ambient + diffuse + specular;
    FragColor   = vec4(result, 1.0);
}
