#include "fluid_system.h"

#include <misc/helper.h>

#include <glm/gtc/random.hpp>

#include <iostream>
#include <cmath>

/// @brief The parameters for simulation
namespace simulation_params
{
    constexpr int stepsPerFrame{ 4 };
    constexpr int solverIterations{ 3 };

    constexpr float waterDensity{ 997.0f }; // kg/m^3
    const glm::vec3 gravity{ 0.0f, -9.80665f, 0.0f }; // m/s^2
    //const glm::vec3 gravity{ 0.0f, 0.0f, 0.0f }; // m/s^2
    constexpr float deltaTime{ 1.0f / (60 * stepsPerFrame * 10) };
    constexpr float collisionDamping{ 0.0f };

    constexpr int numParticles{ 10'000 };
    constexpr int expectedParticlesPerCell{ 30 };

    const glm::vec3 boundaryLow{ -4.0f, -4.0f, -4.0f };
    const glm::vec3 boundaryHigh{ 4.0f, 4.0f, 4.0f };
    const glm::vec3 volumeLow{ -3.2f, -3.2f, -3.2f };
    const glm::vec3 volumeHigh{ 3.2f, 3.2f, 3.2f };

    constexpr int workGroupSize{ 1024 };
}

namespace shader_path
{
    const char* gravity{ "shaders/gravity.comp" };
    const char* particlesCells{ "shaders/particles_cells.comp" };
    const char* prefixSum{ "shaders/prefix_sum_cells.comp" };
    const char* reindex{ "shaders/sort_particles.comp" };
    const char* positionSolver{ "shaders/position_solver.comp" };
    const char* velocityCorrect{ "shaders/correct_velocity.comp" };
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
                                    2 * simulation_params::workGroupSize); // for convenience of compute shader group size
    grid.cellSize = diagonal.x / grid.resolution.x;

    return grid;
}

std::vector<glm::vec4> FluidSystem::uniformRandomPositions(BoundingBox volume, int numParticles)
{
    std::vector<glm::vec4> positions{};
    for (int i{ 0 }; i < numParticles; ++i)
    {
        positions.push_back(glm::vec4(glm::linearRand(volume.low, volume.high), 1.0f));
    }
    return positions;
}

void FluidSystem::applyGravity()
{
    m_positions.bind(0);
    m_velocities.bind(1);
    m_nextPositions.bind(2);
    
    m_gravityShader.setUniform("u_gravity", simulation_params::gravity);
    m_gravityShader.setUniform("u_deltaTime", simulation_params::deltaTime);
    m_gravityShader.setUniform("u_boundary.low", simulation_params::boundaryLow);
    m_gravityShader.setUniform("u_boundary.high", simulation_params::boundaryHigh);
    m_gravityShader.setUniform("u_damping", simulation_params::collisionDamping);

    m_gravityShader.activate();
    glDispatchCompute(m_numParticles / simulation_params::workGroupSize, 1, 1);
    glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
}

void FluidSystem::countParticlesCells()
{
    m_nextPositions.bind(0);
    m_numParticlesCells.bind(1);

    m_particlesCellsShader.setUniform("u_boundary.low", simulation_params::boundaryLow);
    m_particlesCellsShader.setUniform("u_boundary.high", simulation_params::boundaryHigh);
    m_particlesCellsShader.setUniform("u_gridResolution", m_grid.resolution);

    m_particlesCellsShader.activate();
    glDispatchCompute(m_numParticles / simulation_params::workGroupSize, 1, 1);
    glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
}

void FluidSystem::prefixSumCells()
{
    m_numParticlesCells.bind(0);
    m_prefixSumParticlesCells.bind(1);

    m_prefixSumShader.activate();
    glDispatchCompute(glm::ceil(m_grid.numCells / (2 * simulation_params::workGroupSize)), 1, 1);
    glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

    //GLuint count[4096];
    //glGetNamedBufferSubData(m_numParticlesCells, 0, 4096 * sizeof(GLuint), count);
    //std::cout << "number of particles:\n";
    //for (int i{ 0 }; i < 4096; i++)
    //    std::cout << count[i] << " ";
    //std::cout << '\n';

    //glGetNamedBufferSubData(m_prefixSumParticlesCells, 0, 4096 * sizeof(GLuint), count);
    //std::cout << "prefix sum number of particles:\n";
    //for (int i{ 0 }; i < 4096; i++)
    //    std::cout << count[i] << " ";
    //std::cout << '\n';

    //std::cout << "Total particles: " << m_numParticles << '\n';
}

void FluidSystem::reindexParticles()
{
    //GLuint count[2048];
    //glGetNamedBufferSubData(m_numParticlesCells, 0, 2048 * sizeof(GLuint), count);
    //std::cout << "number of particles before:\n";
    //for (int i{ 0 }; i < 2048; i++)
    //    std::cout << count[i] << " ";
    //std::cout << '\n';

    m_nextPositions.bind(0); // input position
    m_prefixSumParticlesCells.bind(1);
    m_numParticlesCells.bind(2);
    m_positions.bind(3); // must save reindexed position for computing velocity

    m_reindexShader.setUniform("u_boundary.low", simulation_params::boundaryLow);
    m_reindexShader.setUniform("u_boundary.high", simulation_params::boundaryHigh);
    m_reindexShader.setUniform("u_gridResolution", m_grid.resolution);

    m_reindexShader.activate();
    glDispatchCompute(m_numParticles / simulation_params::workGroupSize, 1, 1);
    glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

    //glGetNamedBufferSubData(m_numParticlesCells, 0, 2048 * sizeof(GLuint), count);
    //std::cout << "number of particles after:\n";
    //for (int i{ 0 }; i < 2048; i++)
    //    std::cout << count[i] << " ";
    //std::cout << '\n';
}

