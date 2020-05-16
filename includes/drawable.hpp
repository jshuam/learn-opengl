#pragma once

#include <cstddef>

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>

#include "vertex.hpp"

class Drawable
{
  public:
    Drawable(GLfloat* vertices, GLuint numVertices)
    {
        m_vertices    = new Vertex[sizeof(GLfloat) * numVertices];
        m_numVertices = numVertices;

        glGenVertexArrays(1, &m_VAO);
        glGenBuffers(1, &m_VBO);

        glBindVertexArray(m_VAO);
        glBindBuffer(GL_ARRAY_BUFFER, m_VBO);

        glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * m_numVertices, m_vertices, GL_STATIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, position));

        glEnableVertexAttribArray(0);
    };

    void drawArrays()
    {
        glBindVertexArray(m_VAO);
        glDrawArrays(GL_LINES, 0, m_numVertices);
    }

    void drawElements()
    {}

  private:
    GLuint m_VBO;
    GLuint m_VAO;
    GLuint m_numVertices;
    Vertex* m_vertices;
};
