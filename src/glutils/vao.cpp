#include "vao.h"

#include <utility>

VAO::VAO()
{
    glGenVertexArrays(1, &m_id);
}

VAO::~VAO()
{
    glDeleteVertexArrays(1, &m_id);
}

VAO::VAO(VAO&& other) noexcept
    : m_id{ other.m_id }
{
    other.m_id = 0;
}

VAO& VAO::operator=(VAO&& other) noexcept
{
    std::swap(m_id, other.m_id);
    return *this;
}

void VAO::setAttrib(GLuint buffer, GLuint index, GLint size, GLenum type, GLsizei stride, size_t offset, GLboolean normalized) const
{
    activate();
    glBindBuffer(GL_ARRAY_BUFFER, buffer);
    glVertexAttribPointer(index, size, type, normalized, stride, reinterpret_cast<void*>(offset));
    glEnableVertexAttribArray(index);
}
