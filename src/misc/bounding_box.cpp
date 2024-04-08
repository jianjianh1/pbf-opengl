#include "bounding_box.h"

float BoundingBox::volume() const
{
    glm::vec3 diagonal{ high - low };
    return diagonal.x * diagonal.y * diagonal.z;
}
