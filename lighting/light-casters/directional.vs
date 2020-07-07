#version 330 core

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTexCoords;

out vec3 FragPos;
out vec3 LightDir;
out vec3 Normal;
out vec2 TexCoords;

struct Light
{
    vec3 direction;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform Light light;

void main()
{
    FragPos   = vec3(view * model * vec4(aPos, 1.0));
    LightDir  = vec3(view * vec4(-light.direction, 0.0));
    Normal    = mat3(transpose(inverse(view * model))) * aNormal;
    TexCoords = aTexCoords;

    gl_Position = projection * vec4(FragPos, 1.0);
}
