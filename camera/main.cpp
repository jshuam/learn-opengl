#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <algorithm>
#include <functional>
#include <iostream>

#include "shader.h"

float vertices[] = {
    // positions        // texture coords
    0.5f,  0.5f,  0.0f, 1.0f, 1.0f, // top right
    0.5f,  -0.5f, 0.0f, 1.0f, 0.0f, // bottom right
    -0.5f, -0.5f, 0.0f, 0.0f, 0.0f, // bottom left
    -0.5f, 0.5f,  0.0f, 0.0f, 1.0f  // top left
};

float cube[] = { -0.5f, -0.5f, -0.5f, 0.0f, 0.0f, 0.5f,  -0.5f, -0.5f, 1.0f, 0.0f, 0.5f,  0.5f,  -0.5f, 1.0f, 1.0f,
                 0.5f,  0.5f,  -0.5f, 1.0f, 1.0f, -0.5f, 0.5f,  -0.5f, 0.0f, 1.0f, -0.5f, -0.5f, -0.5f, 0.0f, 0.0f,

                 -0.5f, -0.5f, 0.5f,  0.0f, 0.0f, 0.5f,  -0.5f, 0.5f,  1.0f, 0.0f, 0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
                 0.5f,  0.5f,  0.5f,  1.0f, 1.0f, -0.5f, 0.5f,  0.5f,  0.0f, 1.0f, -0.5f, -0.5f, 0.5f,  0.0f, 0.0f,

                 -0.5f, 0.5f,  0.5f,  1.0f, 0.0f, -0.5f, 0.5f,  -0.5f, 1.0f, 1.0f, -0.5f, -0.5f, -0.5f, 0.0f, 1.0f,
                 -0.5f, -0.5f, -0.5f, 0.0f, 1.0f, -0.5f, -0.5f, 0.5f,  0.0f, 0.0f, -0.5f, 0.5f,  0.5f,  1.0f, 0.0f,

                 0.5f,  0.5f,  0.5f,  1.0f, 0.0f, 0.5f,  0.5f,  -0.5f, 1.0f, 1.0f, 0.5f,  -0.5f, -0.5f, 0.0f, 1.0f,
                 0.5f,  -0.5f, -0.5f, 0.0f, 1.0f, 0.5f,  -0.5f, 0.5f,  0.0f, 0.0f, 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

                 -0.5f, -0.5f, -0.5f, 0.0f, 1.0f, 0.5f,  -0.5f, -0.5f, 1.0f, 1.0f, 0.5f,  -0.5f, 0.5f,  1.0f, 0.0f,
                 0.5f,  -0.5f, 0.5f,  1.0f, 0.0f, -0.5f, -0.5f, 0.5f,  0.0f, 0.0f, -0.5f, -0.5f, -0.5f, 0.0f, 1.0f,

                 -0.5f, 0.5f,  -0.5f, 0.0f, 1.0f, 0.5f,  0.5f,  -0.5f, 1.0f, 1.0f, 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
                 0.5f,  0.5f,  0.5f,  1.0f, 0.0f, -0.5f, 0.5f,  0.5f,  0.0f, 0.0f, -0.5f, 0.5f,  -0.5f, 0.0f, 1.0f };

unsigned int indices[] = {
    0, 1, 3, // first triangle
    1, 2, 3  // second triangle
};

glm::vec3 cubePositions[] = { glm::vec3(0.0f, 0.0f, 0.0f),    glm::vec3(2.0f, 5.0f, -15.0f),
                              glm::vec3(-1.5f, -2.2f, -2.5f), glm::vec3(-3.8f, -2.0f, -12.3f),
                              glm::vec3(2.4f, -0.4f, -3.5f),  glm::vec3(-1.7f, 3.0f, -7.5f),
                              glm::vec3(1.3f, -2.0f, -2.5f),  glm::vec3(1.5f, 2.0f, -2.5f),
                              glm::vec3(1.5f, 0.2f, -1.5f),   glm::vec3(-1.3f, 1.0f, -1.5f) };

constexpr int windowWidth = 800;
constexpr int windowHeight = 600;

constexpr unsigned int numModels = 2;
constexpr unsigned int numShaders = 1;
constexpr unsigned int numTextures = 2;

unsigned int VAOs[numModels];
unsigned int VBOs[numModels];
unsigned int textures[numTextures];

Shader shaderPrograms[numShaders];

std::function<void()> drawFunc;

void setupVAOs();
void createTexture(const char* filename, unsigned int texture, GLint format, GLint sourceFormat);

void drawBox();
void drawCube();
void drawCubes();

glm::vec3 cameraPos(0.0f, 0.0f, 3.0f);
glm::vec3 cameraFront(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp(0.0f, 1.0f, 0.0f);

float deltaTime = 0.0f;
float lastFrame = 0.0f;
float elapsedTime = 0.0f;
int frameCount = 0;

float lastX = 400, lastY = 300;
float pitch = 0.0f;
float yaw = -90.0f;
bool firstMouse = true;
float fov = 44.9f;

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
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = ypos - lastY;

    lastX = xpos;
    lastY = ypos;

    const float sensitivity = 0.05f;
    xoffset *= sensitivity;
    yoffset *= sensitivity;

    yaw += xoffset;
    pitch -= yoffset;

    if (pitch > 89.0f)
        pitch = 89.0f;
    if (pitch < -89.0f)
        pitch = -89.0f;

    glm::vec3 direction;
    direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    direction.y = sin(glm::radians(pitch));
    direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    cameraFront = glm::normalize(direction);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    float newFov = fov -= yoffset;

    if (newFov < 1.0f)
    {
        newFov = 1.0f;
        fov = newFov;
        return;
    }
    if (newFov > 45.0f)
    {
        newFov = 45.0f;
        fov = newFov;
        return;
    }

    fov = newFov;
}

