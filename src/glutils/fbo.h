#pragma once

#include "texture.h"

/// @brief Wrapper for OpenGL's frame buffer object
class FBO
{
private:
    /// @brief ID of this FBO; nonzero if initialized
    GLuint m_id{};

    /// @brief ID of depth render buffer; created if no depth attachment
    GLuint m_depthRenderBuffer{};

    /// @brief Width of last bind texture
    int m_width{};

    /// @brief Height of last bind texture
    int m_height{};

    /// @brief Bind the render buffer as depth attachment
    void bindDepthRenderBuffer();

public:
    /// @brief Create a framebuffer
    FBO();

    /// @brief Delete the object on GPU
    ~FBO();

    /// @brief No copying
    FBO(const FBO& other) = delete;

    /// @brief No copying
    FBO& operator=(const FBO& other) = delete;

    /// @brief Bind the texture as depth attachment
    void bind(const Texture& texture);

    /// @brief Set depth output to a render buffer
    void disableDepthOutput();

    /// @brief Disable color output
    void disableColorOutput();

    /// @brief Activate framebuffer; will check availability
    void activate();

    /// @brief Change to default framebuffer
    inline void deactivate() const { glBindFramebuffer(GL_FRAMEBUFFER, 0); }
};