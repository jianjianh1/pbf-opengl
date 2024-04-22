#pragma once

#include "ssbo.h"

#include <glad/glad.h>

/// @brief Wrapper class for OpenGL's Vertex Array Object
class VAO
{
private:
    /// @brief ID of VAO, nonzero if initialized
    GLuint m_id{};

public:
    /// @brief Default constructor
    VAO();

    /// @brief Delete VAO on GPU
    ~VAO();

    /// @brief Move only
    VAO(const VAO& other) = delete;

    /// @brief Move only
    VAO& operator=(const VAO& other) = delete;

    /// @brief Move constructor
    VAO(VAO&& other) noexcept;

    /// @brief Move assignment
    VAO& operator=(VAO&& other) noexcept;

    /// @brief Activate this VAO
    inline void activate() const { glBindVertexArray(m_id); }

    /// @brief Set an attribute with a buffer
    /// @param buffer the buffer ID
    /// @param index the index of attribute
    /// @param size the size of attribute
    /// @param type the type of data
    /// @param stride the stride
    /// @param offset the offset
    /// @param normalized if the data is normalized
    void setAttrib(GLuint buffer, GLuint index, GLint size, GLenum type, GLsizei stride, size_t offset, GLboolean normalized = GL_FALSE) const;

    /// @brief Implicit conversion to unsigned int
    inline operator GLuint() const { return m_id; }
};