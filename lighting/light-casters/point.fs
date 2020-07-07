#version 330 core

out vec4 FragColor;

struct Material
{
    sampler2D diffuse;
    sampler2D specular;
    float shininess;
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

in vec3 FragPos;
in vec3 LightPos;
in vec3 Normal;
in vec2 TexCoords;

uniform Material material;
uniform Light light;

void main()
{
    vec3 ambient = light.ambient * texture(material.diffuse, TexCoords).rgb;

    vec3 normal           = normalize(Normal);
    vec3 lightDir         = normalize(LightPos - FragPos);
    float diffuseStrength = max(dot(normal, lightDir), 0.0);
    vec3 diffuse          = light.diffuse * diffuseStrength * texture(material.diffuse, TexCoords).rgb;

    vec3 viewDir           = normalize(-FragPos);
    vec3 reflectDir        = reflect(-lightDir, normal);
    float specularStrength = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec3 specular          = vec3(0.0);

    if (specularStrength > 0.0)
        specular = light.specular * specularStrength * texture(material.specular, TexCoords).rgb;

    float distance    = length(LightPos - FragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));

    ambient *= attenuation;
    diffuse *= attenuation;
    specular *= attenuation;

    vec3 result = ambient + diffuse + specular;
    FragColor   = vec4(result, 1.0);
}
