#include "renderer.h"

#include <misc/parameters.h>

#include <iostream>

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

Renderer::Renderer()
    : m_width{ render_params::width }
    , m_height{ render_params::height }
    , m_title{ render_params::title}
    , m_context{ setupContext(m_width, m_height, m_title.c_str()) }
    , m_background{ render_params::clearColor }
    , m_camera{ render_params::cameraDistance, render_params::cameraAngleY, render_params::cameraAngleX }
    , m_light{ render_params::lightDistance, render_params::lightAngleY, render_params::lightAngleX }
{
    glfwSetWindowUserPointer(m_context, this); // GLFW callbacks can only be static functions
};

Renderer::~Renderer()
{
    glfwTerminate();
}

void Renderer::run()
{
    glEnable(GL_DEPTH_TEST);
    glClearColor(m_background.r, m_background.g, m_background.b, m_background.a);
    while (!glfwWindowShouldClose(m_context))
    {
        glfwGetFramebufferSize(m_context, &m_width, &m_height);
        glViewport(0, 0, m_width, m_height);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glfwSwapBuffers(m_context);
        glfwPollEvents();
    }
}
