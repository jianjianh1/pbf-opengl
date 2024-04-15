#include "fluid_system.h"

#include <misc/helper.h>

#include <glm/gtc/random.hpp>

#include <iostream>
#include <cmath>

/// @brief The parameters for simulation
namespace simulation_params
{
    constexpr int stepsPerFrame{ 2 };
    constexpr int solverIterations{ 3 };

    constexpr float waterDensity{ 997.0f }; // kg/m^3
    const glm::vec3 gravity{ 0.0f, -9.80665f, 0.0f }; // m/s^2
    //const glm::vec3 gravity{ 0.0f, 0.0f, 0.0f }; // m/s^2
    constexpr float deltaTime{ 1.0f / (60 * stepsPerFrame) };
    constexpr float collisionDamping{ 0.1f };

    constexpr int numParticles{ 80'000 };
    constexpr int expectedParticlesPerCell{ 20 };

    const glm::vec3 boundaryLow{ -1.0f, -1.0f, -1.0f};
    const glm::vec3 boundaryHigh{ 1.5f, 1.0f, 1.0f };
    const glm::vec3 volumeLow{ -0.9f, -0.9f , -0.9f };
    //const glm::vec3 volumeLow{ -1.0f, -1.0f , -1.0f };
    const glm::vec3 volumeHigh{ 0.5f, 0.5f, 0.5f };
    //const glm::vec3 volumeHigh{1.0f, 1.0f, 1.0f};

    constexpr int workGroupSize{ 1024 };
}

namespace shader_path
{
    const char* gravity{ "shaders/gravity.comp" };
    const char* particlesCells{ "shaders/particles_cells.comp" };
    const char* prefixSumLocal{ "shaders/prefix_sum_cells_local.comp" };
    const char* prefixSumGlobal{ "shaders/prefix_sum_cells_global.comp" };
    const char* reindex{ "shaders/sort_particles.comp" };
    const char* computeLambda{ "shaders/compute_lambda.comp" };
    const char* computePosition{ "shaders/compute_position.comp" };
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
    m_startPosition.bind(0);
    m_velocities.bind(1);
    m_intermediatePositions.bind(2);
    
    m_gravityShader.setUniform("u_gravity", simulation_params::gravity);
    m_gravityShader.setUniform("u_deltaTime", simulation_params::deltaTime);
    m_gravityShader.setUniform("u_boundary.low", m_boundary.low);
    m_gravityShader.setUniform("u_boundary.high", m_boundary.high);
    m_gravityShader.setUniform("u_damping", simulation_params::collisionDamping);

    m_gravityShader.activate();
    glDispatchCompute(m_numParticles / simulation_params::workGroupSize, 1, 1);
    glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
}

void FluidSystem::countParticlesCells()
{
    m_intermediatePositions.bind(0);
    m_numParticlesCells.bind(1);

    m_particlesCellsShader.setUniform("u_boundary.low", m_boundary.low);
    m_particlesCellsShader.setUniform("u_boundary.high", m_boundary.high);
    m_particlesCellsShader.setUniform("u_gridResolution", m_grid.resolution);

    m_particlesCellsShader.activate();
    glDispatchCompute(m_numParticles / simulation_params::workGroupSize, 1, 1);
    glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
}

void FluidSystem::prefixSumCells()
{
    //glCopyNamedBufferSubData(m_numParticles, m_prefixSumParticlesCells, 0, 0, m_grid.numCells * sizeof(GLuint));
    m_numParticlesCells.bind(0);
    m_prefixSumParticlesCells.bind(1);

    m_prefixSumLocalShader.activate();
    glDispatchCompute(m_grid.numCells / (2 * simulation_params::workGroupSize), 1, 1);
    glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT | GL_BUFFER_UPDATE_BARRIER_BIT);

    GLuint localSteps{ static_cast<GLuint>(glm::ceil(glm::log2(static_cast<float>(simulation_params::workGroupSize))) + 1) };
    GLuint globalSteps{ static_cast<GLuint>(glm::ceil(glm::log2(static_cast<float>(m_grid.numCells))) + 1) };
    m_prefixSumParticlesCells.bind(0);
    m_prefixSumGlobalShader.activate();
    for (GLuint step{ localSteps }; step < globalSteps; ++step)
    {
        m_prefixSumGlobalShader.setUniform("u_step", step);
        glDispatchCompute(m_grid.numCells / (2 * simulation_params::workGroupSize), 1, 1);
        glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT | GL_BUFFER_UPDATE_BARRIER_BIT);
    }

    //GLuint total;
    //glGetNamedBufferSubData(m_prefixSumParticlesCells, (m_grid.resolution.x * m_grid.resolution.y * m_grid.resolution.z - 1) * sizeof(GLuint), sizeof(GLuint), &total);
    //std::cout << "Total computed: " << total << "\n";
    //std::cout << "Total particles: " << m_numParticles << '\n';
    //if (total != m_numParticles) {
        //GLuint* count = new GLuint[m_grid.numCells]{};
        //glGetNamedBufferSubData(m_numParticlesCells, 0, m_grid.numCells * sizeof(GLuint), count);
        //std::cout << "number of particles:\n";
        //for (int i{ 0 }; i < m_grid.numCells; i++)
        //    std::cout << count[i] << " ";
        //std::cout << '\n';
        //glGetNamedBufferSubData(m_prefixSumParticlesCells, 0, m_grid.numCells * sizeof(GLuint), count);
        //std::cout << "prefix sum number of particles:\n";
        //for (int i{ 0 }; i < m_grid.numCells; i++)
        //    std::cout << count[i] << " ";
        //std::cout << '\n';
        //delete[] count;
    //}
}

