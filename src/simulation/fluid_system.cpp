#include "fluid_system.h"

#include <misc/helper.h>

#include <glm/gtc/random.hpp>

#include <iostream>
#include <cmath>

/// @brief The parameters for simulation
namespace simulation_params
{
    constexpr float waterDensity{ 997.0f }; // kg/m^3
    const glm::vec3 gravity{ 0.0f, 9.80665f, 0.0f }; // m/s^2
    constexpr float deltaTime{ 1.0f / 240 }; // 4 steps per frame
    constexpr int numParticles{ 100'000 };
    constexpr int expectedParticlesPerCell{ 30 };
    const glm::vec3 boundaryLow{ -1.0f, -1.0f, -1.0f };
    const glm::vec3 boundaryHigh{ 1.0f, 1.0f, 1.0f };
    const glm::vec3 volumeLow{ -0.8f, -0.8f, -0.8f };
    const glm::vec3 volumeHigh{ 0.8f, 0.8f, 0.8f };

    constexpr int workGroupSize{ 1024 };
}

Grid FluidSystem::createGrid(BoundingBox box, BoundingBox volume, int numParticles, int expectedParticlesPerCell)
{
    int expectedNumCells{ numParticles / expectedParticlesPerCell + 1};
    float expectedCellVolume{ volume.volume() / expectedNumCells };
    float expectedCellSize{ std::cbrt(expectedCellVolume) };

    Grid grid{};
    glm::vec3 diagonal{ box.high - box.low };
    grid.resolution = glm::ceil(diagonal / expectedCellSize);
    grid.numCells = grid.resolution.x * grid.resolution.y * grid.resolution.z;
    grid.cellSize = diagonal.x / grid.resolution.x;

    std::cout << "Grid resolution: " << grid.resolution.x << " " << grid.resolution.y << " " << grid.resolution.z << "\n";
    std::cout << "Radius: " << grid.cellSize << '\n';
    return grid;
}

std::vector<glm::vec4> FluidSystem::uniformRandomPositions(BoundingBox volume, int numParticles)
{
    std::vector<glm::vec4> positions{};
    for (int i{ 0 }; i < numParticles; ++i)
    {
        positions.push_back(glm::vec4(glm::linearRand(volume.low, volume.high), 1.0f));
    }
    for (int i{ 0 }; i < 10; ++i)
    {
        std::cout << positions[i].x << ' ' << positions[i].y << ' ' << positions[i].z << ' ' << positions[i].w << '\n';
    }
    return positions;
}

FluidSystem::FluidSystem()
    : m_boundary{ simulation_params::boundaryLow, simulation_params::boundaryHigh }
    , m_volume{ simulation_params::volumeLow, simulation_params::volumeHigh }
    , m_numParticles{ helper::roundUp(simulation_params::numParticles, simulation_params::workGroupSize) }
    , m_mass{ m_volume.volume() * simulation_params::waterDensity / m_numParticles }
    , m_grid{ createGrid(m_boundary, m_volume, m_numParticles, simulation_params::expectedParticlesPerCell) }
    , m_position{GL_STATIC_DRAW, m_numParticles * sizeof(glm::vec4), uniformRandomPositions(m_volume, m_numParticles).data() }
{

}
