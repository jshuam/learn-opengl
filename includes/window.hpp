#pragma once

#if __unix__
#include <GL/glew.h>
#elif _WIN32
#include <GLEW/glew.h>
#endif

#include <GLFW/glfw3.h>

class Window
{
  public:
    Window(const char* title, float width = 800, float height = 600)
        : aspectRatio(width / height),
          m_window(glfwCreateWindow(width, height, title, NULL, NULL)),
          m_width(width),
          m_height(height)
    {}

    GLFWwindow* get()
    {
        return m_window;
    }

    void setResolution(float width, float height)
    {
        m_width  = width;
        m_height = height;
    }

  public:
    float aspectRatio;

  private:
    GLFWwindow* m_window;

    float m_width;
    float m_height;
};
