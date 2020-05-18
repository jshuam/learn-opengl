#version 330 core

in vec3 FragPos;
in vec3 Normal;

out vec4 FragColor;

uniform vec3 objectColor;
uniform vec3 lightColor;
uniform vec3 lightPos;
uniform vec3 viewPos;

void main()
{
    float ambientStrength = 0.4;
    vec3 ambient          = ambientStrength * lightColor;

    vec3 normal           = normalize(Normal);
    vec3 lightDir         = normalize(lightPos - FragPos);
    float diffuseStrength = max(dot(normal, lightDir), 0.0);
    vec3 diffuse          = diffuseStrength * lightColor;

    float specularStrength  = 0.5;
    vec3 viewDir            = normalize(viewPos - FragPos);
    vec3 reflectDir         = reflect(-lightDir, normal);
    float shininessStrength = pow(max(dot(viewDir, reflectDir), 0.0), 32);
    vec3 specular           = specularStrength * shininessStrength * lightColor;

    vec3 result = (ambient + diffuse + specular) * objectColor;
    FragColor   = vec4(result, 1.0);
};
