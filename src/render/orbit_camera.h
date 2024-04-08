#pragma once

#include <glm/glm.hpp>

/// @brief A camera that moves like in a fixed orbit
class OrbitCamera
{
private:
    /// @brief The distance from the orbit center
    float m_distance{};

    /// @brief angle in degree from positive y axis
    float m_angleFromY{};

    /// @brief angle in degree from positive x axis
    float m_angleFromX{};

    /// @brief the orbit center
    const glm::vec3 m_center{};

public:
    /// @brief Construct a camera with an initial distance, angles, orbit center,
    ///        and up direction
    /// @param distance the distance from the orbit center
    /// @param angleFromY the angle in degree from y axis
    /// @param angleFromX the angle in degree form x axis
    /// @param lookAt the orbit center
    /// @param worldUp the up direction
    OrbitCamera(
        float distance, float angleFromY, float angleFromX,
        const glm::vec3& lookAt = glm::vec3(0.0f));

    /// @brief Move this camera vertically by the angle
    void moveVertical(float angle);

    /// @brief Move this camera horizontally by the angle
    void moveHorizontal(float angle);

    /// @brief Change the distance between this camera and orbit center
    /// @param diff the difference in distance
    /// @param min the lower bound distance
    /// @param max the upper bound distance
    void moveDistance(float diff, float min = 0.1f, float max = 1000.0f);

    /// @brief Get the position of camera
    glm::vec3 position() const;

    /// @brief Get the view matrix of camera
    glm::mat4 viewMatrix(glm::vec3 worldUp = glm::vec3{ 0.0f, 1.0f, 0.0f }) const;
};
