#include "fluid_system.h"

#include <misc/helper.h>

#include <glm/gtc/random.hpp>

#include <iostream>
#include <cmath>

/// @brief The parameters for simulation
namespace simulation_params
{
    constexpr int stepsPerFrame{ 4 };

    constexpr float waterDensity{ 997.0f }; // kg/m^3
    const glm::vec3 gravity{ 0.0f, 9.80665f, 0.0f }; // m/s^2
    constexpr float deltaTime{ 1.0f / (60 * stepsPerFrame) };
    constexpr float collisionDamping{ 0.1f };

    constexpr int numParticles{ 100'000 };
    constexpr int expectedParticlesPerCell{ 30 };

    const glm::vec3 boundaryLow{ -4.0f, -4.0f, -4.0f };
    const glm::vec3 boundaryHigh{ 4.0f, 4.0f, 4.0f };
    const glm::vec3 volumeLow{ -3.2f, -3.2f, -3.2f };
    const glm::vec3 volumeHigh{ 3.2f, 3.2f, 3.2f };

    constexpr int workGroupSize{ 1024 };
}

namespace shader_path
{

}

Grid FluidSystem::createGrid(BoundingBox box, BoundingBox volume, int numParticles, int expectedParticlesPerCell)
{
    int expectedNumCells{ numParticles / expectedParticlesPerCell + 1};
    float expectedCellVolume{ volume.volume() / expectedNumCells };
    float expectedCellSize{ std::cbrt(expectedCellVolume) };

    Grid grid{};
    glm::vec3 diagonal{ box.high - box.low };
    grid.resolution = glm::ceil(diagonal / expectedCellSize);
    grid.numCells = helper::roundUp(grid.resolution.x * grid.resolution.y * grid.resolution.z,
                                    2 * simulation_params::workGroupSize); // round up to avoid bound checking
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
    , m_positions{ GL_STATIC_DRAW, m_numParticles * sizeof(glm::vec4), uniformRandomPositions(m_volume, m_numParticles).data() }
    , m_velocities{ GL_STATIC_DRAW, m_numParticles * sizeof(glm::vec4), std::vector<glm::vec4>(m_numParticles).data() }
    , m_numParticlesCells{ GL_STATIC_COPY, m_grid.numCells * sizeof(GLuint) }
    , m_prefixSumParticlesCells{ GL_STATIC_COPY, m_grid.numCells * sizeof(GLuint) }
    , m_densities{ GL_STATIC_COPY, m_numParticles * sizeof(float) }
    , m_lambdas{ GL_STATIC_COPY, m_numParticles * sizeof(float) }
    , m_VAO{}
{
    std::cout << "Positions on GPU\n";
    glm::vec4 positions[10];
    glGetNamedBufferSubData(m_positions, 0, 10 * sizeof(glm::vec4), positions);
    for (int i{ 0 }; i < 10; ++i)
    {
        std::cout << positions[i].x << ' ' << positions[i].y << ' ' << positions[i].z << ' ' << positions[i].w << '\n';
    }

    std::cout << "Velocities on GPU\n";
    glm::vec4 velocities[10];
    glGetNamedBufferSubData(m_velocities, 0, 10 * sizeof(glm::vec4), velocities);
    for (int i{ 0 }; i < 10; ++i)
    {
        std::cout << velocities[i].x << ' ' << velocities[i].y << ' ' << velocities[i].z << ' ' << velocities[i].w << '\n';
    }
}

void FluidSystem::draw(const ShaderProgram& program) const
{
    m_VAO.setAttrib(m_positions, 0, 4, GL_FLOAT, sizeof(glm::vec4), 0);
    program.activate();
    glDrawArrays(GL_POINTS, 0, m_numParticles);
}
