#pragma once

#include <glad/glad.h>

/// @brief Wrapper class for OpenGL's cubemap texture
class Cubemap
{
private:
    /// @brief ID of this cubemap texture; nonzero if initialized
    GLuint m_id{};

public:
    /// @brief Default constructor; cubemap not usable
    Cubemap() = default;

    /// @brief Free cubemap memory on GPU
    ~Cubemap();

    /// @brief No copying
    Cubemap(const Cubemap& other) = delete;

    /// @brief No copying
    Cubemap& operator=(const Cubemap& other) = delete;

    /// @brief Move constructor
    Cubemap(Cubemap&& other) noexcept;

    /// @brief Move assignment
    Cubemap& operator=(Cubemap&& other) noexcept;

    /// @brief Create a cubemap from six images
    /// @param flipVertical true if images need to be flipped vertically
    Cubemap(
        const char* positiveX, const char* negativeX,
        const char* positiveY, const char* negativeY,
        const char* positiveZ, const char* negativeZ,
        bool flipVertical = false
    );

    /// @brief Bind to a texture unit
    /// @param textureUnit texture unit index
    void bind(GLuint textureUnit = 0) const;

    /// @brief Generate mipmap for this texture
    void generateMipmap() const;

    /// @brief Return its ID when converted to unsigned int
    inline operator GLuint() const { return m_id; }
};