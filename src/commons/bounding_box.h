#pragma once

#include <glm/glm.hpp>

/// @brief A bounding box represented by its lowest and highest coordinates
struct BoundingBox
{
    /// @brief The lowest x, y, and z values
    glm::vec3 low;
    /// @brief The highest x, y, and z values
    glm::vec3 high;
};