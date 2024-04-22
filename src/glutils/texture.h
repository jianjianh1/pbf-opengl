#pragma once

#include <glm/glm.hpp>
#include <glad/glad.h>

/// @brief Wrapper for OpenGL's texture 2D
class Texture
{
private:
    /// @brief width of texture
    int m_width{};

    /// @brief height of texture
    int m_height{};

    /// @brief texture format
    GLint m_format{};

    /// @brief ID of texture buffer, nonzero if initialized
    GLuint m_id{};

public:
    /// @brief Default constructor
    Texture() = default;

    /// @brief Delete texture on GPU
    ~Texture();

    /// @brief No copying
    Texture(const Texture& other) = delete;

    /// @brief No copying
    Texture& operator=(const Texture& other) = delete;

    /// @brief Move constructor
    Texture(Texture&& other) noexcept;

    /// @brief Move assignment
    Texture& operator=(Texture&& other) noexcept;

    /// @brief Create a texture with given size
    Texture(int width, int height, int format = GL_RGB);

    /// @brief Bind the given texture unit
    void bind(GLuint textureUnit = 0) const;

    /// @brief Set the wrapping method to clamping to edge
    void clampToEdge() const;

    /// @brief Set the wrapping method to clamp to border with the border color
    /// @param borderColor 
    void clampToBorder(const glm::vec4& borderColor = glm::vec4(1.0f)) const;

    /// @brief Generate mipmap for this texture
    void generateMipmap() const;

    /// @brief Return its ID when converted to unsigned int
    inline operator GLuint() const { return m_id; }

    /// @brief Return the width
    inline int width() const { return m_width; }

    /// @brief Return the height
    inline int height() const { return m_height; }

    /// @brief Return the format
    inline int format() const { return m_format; }
};