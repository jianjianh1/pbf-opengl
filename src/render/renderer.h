#pragma once

#include "orbit_camera.h"
#include "orbit_light.h"
#include <simulation/fluid_system.h>
#include <glutils/shader_program.h>

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

    /// @brief The time when the last frame starts
    double m_timeLastFrame;

    /// @brief Frames for printing FPS
    int m_frames;

    /// @brief Background color (clear color)
    glm::vec4 m_background;

    /// @brief The main camera
    OrbitCamera m_camera;

    /// @brief Only one light
    OrbitLight m_light;

    /// @brief Shader for rendering fluid
    ShaderProgram m_fluidShader;

    /// @brief The fluid render
    FluidSystem m_fluid;

    /// @brief Initialize the window
    static GLFWwindow* setupContext(int width, int height, const char* title);

    /// @brief Callback when some specific keys are pressed
    static void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);

public:
    /// @brief Create a renderer
    Renderer();

    /// @brief Close the renderer
    ~Renderer();

    /// @brief No copy
    Renderer(const Renderer& other) = delete;

    /// @brief No copy
    Renderer& operator=(const Renderer& other) = delete;

    /// @brief Render loop
    void run();

    /// @brief Render fluid
    void renderFluid();
};