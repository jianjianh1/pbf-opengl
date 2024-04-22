#include "image.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>

#include <utility>

Image::Image(const char* path, bool flipVertical)
{
    stbi_set_flip_vertically_on_load(flipVertical);
    m_data = stbi_load(path, &m_width, &m_height, &m_channels, 0);
}

Image::Image(const Image &other)
    : m_width{ other.m_width }
    , m_height{ other.m_height }
    , m_channels{ other.m_channels }
{
    stbi__copyval(m_channels, m_data, other.m_data);
}

Image &Image::operator=(Image other)
{
    std::swap(m_width, other.m_width);
    std::swap(m_height, other.m_height);
    std::swap(m_channels, other.m_channels);
    std::swap(m_data, other.m_data);

    return *this;
}

Image::~Image()
{
    stbi_image_free(m_data);
}
