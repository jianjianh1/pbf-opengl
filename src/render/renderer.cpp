#include "renderer.h"

#include <glm/ext/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>

/// @brief Parameters for the renderer
namespace render_params
{
    constexpr int contextVersionMajor{ 4 };
    constexpr int contextVersionMinor{ 6 };

    constexpr int width{ 1600 };
    constexpr int height{ 900 };
    const char* title{ "Fluid Simulation" };

    constexpr int fpsFrames{ 60 }; // compute fps every this frames

    constexpr float cameraDistance{ 3.0f };
    constexpr float cameraAngleY{ 60.0f };
    constexpr float cameraAngleX{ 60.0f };

    constexpr float lightDistance{ 4.0f };
    constexpr float lightAngleY{ 10.0f };
    constexpr float lightAngleX{ -90.0f };

    constexpr float fov{ 45.0f }; // in degree
    constexpr float near{ 0.1f };
    constexpr float far{ 100.0f };

    constexpr float pointSize{ 0.3f };
    constexpr float particleRadius{ 0.005f };

    constexpr int renderTextureWidth{ 64 * 16 };
    constexpr int renderTextureHeight{ 64 * 10 };

    constexpr int backgroundWidth{ 1920 };
    constexpr int backgroundHeight{ 1080 };

    const glm::vec3 fluidColor{ 0.137f, 0.537f, 1.0f };
    constexpr float fluidSpecular{ 0.5f };
    constexpr float fluidShininess{ 100.0f };
    constexpr float fluidReflectance{ 0.1f };
    constexpr float fluidRefractance{ 0.1f };
    constexpr float fluidAttenuation{ 1.0f };
}

namespace shader_path
{
    const char* particleVert{ "shaders/particle.vert" };
    const char* normalFrag{ "shaders/normal.frag" };
    const char* thicknessFrag{ "shaders/thickness.frag" };
    const char* quadVert{ "shaders/fullscreen_quad.vert" };
    const char* quadFrag{ "shaders/fullscreen_texture.frag" };
    const char* gaussianFrag{ "shaders/gaussian.frag" };
    const char* backgroundFrag{ "shaders/background.frag" };
    const char* finalFrag{ "shaders/final.frag" };
}

namespace texture_path
{
    const char* skyboxPosX{ "assets/cubemap_posx.png" };
    const char* skyboxNegX{ "assets/cubemap_negx.png" };
    const char* skyboxPosY{ "assets/cubemap_posy.png" };
    const char* skyboxNegY{ "assets/cubemap_negy.png" };
    const char* skyboxPosZ{ "assets/cubemap_posz.png" };
    const char* skyboxNegZ{ "assets/cubemap_negz.png" };
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
    Renderer* renderer = static_cast<Renderer*>(glfwGetWindowUserPointer(window));
    float moveAmount{ 0.03f * renderer->m_fps / 60.0f };
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
        renderer->m_fluid.moveBoundaryZ(moveAmount);
    }
    if (key == GLFW_KEY_UP)
    {
        renderer->m_fluid.moveBoundaryZ(-moveAmount);
    }
}

void Renderer::mouseCallback(GLFWwindow* window, double xpos, double ypos)
{
    constexpr float cameraSensitivity{ 0.1f };
    constexpr float distSensitivity{ 0.05f };
    Renderer* renderer = static_cast<Renderer*>(glfwGetWindowUserPointer(window));

    float xoffset{ static_cast<float>(xpos - renderer->m_mouseLastX) };
    float yoffset{ static_cast<float>(ypos - renderer->m_mouseLastY) };
    renderer->m_mouseLastX = xpos;
    renderer->m_mouseLastY = ypos;

    if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS
        || glfwGetKey(window, GLFW_KEY_RIGHT_CONTROL) == GLFW_PRESS)
    {
        if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
        {
            renderer->m_light.moveHorizontal(-xoffset);
            renderer->m_light.moveVertical(yoffset);
        }
    }
    else
    {

        if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
        {
            renderer->m_camera.moveHorizontal(cameraSensitivity * xoffset);
            renderer->m_camera.moveVertical(-cameraSensitivity * yoffset);
        }
        else if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS)
        {
            renderer->m_camera.moveDistance(-distSensitivity * yoffset);
        }
    }
}

