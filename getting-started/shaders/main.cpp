#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <algorithm>
#include <functional>
#include <iostream>
#include <math.h>

#include "shader.h"

float vertices[] = {
    // vertex 1
    -0.5f,
    -0.5f,
    0.0f, // position
    1.0f,
    0.0f,
    0.0f, // color
    // vertex 2
    0.5f,
    -0.5f,
    0.0f,
    0.0f,
    1.0f,
    0.0f,
    // vertex 3
    0.0f,
    0.5f,
    0.0f,
    0.0f,
    0.0f,
    1.0f
};

unsigned int VAOs[3];
unsigned int VBOs[3];

Shader shaderPrograms[2];

std::function<void()> drawFunc;

void setupVAOs();
void drawColorChange();
void drawMultiColor();

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    {
        glfwSetWindowShouldClose(window, true);
    }
    if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS)
    {
        drawFunc = drawColorChange;
    }
    if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS)
    {
        drawFunc = drawMultiColor;
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

    GLFWwindow* window = glfwCreateWindow(800, 600, "Hello Window", NULL, NULL);
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
    glfwSetKeyCallback(window, key_callback);

    glClearColor(0.529f, 0.808f, 0.980f, 1.0f);

    setupVAOs();

    shaderPrograms[0].loadShaders("colorChange.vs", "colorChange.fs");
    shaderPrograms[1].loadShaders("multiColor.vs", "multiColor.fs");

    drawFunc = drawColorChange;

    while (!glfwWindowShouldClose(window))
    {
        glClear(GL_COLOR_BUFFER_BIT);

        drawFunc();

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteVertexArrays(3, VAOs);
    glDeleteBuffers(1, VBOs);
    glfwTerminate();

    return 0;
}

void drawColorChange()
{
    float timeValue  = glfwGetTime();
    float colorValue = (sin(timeValue) / 2.0f) + 0.5f;
    float redValue   = std::clamp(colorValue, 0.0f, 1.0f);
    float greenValue = std::clamp(colorValue, 0.0f, 0.388f);
    float blueValue  = std::clamp(colorValue, 0.0f, 0.278f);
    shaderPrograms[0].use();
    shaderPrograms[0].setFloat4("ourColor", redValue, greenValue, blueValue, 1.0f);
    glBindVertexArray(VAOs[0]);
    glDrawArrays(GL_TRIANGLES, 0, 3);
}

void drawMultiColor()
{
    shaderPrograms[1].use();
    glBindVertexArray(VAOs[1]);
    glDrawArrays(GL_TRIANGLES, 0, 3);
}

void setupVAOs()
{
    glGenVertexArrays(3, VAOs);
    glGenBuffers(1, VBOs);

    glBindVertexArray(VAOs[0]);

    glBindBuffer(GL_ARRAY_BUFFER, VBOs[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindVertexArray(VAOs[1]);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
}
