#include "fullscreen_quad.h"

FullscreenQuad::FullscreenQuad()
    : m_VAO{}
{
    // counter-clock wise
    float vertices[]{
        -1.0f, -1.0f,
        1.0f, -1.0f,
        1.0f, 1.0f,
        -1.0f, 1.0f
    };

    glGenBuffers(1, &m_VBO);
    glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    m_VAO.setAttrib(m_VBO, 0, 2, GL_FLOAT, 2 * sizeof(float), 0);
}

FullscreenQuad::~FullscreenQuad()
{
    glDeleteBuffers(1, &m_VBO);
}

void FullscreenQuad::draw(const ShaderProgram& program)
{
    m_VAO.activate();
    program.activate();
    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
}
