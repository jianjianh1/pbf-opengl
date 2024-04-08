#include "orbit_camera.h"

#include <glm/ext/matrix_transform.hpp>

OrbitCamera::OrbitCamera(
    float distance, float angleFromY, float angleFromX,
    const glm::vec3& lookAt)
    : m_distance{ distance }
    , m_angleFromY{ glm::clamp(angleFromY, 1.0f, 179.0f) }
    , m_angleFromX{ angleFromX }
    , m_center{ lookAt }
{
}

void OrbitCamera::moveVertical(float angle)
{
    m_angleFromY += angle;
    if (m_angleFromY > 179.0f) m_angleFromY = 179.0f;
    if (m_angleFromY < 1.0f) m_angleFromY = 1.0f;
}

void OrbitCamera::moveHorizontal(float angle)
{
    m_angleFromX += angle;
}

void OrbitCamera::moveDistance(float diff, float min, float max)
{
    m_distance += diff;
    if (m_distance > max) m_distance = max;
    if (m_distance < min) m_distance = min;
}

glm::vec3 OrbitCamera::position() const
{
    return glm::vec3 {
        m_distance * glm::sin(glm::radians(m_angleFromY)) * glm::cos(glm::radians(m_angleFromX)),
        m_distance * glm::cos(glm::radians(m_angleFromY)),
        m_distance * glm::sin(glm::radians(m_angleFromY)) * glm::sin(glm::radians(m_angleFromX))
    } + m_center;
}

glm::mat4 OrbitCamera::viewMatrix(glm::vec3 worldUp) const
{
    return glm::lookAt(position(), m_center, worldUp);
}
