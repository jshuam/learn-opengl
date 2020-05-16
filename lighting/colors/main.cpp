#include <GL/glew.h>
#include <GLFW/glfw3.h>
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

const float g_windowWidth  = 1280.0f;
const float g_windowHeight = 720.0f;

Cube* coralCube;

Cube* light;
glm::vec3 lightPos(1.2f, 1.0f, 2.0f);

Shader g_objectShader;
Shader g_lightShader;

Camera g_camera;
float deltaTime = 0.0f;

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
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
    g_objectShader.loadShaders("colors.vs", "colors.fs");
    g_objectShader.use();
    g_objectShader.setVec3("objectColor", glm::vec3(1.0f, 0.5f, 0.31f));
    g_objectShader.setVec3("lightColor", glm::vec3(1.0f, 1.0f, 1.0f));

    g_lightShader.loadShaders("light.vs", "light.fs");

    coralCube           = new Cube();
    coralCube->modelMat = glm::mat4(1.0f);

    light           = new Cube();
    light->modelMat = glm::mat4(1.0f);
    light->modelMat = glm::translate(light->modelMat, lightPos);
    light->modelMat = glm::scale(light->modelMat, glm::vec3(0.2f));
}

void renderScene()
{
    g_objectShader.use();
    g_objectShader.setMat4("model", coralCube->modelMat);
    g_objectShader.setMat4("view", g_camera.view());
    g_objectShader.setMat4("projection", g_camera.projection(g_windowWidth / g_windowHeight));
    coralCube->draw();

    g_lightShader.use();
    g_lightShader.setMat4("model", light->modelMat);
    g_lightShader.setMat4("view", g_camera.view());
    g_lightShader.setMat4("projection", g_camera.projection(g_windowWidth / g_windowHeight));
    light->draw();
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

    GLFWwindow* window = glfwCreateWindow(g_windowWidth, g_windowHeight, "Colors", NULL, NULL);
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

    delete coralCube;
    delete light;

    glfwTerminate();

    return 0;
}
