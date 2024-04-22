#pragma once

#include <glad/glad.h>
#include <glutils/shader_program.h>
#include <glutils/vao.h>

/// @brief A fullscreen quad for rendering with textures
class FullscreenQuad
{
private:
    /// @brief The VAO for drawing
    VAO m_VAO{};

    /// @brief The ID of associated VBO
    GLuint m_VBO{};

public:
    /// @brief Allocate a fullscreen quad on GPU
    FullscreenQuad();

    /// @brief Destroy the VBO on GPU
    ~FullscreenQuad();

    /// @brief Draw the quad with the program
    void draw(const ShaderProgram& program);
};