void FluidSystem::reindexParticles()
{
    //GLuint* count = new GLuint[m_grid.numCells];
    //glGetNamedBufferSubData(m_numParticlesCells, 0, m_grid.numCells * sizeof(GLuint), count);
    //std::cout << "number of particles in cells before:\n";
    //for (int i{ 0 }; i < m_grid.numCells; i++)
    //    std::cout << count[i] << " ";
    //std::cout << '\n';

    m_prefixSumParticlesCells.bind(0);
    m_numParticlesCells.bind(1);
    m_intermediatePositions.bind(2);
    m_nextPositions.bind(3);
    m_startPosition.bind(4);
    m_savedPositions.bind(5); // save for velocity correction

    m_reindexShader.setUniform("u_boundary.low", m_boundary.low);
    m_reindexShader.setUniform("u_boundary.high", m_boundary.high);
    m_reindexShader.setUniform("u_gridResolution", m_grid.resolution);

    m_reindexShader.activate();
    glDispatchCompute(m_numParticles / simulation_params::workGroupSize, 1, 1);
    glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

    //glGetNamedBufferSubData(m_numParticlesCells, 0, m_grid.numCells * sizeof(GLuint), count);
    //std::cout << "number of particles in cells after:\n";
    //for (int i{ 0 }; i < m_grid.numCells; i++)
    //    std::cout << count[i] << " ";
    //std::cout << '\n';

    //const int offset = m_numParticles / 2;
    //constexpr int n = 10;
    //glm::vec4 positions[n];
    //glad_glGetNamedBufferSubData(m_startPosition, offset, n * sizeof(glm::vec4), positions);
    //std::cout << "Particle positions:\n";
    //for (int i{ 0 }; i < n; i++)
    //    std::cout << positions[i].x << " " << positions[i].y << " " << positions[i].z << '\n';

}

void FluidSystem::positionSolver()
{
    m_intermediatePositions.bind(0);
    m_lambdas.bind(1);
    m_densities.bind(2);
    m_prefixSumParticlesCells.bind(3);

    m_computeLambdaShader.setUniform("u_boundary.low", m_boundary.low);
    m_computeLambdaShader.setUniform("u_boundary.high", m_boundary.high);
    m_computeLambdaShader.setUniform("u_gridResolution", m_grid.resolution);
    m_computeLambdaShader.setUniform("u_mass", m_mass);
    m_computeLambdaShader.setUniform("u_restDensity", simulation_params::waterDensity);
    m_computeLambdaShader.setUniform("u_radius", m_grid.cellSize);

    m_computeLambdaShader.activate();
    glDispatchCompute(m_numParticles / simulation_params::workGroupSize, 1, 1);
    glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

    m_intermediatePositions.bind(0);
    m_lambdas.bind(1);
    m_prefixSumParticlesCells.bind(2);
    m_nextPositions.bind(3);

    m_computePositionShader.setUniform("u_boundary.low", m_boundary.low);
    m_computePositionShader.setUniform("u_boundary.high", m_boundary.high);
    m_computePositionShader.setUniform("u_gridResolution", m_grid.resolution);
    m_computePositionShader.setUniform("u_mass", m_mass);
    m_computePositionShader.setUniform("u_restDensity", simulation_params::waterDensity);
    m_computePositionShader.setUniform("u_radius", m_grid.cellSize);
    m_computePositionShader.setUniform("u_damping", simulation_params::collisionDamping);
    m_computePositionShader.setUniform("u_iterations", simulation_params::solverIterations);

    m_computePositionShader.activate();
    glDispatchCompute(m_numParticles / simulation_params::workGroupSize, 1, 1);
    glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

    //constexpr int testNum{ 10 };
    //float densities[testNum];
    //glGetNamedBufferSubData(m_densities, 0, testNum * sizeof(float), densities);
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
    for (int i{ 0 }; i < simulation_params::solverIterations; i++)
    {
        SSBO::swap(m_intermediatePositions, m_nextPositions);
        positionSolver();
    }
}

void FluidSystem::velecityCorrection()
{
    m_savedPositions.bind(0);
    m_nextPositions.bind(1);
    m_velocities.bind(2);

    m_velocityCorrectShader.setUniform("u_deltaTime", simulation_params::deltaTime);

    m_velocityCorrectShader.activate();
    glDispatchCompute(m_numParticles / simulation_params::workGroupSize, 1, 1);
    glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
}

