#include "cubemap.h"

#include <misc/image.h>

#include <iostream>
#include <utility>

Cubemap::~Cubemap()
{
    glDeleteTextures(1, &m_id);
}

Cubemap::Cubemap(Cubemap&& other) noexcept
    : m_id{ other.m_id }
{
    other.m_id = 0;
}

Cubemap& Cubemap::operator=(Cubemap&& other) noexcept
{
    std::swap(m_id, other.m_id);
    return *this;
}

Cubemap::Cubemap(
    const char* positiveX, const char* negativeX,
    const char* positiveY, const char* negativeY,
    const char* positiveZ, const char* negativeZ,
    bool flipVertical)
{
    glGenTextures(1, &m_id);
    glBindTexture(GL_TEXTURE_CUBE_MAP, m_id);

    const char* images[]{
        positiveX, negativeX, positiveY, negativeY, positiveZ, negativeZ
    };
    for (int i{ 0 }; i < 6; ++i)
    {
        std::cout << "Loading image " << images[i] << " ...\n";
        Image img{ images[i], flipVertical };
        if (!img.data())
        {
            std::cerr << "Failed to load image " << images[i] << "!\n\n";
            glDeleteTextures(1, &m_id);
            m_id = 0;
            return;
        }
        std::cout << "Image " << images[i] << " loaded\n\n";

        GLenum format{};
        switch (img.channels())
        {
        case 3:
            format = GL_RGB;
            break;
        case 4:
            format = GL_RGBA;
            break;
        default:
            std::cerr << "Unsupported format!\n\n";
            glDeleteTextures(1, &m_id);
            m_id = 0;
            return;
        }

        glTexImage2D(
            GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, format,
            img.width(), img.height(), 0, format, GL_UNSIGNED_BYTE, img.data()
        );
    }

    glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
    generateMipmap();
}

void Cubemap::bind(GLuint textureUnit) const
{
    glActiveTexture(GL_TEXTURE0 + textureUnit);
    glBindTexture(GL_TEXTURE_CUBE_MAP, m_id);
}

void Cubemap::generateMipmap() const
{
    bind();
    glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAX_ANISOTROPY, 8.0f);
}