void processInput(GLFWwindow* window)
{
    const float cameraSpeed = 2.5f * deltaTime;
    const glm::vec3 cameraRight = glm::normalize(glm::cross(cameraFront, cameraUp));

    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
    {
        cameraPos.y += cameraSpeed;
    }

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
    {
        cameraPos += cameraSpeed * cameraFront;
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
    {
        cameraPos -= cameraSpeed * cameraFront;
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
    {
        cameraPos -= cameraRight * cameraSpeed;
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
    {
        cameraPos += cameraRight * cameraSpeed;
    }

    if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS)
    {
        drawFunc = drawBox;
        return;
    }
    if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS)
    {
        drawFunc = drawCube;
        return;
    }
    if (glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS)
    {
        drawFunc = drawCubes;
        return;
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

    GLFWwindow* window = glfwCreateWindow(windowWidth, windowHeight, "Camera", NULL, NULL);
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
    glfwSetScrollCallback(window, scroll_callback);
    glfwSetCursorPosCallback(window, mouse_callback);

    glClearColor(0.529f, 0.808f, 0.980f, 1.0f);

    glfwSetInputMode(window, GLFW_STICKY_KEYS, GLFW_TRUE);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    setupVAOs();

    glGenTextures(numTextures, textures);
    createTexture("container.jpg", textures[0], GL_RGB, GL_RGB);
    createTexture("awesomeface.png", textures[1], GL_RGBA, GL_RGBA);

    shaderPrograms[0].loadShaders("vert.vs", "frag.fs");

    shaderPrograms[0].use();
    shaderPrograms[0].setInt("texture1", 0);
    shaderPrograms[0].setInt("texture2", 1);

    drawFunc = drawBox;
    glEnable(GL_DEPTH_TEST);

    while (!glfwWindowShouldClose(window))
    {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        processInput(window);
        drawFunc();

        glm::mat4 view = glm::mat4(1.0f);
        view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);

        glm::mat4 projection;
        projection = glm::perspective(glm::radians(fov), 800.0f / 600.0f, 0.1f, 100.0f);

        unsigned int viewLoc = glGetUniformLocation(shaderPrograms[0].id, "view");
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));

        unsigned int projLoc = glGetUniformLocation(shaderPrograms[0].id, "projection");
        glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

        glfwSwapBuffers(window);
        glfwPollEvents();

        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        elapsedTime += deltaTime;
        lastFrame = currentFrame;
        frameCount++;

        if (elapsedTime >= 1.0f)
        {
            glfwSetWindowTitle(window, std::string("Camera - " + std::to_string(frameCount)).c_str());
            frameCount = 0;
            elapsedTime = 0.0f;
        }
    }

    glDeleteVertexArrays(3, VAOs);
    glDeleteBuffers(1, VBOs);
    glfwTerminate();

    return 0;
}

void drawBox()
{
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, textures[0]);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, textures[1]);

    shaderPrograms[0].use();

    glm::mat4 model = glm::mat4(1.0f);
    model = glm::rotate(model, glm::radians(-55.0f), glm::vec3(1.0f, 0.0f, 0.0f));

    unsigned int modelLoc = glGetUniformLocation(shaderPrograms[0].id, "model");
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

    glBindVertexArray(VAOs[0]);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}

void drawCube()
{
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, textures[0]);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, textures[1]);

    shaderPrograms[0].use();

    glm::mat4 model = glm::mat4(1.0f);
    model = glm::rotate(model, (float)glfwGetTime() * glm::radians(50.0f), glm::vec3(0.5f, 1.0f, 0.0f));

    unsigned int modelLoc = glGetUniformLocation(shaderPrograms[0].id, "model");
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

    glBindVertexArray(VAOs[1]);
    glDrawArrays(GL_TRIANGLES, 0, 36);
}

void drawCubes()
{
    glBindVertexArray(VAOs[1]);
    shaderPrograms[0].use();

    for (int i = 0; i < 10; ++i)
    {
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, cubePositions[i]);
        float angle = 20.0f * i;

        if (i % 3 == 0)
        {
            angle *= glfwGetTime();
        }

        model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
        shaderPrograms[0].setMat4("model", model);
        glDrawArrays(GL_TRIANGLES, 0, 36);
    }
}

void setupVAOs()
{
    unsigned int IBO;

    glGenVertexArrays(numModels, VAOs);
    glGenBuffers(numModels, VBOs);
    glGenBuffers(1, &IBO);

    glBindVertexArray(VAOs[0]);

    glBindBuffer(GL_ARRAY_BUFFER, VBOs[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindVertexArray(VAOs[1]);

    glBindBuffer(GL_ARRAY_BUFFER, VBOs[1]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cube), cube, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
}

void createTexture(const char* filename, unsigned int texture, GLint format, GLint sourceFormat)
{
    glBindTexture(GL_TEXTURE_2D, texture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    int width, height, numChannels;
    stbi_set_flip_vertically_on_load_thread(true);
    unsigned char* data = stbi_load(filename, &width, &height, &numChannels, 0);

    if (data)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, sourceFormat, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    stbi_image_free(data);
}