FluidSystem::FluidSystem()
    : m_boundary{ simulation_params::boundaryLow, simulation_params::boundaryHigh }
    , m_volume{ simulation_params::volumeLow, simulation_params::volumeHigh }
    , m_numParticles{ helper::roundUp(simulation_params::numParticles, simulation_params::workGroupSize) }
    , m_mass{ m_volume.volume() * simulation_params::waterDensity / m_numParticles }
    , m_grid{ createGrid(m_boundary, m_volume, m_numParticles, simulation_params::expectedParticlesPerCell) }
    , m_startPosition{ GL_STATIC_DRAW, m_numParticles * sizeof(glm::vec4), uniformRandomPositions(m_volume, m_numParticles).data() }
    , m_savedPositions{ GL_STATIC_COPY, m_numParticles * sizeof(glm::vec4) }
    , m_intermediatePositions{ GL_STATIC_COPY, m_numParticles * sizeof(glm::vec4) }
    , m_nextPositions{ GL_STATIC_COPY, m_numParticles * sizeof(glm::vec4) }
    , m_velocities{ GL_STATIC_COPY, m_numParticles * sizeof(glm::vec4), std::vector<glm::vec4>(m_numParticles).data() }
    , m_numParticlesCells{ GL_STATIC_COPY, m_grid.numCells * sizeof(GLuint), std::vector<GLuint>(m_grid.numCells).data()}
    , m_prefixSumParticlesCells{ GL_STATIC_COPY, m_grid.numCells * sizeof(GLuint) }
    , m_densities{ GL_STATIC_DRAW, m_numParticles * sizeof(float) }
    , m_lambdas{ GL_STATIC_COPY, m_numParticles * sizeof(float) }
    , m_VAO{}
    , m_gravityShader{ shader_path::gravity }
    , m_particlesCellsShader{ shader_path::particlesCells }
    , m_prefixSumLocalShader{ shader_path::prefixSumLocal }
    , m_prefixSumGlobalShader{ shader_path::prefixSumGlobal }
    , m_reindexShader{ shader_path::reindex }
    , m_computeLambdaShader{ shader_path::computeLambda }
    , m_computePositionShader{ shader_path::computePosition }
    , m_velocityCorrectShader{ shader_path::velocityCorrect }
{
    //GLuint count[10];
    //std::cout << "Buffer ID: " << m_numParticlesCells << '\n';
    //glGetNamedBufferSubData(m_numParticlesCells, 0, 10 * sizeof(GLuint), count);
    //std::cout << "The number of particles:\n";
    //for (int i{ 0 }; i < 10; i++)
    //    std::cout << count[i] << " ";
    //std::cout << '\n';

    std::cout << "Grid resolution: " << m_grid.resolution.x << ' ' << m_grid.resolution.y << ' ' << m_grid.resolution.z << '\n';
    std::cout << "Cell size: " << m_grid.cellSize << '\n';
    std::cout << "Number of particles: " << m_numParticles << '\n';
    std::cout << "Particle mass: " << m_mass << '\n';
    std::cout << '\n';
}

void FluidSystem::draw(const ShaderProgram& program) const
{
    //glCopyNamedBufferSubData(m_startPosition, m_savedPositions, 0, 0, m_numParticles * sizeof(glm::vec4));
    m_VAO.setAttrib(m_startPosition, 0, 3, GL_FLOAT, sizeof(glm::vec4), 0);
    m_VAO.setAttrib(m_densities, 1, 1, GL_FLOAT, sizeof(float), 0);
    program.activate();
    glPointSize(5.0f);
    glDrawArrays(GL_POINTS, 0, m_numParticles);
    //glUseProgram(program);
    //float red[]{ 0.0f, 1.0f, 0.0f, 1.0f };
    //glUniform4fv(glGetUniformLocation(program, "u_color"), 1, red);
    //glDrawArrays(GL_POINTS, m_numParticles - 100, 99);
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
        SSBO::swap(m_startPosition, m_nextPositions);
    }
}

void FluidSystem::reset()
{
    m_startPosition = SSBO{
        GL_STATIC_DRAW,
        static_cast<GLsizeiptr>(m_numParticles * sizeof(glm::vec4)),
        uniformRandomPositions(m_volume, m_numParticles).data()
    };
}

void FluidSystem::moveBoundaryX(float amount)
{
    m_boundary.high.x += amount;
    if (m_boundary.high.x < m_volume.high.x) m_boundary.high.x = m_volume.high.x;
    if (m_boundary.high.x > 3 * simulation_params::boundaryHigh.x) m_boundary.high.x = 3 * simulation_params::boundaryHigh.x;
}

void FluidSystem::moveBoundaryZ(float amount)
{
    m_boundary.high.z += amount;
    if (m_boundary.high.z < m_volume.high.z) m_boundary.high.z = m_volume.high.z;
    if (m_boundary.high.z > 3 * simulation_params::boundaryHigh.z) m_boundary.high.z = 3 * simulation_params::boundaryHigh.z;
}
