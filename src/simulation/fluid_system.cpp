#include "fluid_system.h"

#include <cmath>

FluidSystem::FluidSystem(const BoundingBox& boundary, const BoundingBox& volume, size_t numParticles, float compression)
    : m_boundary{ boundary }
{

}
