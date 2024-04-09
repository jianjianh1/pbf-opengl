#pragma once

#include "orbit_camera.h"
#include "orbit_light.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include <string>

/// @brief Renderer for this project
class Renderer
{
private:
    /// @brief The width of viewport
    int m_width;

    /// @brief The height of viewport
    int m_height;

    /// @brief The title of window
    std::string m_title;

    /// @brief The OpenGL context, a window
    GLFWwindow* m_context;

    /// @brief Background color (clear color)
    glm::vec4 m_background;

    /// @brief The main camera
    OrbitCamera m_camera;

    /// @brief Only one light
    OrbitLight m_light;

    /// @brief Initialize the window
    static GLFWwindow* setupContext(int width, int height, const char* title);

public:
    /// @brief Create a renderer with the initial size and title
    Renderer();

    /// @brief Close the renderer
    ~Renderer();

    /// @brief No copy
    Renderer(const Renderer& other) = delete;

    /// @brief No copy
    Renderer& operator=(const Renderer& other) = delete;

    /// @brief Render loop
    void run();
};