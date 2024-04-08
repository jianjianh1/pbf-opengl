#pragma once

#include <misc/bounding_box.h>
#include <misc/grid.h>

#include <glm/glm.hpp>
#include <glad/glad.h>

/// @brief A fluid system that's based on position based fluids
class FluidSystem
{
private:
    /// @brief The SSBO for particles' starting position in one frame
    GLuint m_startPosition;

    /// @brief The SSBO for particles' intermediate position for computing
    GLuint m_intermediatePosition;

    /// @brief The other SSBO for particles' next position for computing
    GLuint m_nextPosition;

    /// @brief The SSBO for storing the velocities of particles
    GLuint m_velocities;
    
    /// @brief The SSBO for storing the densities of particles
    GLuint m_densities;

    /// @brief The SSBO for storing lambdas (step size in the Newton's method) of particles
    GLuint m_lambdas;

    /// @brief The effective radius for SPH method
    float effectiveRadius;

    /// @brief The boundary of this system
    BoundingBox m_boundary;

    /// @brief The grid used for finding neighbors
    Grid m_grid;

    /// @brief The SSBO for storing the number of particles in the cells
    GLuint m_numParticlesCells;

    /// @brief The SSBO for storing the prefix sum of particles in the cells
    GLuint m_prefixSumParticlesCells;

public:
    /// @brief Create a fluid system with the given boundary and the given initial volume of fluid with
    ///        the number of particles
    /// @param boundary the initial boundary of the system
    /// @param volume the initial volume
    /// @param numParticles the number of particles
    /// @param compression the initial compression of the fluid
    FluidSystem(const BoundingBox& boundary, const BoundingBox& volume, size_t numParticles, float compression = 1.0f);
};