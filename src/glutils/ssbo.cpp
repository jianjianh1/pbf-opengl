#include "ssbo.h"

#include <utility>

SSBO::~SSBO()
{
    if (m_id)
    {
        glDeleteBuffers(1, &m_id);
    }
}

SSBO::SSBO(SSBO&& other) noexcept
    : m_id{ other.m_id }
{
    other.m_id = 0;
}

SSBO& SSBO::operator=(SSBO&& other) noexcept
{
    std::swap(m_id, other.m_id);
    return *this;
}

SSBO::SSBO(GLenum usage, GLsizeiptr size, const void* data)
{
    glGenBuffers(1, &m_id);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_id);
    glBufferData(GL_SHADER_STORAGE_BUFFER, size, data, usage);
}

void SSBO::bind(GLuint index) const
{
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_id);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, index, m_id);
}
