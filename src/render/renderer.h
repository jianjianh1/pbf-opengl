#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <string>

/// @brief Parameters for the renderer
namespace renderparams
{
    constexpr int contextVersionMajor{ 4 };
    constexpr int contextVersionMinor{ 6 };
}

/// @brief Renderer for this project
class Renderer
{
private:
    int m_width;
    int m_height;
    std::string m_title;
    GLFWwindow* m_context;

    static GLFWwindow* setupContext(int width, int height, const char* title);

public:
    /// @brief Create a renderer with the initial size and title
    Renderer(int width, int height, const char* title);

    /// @brief Close the renderer
    ~Renderer();

    /// @brief No copy
    Renderer(const Renderer& other) = delete;

    /// @brief No copy
    Renderer& operator=(const Renderer& other) = delete;
};