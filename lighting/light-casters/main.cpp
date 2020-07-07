#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/ext/matrix_transform.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>
#include <glm/gtx/transform.hpp>

#include <algorithm>
#include <functional>
#include <iostream>

#include "camera.hpp"
#include "cube.hpp"
#include "shader.hpp"
#include "textured_cube.hpp"

float g_windowWidth  = 1280.0f;
float g_windowHeight = 720.0f;
float g_aspectRatio  = g_windowWidth / g_windowHeight;

GLuint g_containerMap;
GLuint g_containerSpecMap;
GLuint g_containerEmission;
TexturedCube* g_crate;

glm::vec3 cubePositions[] = { glm::vec3(0.0f, 0.0f, 0.0f),    glm::vec3(2.0f, 5.0f, -15.0f),
                              glm::vec3(-1.5f, -2.2f, -2.5f), glm::vec3(-3.8f, -2.0f, -12.3f),
                              glm::vec3(2.4f, -0.4f, -3.5f),  glm::vec3(-1.7f, 3.0f, -7.5f),
                              glm::vec3(1.3f, -2.0f, -2.5f),  glm::vec3(1.5f, 2.0f, -2.5f),
                              glm::vec3(1.5f, 0.2f, -1.5f),   glm::vec3(-1.3f, 1.0f, -1.5f) };

Shader g_dirLightShader;
Shader g_pointLightShader;

Camera g_camera;

float deltaTime = 0.0f;

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    g_windowWidth  = width;
    g_windowHeight = height;
    g_aspectRatio  = static_cast<float>(width) / height;

    glViewport(0, 0, width, height);
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    g_camera.rotate(xpos, ypos);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    g_camera.zoom(yoffset);
}

void processInput(GLFWwindow* window)
{
    g_camera.move(window, deltaTime);
}

void setupScene()
{
    g_dirLightShader.loadShaders("directional.vs", "directional.fs");
    g_dirLightShader.use();

    g_dirLightShader.setInt("material.diffuse", 0);
    g_dirLightShader.setInt("material.specular", 1);
    g_dirLightShader.setInt("material.emission", 2);
    g_dirLightShader.setFloat("material.shininess", 64.0f);

    g_dirLightShader.setVec3("light.direction", glm::vec3(0.0f, 0.0f, -1.0f));
    g_dirLightShader.setVec3("light.ambient", glm::vec3(0.15f));
    g_dirLightShader.setVec3("light.diffuse", glm::vec3(0.85f));
    g_dirLightShader.setVec3("light.specular", glm::vec3(1.0f));

    g_pointLightShader.loadShaders("point.vs", "point.fs");
    g_pointLightShader.use();

    g_pointLightShader.setInt("material.diffuse", 0);
    g_pointLightShader.setInt("material.specular", 1);
    g_pointLightShader.setInt("material.emission", 2);
    g_pointLightShader.setFloat("material.shininess", 64.0f);

    g_pointLightShader.setVec3("light.position", glm::vec3(0.0f, 0.0f, -5.0f));
    g_pointLightShader.setVec3("light.ambient", glm::vec3(0.15f));
    g_pointLightShader.setVec3("light.diffuse", glm::vec3(0.85f));
    g_pointLightShader.setVec3("light.specular", glm::vec3(1.0f));

    g_pointLightShader.setFloat("light.constant", 1.0f);
    g_pointLightShader.setFloat("light.linear", 0.09f);
    g_pointLightShader.setFloat("light.quadratic", 0.032f);

    glGenTextures(1, &g_containerMap);
    glBindTexture(GL_TEXTURE_2D, g_containerMap);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    int width, height, numChannels;
    stbi_set_flip_vertically_on_load_thread(true);
    unsigned char* data = stbi_load("container2.png", &width, &height, &numChannels, 0);

    if (data)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    stbi_image_free(data);

    glGenTextures(1, &g_containerSpecMap);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, g_containerSpecMap);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    stbi_set_flip_vertically_on_load_thread(true);
    data = stbi_load("container2_specular.png", &width, &height, &numChannels, 0);

    if (data)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    stbi_image_free(data);

    glGenTextures(1, &g_containerEmission);
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, g_containerEmission);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    stbi_set_flip_vertically_on_load_thread(true);
    data = stbi_load("matrix.jpg", &width, &height, &numChannels, 0);

    if (data)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    stbi_image_free(data);

    g_crate           = new TexturedCube();
    g_crate->modelMat = glm::mat4(1.0f);
    g_dirLightShader.setMat4("model", g_crate->modelMat);
}

void renderScene()
{
    // g_dirLightShader.use();

    // g_dirLightShader.setMat4("view", g_camera.view());
    // g_dirLightShader.setMat4("projection", g_camera.projection(g_aspectRatio));

    g_pointLightShader.use();

    g_pointLightShader.setMat4("view", g_camera.view());
    g_pointLightShader.setMat4("projection", g_camera.projection(g_aspectRatio));

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, g_containerMap);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, g_containerSpecMap);

    for (unsigned int i = 0; i < 10; ++i)
    {
        glm::mat4 model = glm::mat4(1.0f);
        model           = glm::translate(model, cubePositions[i]);
        float angle     = 20.0f * i;
        model           = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
        // g_dirLightShader.setMat4("model", model);

        g_pointLightShader.setMat4("model", model);

        g_crate->draw();
    }
}

int main(void)
{
    if (!glfwInit())
    {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return -1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    GLFWwindow* window = glfwCreateWindow(g_windowWidth, g_windowHeight, "Light Casters", NULL, NULL);
    if (window == NULL)
    {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    if (glewInit() != GLEW_OK)
    {
        std::cerr << "Failed to initialize GLEW" << std::endl;
        return -1;
    }

    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetScrollCallback(window, scroll_callback);
    glfwSetCursorPosCallback(window, mouse_callback);

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

    glfwSetInputMode(window, GLFW_STICKY_KEYS, GLFW_TRUE);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    glEnable(GL_DEPTH_TEST);

    setupScene();

    float currentFrameTime;
    float lastFrameTime = 0;

    while (!glfwWindowShouldClose(window))
    {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        processInput(window);

        renderScene();

        lastFrameTime    = currentFrameTime;
        currentFrameTime = glfwGetTime();
        deltaTime        = currentFrameTime - lastFrameTime;

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    delete g_crate;

    glfwTerminate();

    return 0;
}
