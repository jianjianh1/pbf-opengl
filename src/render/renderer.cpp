#include "renderer.h"

#include <iostream>

GLFWwindow* Renderer::setupContext(int width, int height, const char* title)
{
    if (!glfwInit())
    {
        std::cerr << "Failed to initialize GLFW!";
        exit(EXIT_FAILURE);
    }

    // OpenGL version
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, renderparams::contextVersionMajor);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, renderparams::contextVersionMinor);
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

Renderer::Renderer(int width, int height, const char* title)
    : m_width{ width }
    , m_height{ height }
    , m_title{ title }
    , m_context{ setupContext(width, height, title) }
{
    glfwSetWindowUserPointer(m_context, this); // GLFW callbacks can only be static functions
}

Renderer::~Renderer()
{
    glfwTerminate();
}
