#pragma once

#include <glm/glm.hpp>

/// @brief The parameters for simulation
namespace simulation_params
{
    constexpr float waterDensity{ 997.0f }; // kg/m^3
    const glm::vec3 gravity{ 0.0f, 9.80665f, 0.0f }; // m/s^2
    constexpr float deltaTime{ 1.0f / 240 }; // 4 steps per frame
    constexpr int expectedParticlesPerCell{ 30 };
    constexpr int workGroupSize{ 1024 };
}

/// @brief Parameters for the renderer
namespace render_params
{
    constexpr int contextVersionMajor{ 4 };
    constexpr int contextVersionMinor{ 6 };
}