void FluidSystem::positionSolver()
{
    m_intermediatePositions.bind(0);
    m_lambdas.bind(1);
    m_densities.bind(2);
    m_prefixSumParticlesCells.bind(3);
    m_nextPositions.bind(4);

    m_positionSolverShader.setUniform("u_boundary.low", simulation_params::boundaryLow);
    m_positionSolverShader.setUniform("u_boundary.high", simulation_params::boundaryHigh);
    m_positionSolverShader.setUniform("u_gridResolution", m_grid.resolution);
    m_positionSolverShader.setUniform("u_mass", m_mass);
    m_positionSolverShader.setUniform("u_restDensity", simulation_params::waterDensity);
    m_positionSolverShader.setUniform("u_radius", m_grid.cellSize);
    m_positionSolverShader.setUniform("u_damping", simulation_params::collisionDamping);

    m_positionSolverShader.activate();
    glDispatchCompute(m_numParticles / simulation_params::workGroupSize, 1, 1);
    glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

    //constexpr int testNum{ 100 };
    //float densities[testNum];
    //glGetNamedBufferSubData(m_densities, 1024 * sizeof(float), testNum * sizeof(float), densities);
    //std::cout << "Density:\n";
    //for (int i{ 0 }; i < testNum; i++)
    //    std::cout << densities[i] << " ";
    //std::cout << '\n';

    //glm::vec4 positions[testNum];
    //glGetNamedBufferSubData(m_nextPositions, 1024 * sizeof(glm::vec4), testNum * sizeof(glm::vec4), positions);
    //std::cout << "Positions:\n";
    //for (int i{ 0 }; i < testNum; i++)
    //    std::cout << positions[i].x << " " << positions[i].y << " " << positions[i].z << "\n";
}

void FluidSystem::updatePosition()
{
    SSBO::swap(m_positions, m_intermediatePositions);
    positionSolver();
    SSBO::swap(m_positions, m_intermediatePositions);
    for (int i{ 1 }; i < simulation_params::solverIterations; i++)
    {
        SSBO::swap(m_intermediatePositions, m_nextPositions);
        positionSolver();
    }
    glDispatchCompute(m_numParticles / simulation_params::workGroupSize, 1, 1);
    glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
}

void FluidSystem::velecityCorrection()
{
    m_positions.bind(0);
    m_nextPositions.bind(1);
    m_velocities.bind(2);

    m_velocityCorrectShader.setUniform("u_deltaTime", simulation_params::deltaTime);

    m_velocityCorrectShader.activate();

}

FluidSystem::FluidSystem()
    : m_boundary{ simulation_params::boundaryLow, simulation_params::boundaryHigh }
    , m_volume{ simulation_params::volumeLow, simulation_params::volumeHigh }
    , m_numParticles{ helper::roundUp(simulation_params::numParticles, simulation_params::workGroupSize) }
    , m_mass{ m_volume.volume() * simulation_params::waterDensity / m_numParticles }
    , m_grid{ createGrid(m_boundary, m_volume, m_numParticles, simulation_params::expectedParticlesPerCell) }
    , m_positions{ GL_STATIC_DRAW, m_numParticles * sizeof(glm::vec4), uniformRandomPositions(m_volume, m_numParticles).data() }
    , m_velocities{ GL_STATIC_DRAW, m_numParticles * sizeof(glm::vec4), std::vector<glm::vec4>(m_numParticles).data() }
    , m_numParticlesCells{ GL_STATIC_COPY, m_grid.numCells * sizeof(GLuint), std::vector<GLuint>(m_grid.numCells).data()}
    , m_prefixSumParticlesCells{ GL_STATIC_COPY, m_grid.numCells * sizeof(GLuint) }
    , m_intermediatePositions{ GL_STATIC_COPY, m_numParticles * sizeof(glm::vec4) }
    , m_nextPositions{ GL_STATIC_COPY, m_numParticles * sizeof(glm::vec4) }
    , m_densities{ GL_STATIC_DRAW, m_numParticles * sizeof(float) }
    , m_lambdas{ GL_STATIC_COPY, m_numParticles * sizeof(float) }
    , m_VAO{}
    , m_gravityShader{ shader_path::gravity }
    , m_particlesCellsShader{ shader_path::particlesCells }
    , m_prefixSumShader{ shader_path::prefixSum }
    , m_reindexShader{ shader_path::reindex }
    , m_positionSolverShader{ shader_path::positionSolver }
    , m_velocityCorrectShader{ shader_path::velocityCorrect }
{
    //GLuint count[10];
    //std::cout << "Buffer ID: " << m_numParticlesCells << '\n';
    //glGetNamedBufferSubData(m_numParticlesCells, 0, 10 * sizeof(GLuint), count);
    //std::cout << "The number of particles:\n";
    //for (int i{ 0 }; i < 10; i++)
    //    std::cout << count[i] << " ";
    //std::cout << '\n';
}

void FluidSystem::draw(const ShaderProgram& program) const
{
    m_VAO.setAttrib(m_positions, 0, 3, GL_FLOAT, sizeof(glm::vec4), 0);
    program.activate();
    glPointSize(5.0f);
    glDrawArrays(GL_POINTS, 0, m_numParticles);
}

void FluidSystem::update()
{
    for (int i{ 0 }; i < simulation_params::stepsPerFrame; ++i)
    {
        applyGravity();
        countParticlesCells();
        prefixSumCells();
        reindexParticles();
        updatePosition();
        velecityCorrection();
        SSBO::swap(m_positions, m_nextPositions);
    }
}
