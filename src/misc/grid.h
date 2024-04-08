#pragma once

#include <glm/glm.hpp>

/// @brief Grid structure for finding neighbors of a particle
struct Grid
{
    int numCells;
    glm::uvec3 resolution;
};
