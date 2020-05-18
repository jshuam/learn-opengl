#pragma once

#include <glm/ext/matrix_transform.hpp>
#if __unix__
#include <GL/glew.h>
#elif _WIN32
#include <GLEW/glew.h>
#endif

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

class Camera
{
  public:
    float m_speed;
    glm::vec3 m_cameraPos;

  private:
    glm::vec3 m_cameraFront;
    glm::vec3 m_cameraUp;

    float m_near;
    float m_far;
    float m_lastX;
    float m_lastY;
    float m_pitch;
    float m_yaw;
    float m_fov;
    float m_maxFov;
    bool m_firstMouse = true;

  public:
    Camera(glm::vec3 cameraPos   = glm::vec3(0.0f, 0.0f, 3.0f),
           glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f),
           glm::vec3 cameraUp    = glm::vec3(0.0f, 1.0f, 0.0f),
           float speed           = 7.5f,
           float near            = 0.1f,
           float far             = 100.0f,
           float pitch           = 0.0f,
           float yaw             = -90.0f,
           float fov             = 45.0f,
           float maxFov          = 45.0f)
        : m_speed(speed),
          m_cameraPos(cameraPos),
          m_cameraFront(cameraFront),
          m_cameraUp(cameraUp),
          m_near(near),
          m_far(far),
          m_pitch(pitch),
          m_yaw(yaw),
          m_fov(fov),
          m_maxFov(maxFov) {};

    void rotate(float xpos, float ypos)
    {
        if (m_firstMouse)
        {
            m_lastX      = xpos;
            m_lastY      = ypos;
            m_firstMouse = false;
        }

        float xoffset = xpos - m_lastX;
        float yoffset = ypos - m_lastY;

        m_lastX = xpos;
        m_lastY = ypos;

        const float sensitivity = 0.05f;
        xoffset *= sensitivity;
        yoffset *= sensitivity;

        m_yaw += xoffset;
        m_pitch -= yoffset;

        if (m_pitch > 89.0f)
            m_pitch = 89.0f;
        if (m_pitch < -89.0f)
            m_pitch = -89.0f;

        glm::vec3 direction;
        direction.x   = cos(glm::radians(m_yaw)) * cos(glm::radians(m_pitch));
        direction.y   = sin(glm::radians(m_pitch));
        direction.z   = sin(glm::radians(m_yaw)) * cos(glm::radians(m_pitch));
        m_cameraFront = glm::normalize(direction);
    };

    void move(GLFWwindow* window, float deltaTime)
    {
        float finalSpeed      = m_speed * deltaTime;
        glm::vec3 cameraRight = glm::normalize(glm::cross(m_cameraFront, m_cameraUp));

        if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
        {
            finalSpeed = (m_speed * 2) * deltaTime;
        }
        if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
        {
            m_cameraPos.y += finalSpeed;
        }
        if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
        {
            m_cameraPos.y -= finalSpeed;
        }
        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        {
            m_cameraPos += finalSpeed * m_cameraFront;
        }
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        {
            m_cameraPos -= finalSpeed * m_cameraFront;
        }
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        {
            m_cameraPos -= cameraRight * finalSpeed;
        }
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        {
            m_cameraPos += cameraRight * finalSpeed;
        }
    };

    void zoom(float yoffset)
    {
        float newFov = m_fov -= yoffset;

        if (newFov < 1.0f)
        {
            m_fov = 1.0f;
            return;
        }
        if (newFov > m_maxFov)
        {
            m_fov = m_maxFov;
            return;
        }

        m_fov = newFov;
    }

    glm::mat4 view()
    {
        return glm::lookAt(m_cameraPos, m_cameraPos + m_cameraFront, m_cameraUp);
    }

    glm::mat4 projection(const float& aspectRatio)
    {
        return glm::perspective(glm::radians(m_fov), aspectRatio, m_near, m_far);
    }
};