void Renderer::renderFinal()
{
    glViewport(0, 0, m_width, m_height);
    glClearColor(1.0f, 0.0f, 0.0f, 1.0f);
    glEnable(GL_DEPTH_TEST);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    m_depthTexture.bind(0);
    m_finalShader.setUniform("u_depthMap", 0);
    m_normalTexture.bind(1);
    m_finalShader.setUniform("u_normalMap", 1);
    m_thicknessTexture.bind(2);
    m_finalShader.setUniform("u_thicknessMap", 2);
    m_backgroundTexture.bind(3);
    m_finalShader.setUniform("u_background", 3);
    m_skybox.bind(4);
    m_finalShader.setUniform("u_skybox", 4);

    m_finalShader.setUniform("u_projInv", glm::inverse(m_projMatrix));
    m_finalShader.setUniform("u_viewInv", glm::transpose(glm::mat3(m_camera.viewMatrix())));

    m_finalShader.setUniform("u_fluid.color", render_params::fluidColor);
    m_finalShader.setUniform("u_fluid.specular", render_params::fluidSpecular);
    m_finalShader.setUniform("u_fluid.shininess", render_params::fluidShininess);
    m_finalShader.setUniform("u_fluid.reflectance", render_params::fluidReflectance);
    m_finalShader.setUniform("u_fluid.refractance", render_params::fluidRefractance);
    m_finalShader.setUniform("u_fluid.attenuation", render_params::fluidAttenuation);

    glm::vec4 lightPosView{ m_camera.viewMatrix() * glm::vec4(m_light.position(), 1.0) };
    m_finalShader.setUniform("u_light.position", glm::vec3(lightPosView));
    //m_finalShader.setUniform("u_light.position", m_light.position());
    m_finalShader.setUniform("u_light.specular", m_light.specular());

    m_screenQuad.draw(m_finalShader);
}

void Renderer::renderDepthNormal()
{
    m_depthNormalFBO.bind(m_depthTexture);
    m_depthNormalFBO.bind(m_normalTexture);
    m_depthNormalFBO.activate();

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

    glEnable(GL_DEPTH_TEST);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_POINT_SPRITE);
    glTexEnvi(GL_POINT_SPRITE, GL_COORD_REPLACE, GL_TRUE);
    glPointParameteri(GL_POINT_SPRITE_COORD_ORIGIN, GL_LOWER_LEFT);
    glEnable(GL_VERTEX_PROGRAM_POINT_SIZE);

    m_depthNormalShader.setUniform("u_mvp", m_projMatrix * m_camera.viewMatrix());
    m_depthNormalShader.setUniform("u_mv", m_camera.viewMatrix());
    m_depthNormalShader.setUniform("u_pointSize", render_params::pointSize);
    m_depthNormalShader.setUniform("u_proj", m_projMatrix);
    m_depthNormalShader.setUniform("u_radius", render_params::particleRadius);
    m_depthNormalShader.setUniform("u_near", render_params::near);
    m_depthNormalShader.setUniform("u_far", render_params::far);
    m_fluid.draw(m_depthNormalShader);

    m_depthNormalFBO.deactivate();
}

void Renderer::renderThickness()
{
    m_thicknessFBO.bind(m_thicknessTexture);
    m_thicknessFBO.disableDepthOutput();
    m_thicknessFBO.activate();

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

    glDisable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_ONE, GL_ONE);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_POINT_SPRITE);
    glTexEnvi(GL_POINT_SPRITE, GL_COORD_REPLACE, GL_TRUE);
    glPointParameteri(GL_POINT_SPRITE_COORD_ORIGIN, GL_LOWER_LEFT);
    glEnable(GL_VERTEX_PROGRAM_POINT_SIZE);

    m_thicknessShader.setUniform("u_mvp", m_projMatrix * m_camera.viewMatrix());
    m_thicknessShader.setUniform("u_mv", m_camera.viewMatrix());
    m_thicknessShader.setUniform("u_pointSize", render_params::pointSize);
    m_thicknessShader.setUniform("u_proj", m_projMatrix);
    m_thicknessShader.setUniform("u_radius", render_params::particleRadius);
    m_thicknessShader.setUniform("u_near", render_params::near);
    m_thicknessShader.setUniform("u_far", render_params::far);

    m_fluid.draw(m_thicknessShader);

    glDisable(GL_BLEND);
    m_thicknessFBO.deactivate();
}

