#include "renderer.h"

#include <glm/ext/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>

/// @brief Parameters for the renderer
namespace render_params
{
    constexpr int contextVersionMajor{ 4 };
    constexpr int contextVersionMinor{ 6 };

    constexpr int width{ 1920 };
    constexpr int height{ 1080 };
    const char* title{ "Fluid Simulation" };

    const glm::vec4 clearColor{ 1.0f, 1.0f, 1.0f, 1.0f };

    constexpr int fpsFrames{ 60 };

    constexpr float cameraDistance{ 4.0f };
    constexpr float cameraAngleY{ 60.0f };
    constexpr float cameraAngleX{ 45.0f };

    constexpr float lightDistance{ 3.0f };
    constexpr float lightAngleY{ 45.0f };
    constexpr float lightAngleX{ 45.0f };

    const glm::vec4 waterColor{ 0.0f, 0.0f, 1.0f, 1.0f };

    constexpr float fov{ 45.0f }; // in degree
    constexpr float near{ 0.1f };
    constexpr float far{ 1000.0f };
}

namespace shader_path
{
    const char* particleVert{ "shaders/particle.vert" };
    const char* particleFrag{ "shaders/particle.frag" };
}

GLFWwindow* Renderer::setupContext(int width, int height, const char* title)
{
    if (!glfwInit())
    {
        std::cerr << "Failed to initialize GLFW!";
        exit(EXIT_FAILURE);
    }

    // OpenGL version
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, render_params::contextVersionMajor);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, render_params::contextVersionMinor);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // create the window
    GLFWwindow* window{ glfwCreateWindow(width, height, title, nullptr, nullptr) };
    if (!window)
    {
        std::cerr << "Failed to create a window!";
        glfwTerminate();
        exit(EXIT_FAILURE);
    }
    glfwMakeContextCurrent(window);

    // load OpenGL
    if (!gladLoadGL())
    {
        std::cerr << "Failed to load OpenGL!\n";
        glfwTerminate();
        exit(EXIT_FAILURE);
    }

    return window;
}

void Renderer::keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    constexpr float moveAmount{ 0.05f };
    Renderer* renderer = static_cast<Renderer*>(glfwGetWindowUserPointer(window));
    if (key == GLFW_KEY_R && action == GLFW_PRESS)
    {
        renderer->m_fluid.reset();
    }
    if (key == GLFW_KEY_LEFT)
    {
        renderer->m_fluid.moveBoundaryX(-moveAmount);
    }
    if (key == GLFW_KEY_RIGHT)
    {
        renderer->m_fluid.moveBoundaryX(moveAmount);
    }
    if (key == GLFW_KEY_DOWN)
    {
        renderer->m_fluid.moveBoundaryZ(-moveAmount);
    }
    if (key == GLFW_KEY_UP)
    {
        renderer->m_fluid.moveBoundaryZ(moveAmount);
    }
}

Renderer::Renderer()
    : m_width{ render_params::width }
    , m_height{ render_params::height }
    , m_title{ render_params::title }
    , m_context{ setupContext(m_width, m_height, m_title.c_str()) }
    , m_background{ render_params::clearColor }
    , m_camera{ render_params::cameraDistance, render_params::cameraAngleY, render_params::cameraAngleX }
    , m_light{ render_params::lightDistance, render_params::lightAngleY, render_params::lightAngleX }
    , m_fluidShader{ shader_path::particleVert, shader_path::particleFrag }
{
    glfwSetWindowUserPointer(m_context, this); // GLFW callbacks can only be static functions
    glfwSetKeyCallback(m_context, keyCallback);
};

Renderer::~Renderer()
{
    glfwTerminate();
}

void Renderer::run()
{
    //glEnable(GL_DEPTH_TEST);
    glClearColor(m_background.r, m_background.g, m_background.b, m_background.a);
    m_frames = 0;
    m_timeLastFrame = glfwGetTime();
    while (!glfwWindowShouldClose(m_context))
    {
        glfwGetFramebufferSize(m_context, &m_width, &m_height);
        glViewport(0, 0, m_width, m_height);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        renderFluid();
        m_fluid.update();

        glfwSwapBuffers(m_context);
        glfwPollEvents();

        if (++m_frames == render_params::fpsFrames)
        {
            double timeThisFrame{ glfwGetTime() };
            std::cout << "FPS: " << render_params::fpsFrames / (timeThisFrame - m_timeLastFrame) << "\n";
            m_timeLastFrame = timeThisFrame;
            m_frames = 0;
        }
    }
}

void Renderer::renderFluid()
{
    glm::mat4 projection{
        glm::perspective(
            render_params::fov,
            static_cast<float>(m_width) / m_height,
            render_params::near,
            render_params::far
        )
    };

    m_fluidShader.setUniform("u_mvp", projection * m_camera.viewMatrix());
    m_fluidShader.setUniform("u_color", render_params::waterColor);
    m_fluid.draw(m_fluidShader);
}
