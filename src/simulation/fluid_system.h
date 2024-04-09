#pragma once

#include <misc/bounding_box.h>
#include <misc/grid.h>
#include <glutils/ssbo.h>
#include <glutils/vao.h>
#include <glutils/shader_program.h>

#include <glm/glm.hpp>
#include <glad/glad.h>

#include <vector>

/// @brief A fluid system that's based on position based fluids
class FluidSystem
{
private:
    /// @brief The boundary of this system
    BoundingBox m_boundary;

    /// @brief The volume of fluid in m^3; ideally it will not be changed
    BoundingBox m_volume;

    /// @brief Total number of particles
    int m_numParticles;

    /// @brief The mass of each particle in kg
    float m_mass;

    /// @brief The grid used for finding neighbors
    Grid m_grid;

    /// @brief The SSBO for particles' starting position in one frame; initialized, used for drawing
    SSBO m_positions;

    /// @brief The SSBO for storing the velocities of particles; initialized
    SSBO m_velocities;

    /// @brief The SSBO for storing the number of particles in the cells
    SSBO m_numParticlesCells;

    /// @brief The SSBO for storing the prefix sum of particles in the cells
    SSBO m_prefixSumParticlesCells;

    /// @brief The SSBO for particles' intermediate position for computing
    SSBO m_intermediatePositions;

    /// @brief The other SSBO for particles' next position for computing
    SSBO m_nextPositions;

    /// @brief The SSBO for storing the densities of particles, only for debugging
    SSBO m_densities;

    /// @brief The SSBO for storing lambdas (step size in the Newton's method) of particles
    SSBO m_lambdas;

    /// @brief The VAO for rendering particles
    VAO m_VAO;

    /// @brief Shader for computing gravity
    ShaderProgram m_gravityShader;

    /// @brief Shader for counting the number of particles in cells
    ShaderProgram m_particlesCellsShader;

    /// @brief Shader for computing the prefix sum
    ShaderProgram m_prefixSumShader;

    /// @brief Shader for reindexing particles
    ShaderProgram m_reindexShader;

    /// @brief Shader for computing lambdas
    ShaderProgram m_positionSolverShader;

    /// @brief Shader for velocity correction
    ShaderProgram m_velocityCorrectShader;

    /// @brief Create a grid based on the parameters
    /// @param box the box to be divided into a grid of cells
    /// @param volume the volume of fluid
    /// @param numParticles the number of particles
    /// @param expectedParticlesPerCell expected number of particles per cell
    /// @return the grid
    static Grid createGrid(BoundingBox box, BoundingBox volume, int numParticles, int expectedParticlesPerCell);

    /// @brief Get positions distributed uniformly in the given volume
    /// @param volume the volume
    /// @param numParticles the number of particles
    /// @return the positions
    static std::vector<glm::vec4> uniformRandomPositions(BoundingBox volume, int numParticles);

    /// @brief Apply gravity to the positions to get predicted positions
    void applyGravity();

    /// @brief Count the number of particles in each cell
    void countParticlesCells();

    /// @brief Compute the prefix sum number of particles in each cell
    void prefixSumCells();

    /// @brief Reindex the particles
    void reindexParticles();

    /// @brief Compute the lambdas in position based dynamics
    void positionSolver();

    /// @brief Update position using solve iterations
    void updatePosition();

    /// @brief Correct velocities
    void velecityCorrection();

public:
    /// @brief Create a fluid system
    FluidSystem();

    /// @brief Draw the particles
    void draw(const ShaderProgram& program)const;

    /// @brief Update to the next frame
    void update();
};