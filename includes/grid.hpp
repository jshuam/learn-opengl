#pragma once

#include <cstddef>

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>

#include "vertex.hpp"

class Grid
{
  public:
    Grid(int numLines, float lineGap, glm::vec3 color)
        : m_numLines(numLines),
          m_numVertices(numLines * 4),
          m_lineGap(lineGap),
          m_lineLen((numLines - 1.0f) * lineGap),
          m_vertices(new Vertex[m_numVertices]),
          m_color(color)
    {

        for (int i = 0; i < m_numLines; ++i)
        {
            int j = i * 2;

            m_vertices[j].position     = glm::vec3(0.0f, 0.0f, m_lineGap * i);
            m_vertices[j + 1].position = glm::vec3(m_lineLen, 0.0f, m_lineGap * i);

            m_vertices[j].color     = m_color;
            m_vertices[j + 1].color = m_color;
        }

        for (int i = 0; i < m_numLines; ++i)
        {
            int j = (i + m_numLines) * 2;

            m_vertices[j].position     = glm::vec3(m_lineGap * i, 0.0f, 0.0f);
            m_vertices[j + 1].position = glm::vec3(m_lineGap * i, 0.0f, m_lineLen);

            m_vertices[j].color     = m_color;
            m_vertices[j + 1].color = m_color;
        }

        glGenVertexArrays(1, &m_VAO);
        glGenBuffers(1, &m_VBO);

        glBindVertexArray(m_VAO);
        glBindBuffer(GL_ARRAY_BUFFER, m_VBO);

        glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * m_numVertices, m_vertices, GL_STATIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, position));
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, color));

        glEnableVertexAttribArray(0);
        glEnableVertexAttribArray(1);
    };

    glm::mat4 getModelMat()
    {
        glm::mat4 model(1.0f);
        glm::vec3 pos = glm::vec3(-m_lineLen / 2.0f, 0.0f, -m_lineLen / 2);
        model         = glm::translate(pos);

        return model;
    }

    void draw()
    {
        glBindVertexArray(m_VAO);
        glDrawArrays(GL_LINES, 0, m_numVertices);
    }

  private:
    int m_numLines;
    int m_numVertices;
    float m_lineGap;
    float m_lineLen;
    GLuint m_VBO;
    GLuint m_VAO;
    Vertex* m_vertices;
    glm::vec3 m_color;
};
