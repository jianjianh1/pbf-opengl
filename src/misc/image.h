#pragma once

/// @brief Wrapper class for reading image using stb
class Image
{
private:
    /// @brief Image dimensions
    int m_width{};
    int m_height{};
    int m_channels{};

    /// @brief Pixel data that's read in with stb
    unsigned char* m_data{};

public:
    /// @brief Read in a new image form the path and apply vertical flip if necessary
    /// @param path the image file path
    /// @param flipVertical true if image needs to be flipped
    Image(const char* path, bool flipVertical = false);

    /// @brief Copy constructor
    Image(const Image& other);

    /// @brief Copy assignment using copy-swap idiom
    Image& operator=(Image other);

    /// @brief Free the image memory
    ~Image();

    /// @brief Get the image width
    inline int width() const { return m_width; }

    /// @brief Get the image height
    inline int height() const { return m_height; }

    /// @brief Get the number of channels
    inline int channels() const { return m_channels; }

    /// @brief Get the image data pointer
    inline const unsigned char* data() const { return m_data; }
};