void Renderer::smoothNormal()
{
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glEnable(GL_DEPTH_TEST);

    // first pass smooth horizontal
    m_depthNormalFBO.bind(m_smoothNormalTexture);
    m_depthNormalFBO.disableDepthOutput();
    m_depthNormalFBO.activate();

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    m_normalTexture.bind(0);
    m_smoothShader.setUniform("u_texture", 0);
    m_smoothShader.setUniform("u_direction", glm::vec2{ 1.0f / render_params::renderTextureWidth, 0.0f });
    m_screenQuad.draw(m_smoothShader);

    // second path smooth vertical
    m_depthNormalFBO.bind(m_normalTexture);
    m_depthNormalFBO.disableDepthOutput();
    m_depthNormalFBO.activate();

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    m_smoothNormalTexture.bind(0);
    m_smoothShader.setUniform("u_texture", 0);
    m_smoothShader.setUniform("u_direction", glm::vec2{ 0.0f, 1.0f / render_params::renderTextureHeight });
    m_screenQuad.draw(m_smoothShader);

    m_depthNormalFBO.deactivate();
}

void Renderer::renderBackground()
{
    m_backgroundFBO.bind(m_backgroundTexture);
    m_backgroundFBO.disableDepthOutput();
    m_backgroundFBO.activate();

    glClearColor(1.0f, 0.0f, 0.0f, 1.0f);
    glDisable(GL_DEPTH_TEST);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    m_skybox.bind(0);
    glm::mat4 vpInv{ glm::inverse(m_projMatrix * m_camera.viewMatrix()) };
    m_backgroundShader.setUniform("u_skybox", 0);
    m_backgroundShader.setUniform("u_vpInv", vpInv);
    m_screenQuad.draw(m_backgroundShader);

    m_backgroundFBO.deactivate();
}

Renderer::Renderer()
    : m_width{ render_params::width }
    , m_height{ render_params::height }
    , m_title{ render_params::title }
    , m_context{ setupContext(m_width, m_height, m_title.c_str()) }
    , m_camera{ render_params::cameraDistance, render_params::cameraAngleY, render_params::cameraAngleX }
    , m_light{ render_params::lightDistance, render_params::lightAngleY, render_params::lightAngleX }
    , m_skybox{ texture_path::skyboxPosX, texture_path::skyboxNegX, texture_path::skyboxPosY, texture_path::skyboxNegY, texture_path::skyboxPosZ, texture_path::skyboxNegZ }
    , m_depthTexture{ render_params::renderTextureWidth, render_params::renderTextureHeight, GL_DEPTH_COMPONENT }
    , m_normalTexture{ render_params::renderTextureWidth, render_params::renderTextureHeight, GL_RGB }
    , m_smoothNormalTexture{ render_params::renderTextureWidth, render_params::renderTextureHeight, GL_RGB }
    , m_thicknessTexture{ render_params::renderTextureWidth, render_params::renderTextureHeight, GL_RGB }
    , m_backgroundTexture{ render_params::backgroundWidth, render_params::backgroundHeight, GL_RGB }
    , m_finalShader{ shader_path::quadVert, shader_path::finalFrag }
    , m_depthNormalShader{ shader_path::particleVert, shader_path::normalFrag }
    , m_thicknessShader{ shader_path::particleVert, shader_path::thicknessFrag }
    , m_smoothShader{ shader_path::quadVert, shader_path::gaussianFrag }
    , m_backgroundShader{ shader_path::quadVert, shader_path::backgroundFrag }
{
    glfwSetWindowUserPointer(m_context, this); // GLFW callbacks can only be static functions
    glfwSetKeyCallback(m_context, keyCallback);
    glfwSetCursorPosCallback(m_context, mouseCallback);
};

Renderer::~Renderer()
{
    glfwTerminate();
}

void Renderer::run()
{
    m_frames = 0;
    m_timeLastFrame = glfwGetTime();
    while (!glfwWindowShouldClose(m_context))
    {
        glfwGetFramebufferSize(m_context, &m_width, &m_height);
        m_projMatrix = glm::perspective(
            render_params::fov, static_cast<float>(m_width) / m_height,
            render_params::near, render_params::far);

        renderDepthNormal();
        renderThickness();
        renderBackground();
        smoothNormal();
        renderFinal();
        m_fluid.update();

        glfwSwapBuffers(m_context);
        glfwPollEvents();

        if (++m_frames == render_params::fpsFrames)
        {
            double timeThisFrame{ glfwGetTime() };
            m_fps = render_params::fpsFrames / (timeThisFrame - m_timeLastFrame);
            std::cout << "FPS: " << m_fps << "\n";
            m_timeLastFrame = timeThisFrame;
            m_frames = 0;
        }
    }
}
