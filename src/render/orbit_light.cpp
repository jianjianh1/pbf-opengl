#include "orbit_light.h"

#include <glm/ext/matrix_transform.hpp>

OrbitLight::OrbitLight(
    float distance, float angleFromY, float angleFromX, const glm::vec3& center,
    const glm::vec3& ambient, const glm::vec3& diffuse, const glm::vec3& specular)
    : m_distance{ distance }
    , m_angleFromY{ angleFromY }
    , m_angleFromX{ angleFromX }
    , m_center{ center }
    , m_ambient{ ambient }
    , m_diffuse{ diffuse }
    , m_specular{ specular }
{
}

void OrbitLight::moveVertical(float angle)
{
    m_angleFromY += angle;
    if (m_angleFromY > 179.0f) m_angleFromY = 179.0f;
    if (m_angleFromY < 1.0f) m_angleFromY = 1.0f;
}

void OrbitLight::moveHorizontal(float angle)
{
    m_angleFromX += angle;
}

void OrbitLight::moveDistance(float diff, float min, float max)
{
    m_distance += diff;
    if (m_distance > max) m_distance = max;
    if (m_distance < min) m_distance = min;
}

glm::vec3 OrbitLight::position() const
{
    return glm::vec3 {
        m_distance * glm::sin(glm::radians(m_angleFromY)) * glm::cos(glm::radians(m_angleFromX)),
        m_distance * glm::cos(glm::radians(m_angleFromY)),
        m_distance * glm::sin(glm::radians(m_angleFromY)) * glm::sin(glm::radians(m_angleFromX))
    } + m_center;
}

glm::mat4 OrbitLight::viewMatrix(glm::vec3 worldUp) const
{
    return glm::lookAt(position(), m_center, worldUp);
}

