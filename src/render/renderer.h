#pragma once

#include "orbit_camera.h"
#include "orbit_light.h"
#include "fullscreen_quad.h"
#include <simulation/fluid_system.h>
#include <glutils/shader_program.h>
#include <glutils/fbo.h>
#include <glutils/texture.h>
#include <glutils/cubemap.h>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include <string>

/// @brief Renderer for this project
class Renderer
{
private:
    /// @brief The width of viewport
    int m_width{};

    /// @brief The height of viewport
    int m_height{};

    /// @brief The title of window
    std::string m_title{};

    /// @brief The OpenGL context, a window
    GLFWwindow* m_context{};

    /// @brief The time when the last frame starts
    double m_timeLastFrame{};

    /// @brief Frames for printing FPS
    int m_frames{};

    /// @brief The frame rate
    float m_fps{ 60.0f };

    /// @brief Last mouse position for mouse callback
    double m_mouseLastX{};
    double m_mouseLastY{};

    /// @brief Projection matrix
    glm::mat4 m_projMatrix{};

    /// @brief The main camera
    OrbitCamera m_camera{};

    /// @brief Only one light
    OrbitLight m_light{};

    /// @brief The fluid render
    FluidSystem m_fluid{};

    /// @brief Fullscreen quad for rendering texture
    FullscreenQuad m_screenQuad{};

    /// @brief The skybox
    Cubemap m_skybox{};

    /// @brief Texture that holds depth
    Texture m_depthTexture{};

    /// @brief Texture that holds normal map
    Texture m_normalTexture{};

    /// @brief Texture that holds intermediate result of smooth normal
    Texture m_smoothNormalTexture{};

    /// @brief Texture that holds thickness
    Texture m_thicknessTexture{};

    /// @brief Texture that shows background; should be the same size as window
    Texture m_backgroundTexture{};

    /// @brief FBO for rendering depth and normal to texture
    FBO m_depthNormalFBO{};

    /// @brief FBO for rendering thickness to texture
    FBO m_thicknessFBO{};

    /// @brief FBO for rendering background
    FBO m_backgroundFBO{};

    /// @brief Shader for rendering the final scene
    ShaderProgram m_finalShader{};

    /// @brief Shader for rendering the depth and normal
    ShaderProgram m_depthNormalShader{};

    /// @brief Shader for rendering the thickness
    ShaderProgram m_thicknessShader{};

    /// @brief Shader for smoothing normal (gaussian smooth)
    ShaderProgram m_smoothShader{};

    /// @brief Shader for rendering background
    ShaderProgram m_backgroundShader{};

    /// @brief Initialize the window
    static GLFWwindow* setupContext(int width, int height, const char* title);

    /// @brief Callback when some specific keys are pressed
    static void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);

    /// @brief Callback for mouse event
    static void mouseCallback(GLFWwindow* window, double xpos, double ypos);

    /// @brief Render final image
    void renderFinal();

    /// @brief Render depth and normal to texture
    void renderDepthNormal();

    /// @brief Render thickness to texture
    void renderThickness();

    /// @brief Smooth normal
    void smoothNormal();

    /// @brief Render the background behind fluid
    void renderBackground();

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
};