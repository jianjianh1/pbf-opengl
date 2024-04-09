#pragma once

#include <glad/glad.h>

/// @brief Wrapper class for OpenGL's Shader Storage Buffer Object (SSBO)
class SSBO
{
private:
    /// @brief ID of SSBO, nonzero if initialized correctly
    GLuint m_id{};

public:
    /// @brief Default constructor
    SSBO() = default;

    /// @brief Delete buffer on GPU
    ~SSBO();

    /// @brief No copying
    SSBO(const SSBO& other) = delete;

    /// @brief No copying
    SSBO& operator=(const SSBO& other) = delete;

    /// @brief Move constructor
    SSBO(SSBO&& other) noexcept;

    /// @brief Move assignment
    SSBO& operator=(SSBO&& other) noexcept;

    /// @brief Create a SSBO specifying the parameters
    SSBO(GLenum usage, GLsizeiptr size, const void* data = nullptr);

    /// @brief Let this buffer bind to the given index
    void bind(GLuint index) const;

    /// @brief Return its ID when converted to unsigned int
    inline operator GLuint() const { return m_id; }

    /// @brief Swap the two SSBO
    static void swap(SSBO& ssbo1, SSBO& ssbo2);
};