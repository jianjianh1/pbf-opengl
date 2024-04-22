#include "fbo.h"

#include <iostream>

void FBO::bindDepthRenderBuffer()
{
    if (!m_depthRenderBuffer)
    {
        glGenRenderbuffers(1, &m_depthRenderBuffer);
        glBindRenderbuffer(GL_RENDERBUFFER, m_depthRenderBuffer);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, m_width, m_height);
    }
    glBindFramebuffer(GL_FRAMEBUFFER, m_id);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_depthRenderBuffer);
}

FBO::FBO()
{
    glGenFramebuffers(1, &m_id);
}

FBO::~FBO()
{
    glDeleteFramebuffers(1, &m_id);
    glDeleteRenderbuffers(1, &m_depthRenderBuffer);
}

void FBO::bind(const Texture& texture)
{
    if (!texture)
    {
        std::cerr << "Texture is not initialized!\n";
        return;
    }
    glBindFramebuffer(GL_FRAMEBUFFER, m_id);

    GLenum attachment{};
    switch (texture.format())
    {
    case GL_RGB:
    case GL_RGBA:
        attachment = GL_COLOR_ATTACHMENT0;
        break;
    case GL_DEPTH_COMPONENT:
        attachment = GL_DEPTH_ATTACHMENT;
        break;
    }
    glFramebufferTexture2D(GL_FRAMEBUFFER, attachment, GL_TEXTURE_2D, texture, 0);
    m_width = texture.width();
    m_height = texture.height();
}

void FBO::disableDepthOutput()
{
    bindDepthRenderBuffer();
}

void FBO::disableColorOutput()
{
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
}

void FBO::activate()
{
    glBindFramebuffer(GL_FRAMEBUFFER, m_id);
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    {
        std::cerr << "Framebuffer is not complete!\n";
        return;
    }
    glViewport(0, 0, m_width, m_height);
}

