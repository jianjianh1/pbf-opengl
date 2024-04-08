#pragma once

#include <glm/glm.hpp>

/// @brief A light object that moves in a fixed orbit
class OrbitLight
{
private:
    /// @brief The distance from orbit center
    float m_distance{};

    /// @brief The angle in degree from positive y
    float m_angleFromY{};

    /// @brief The angle in degree from positive x
    float m_angleFromX{};

    /// @brief The orbit center
    const glm::vec3 m_center{};

    /// @brief The ambient light intensity
    const glm::vec3 m_ambient{};

    /// @brief The diffuse light intensity
    const glm::vec3 m_diffuse{};

    /// @brief The specular light intensity
    const glm::vec3 m_specular{};

public:
    /// @brief Construct a light with the parameters
    OrbitLight(
        float distance, float angleFromY, float angleFromX,
        const glm::vec3& center = glm::vec3(1.0f),
        const glm::vec3& ambient = glm::vec3(0.1f),
        const glm::vec3& diffuse = glm::vec3(1.0f),
        const glm::vec3& specular = glm::vec3(1.0f));

    /// @brief Move this light vertically by the angle
    void moveVertical(float angle);

    /// @brief Move this light horizontally by the angle
    void moveHorizontal(float angle);

    /// @brief Change the distance between this light and orbit center
    /// @param diff the difference in distance
    /// @param min the lower bound distance
    /// @param max the upper bound distance
    void moveDistance(float diff, float min = 0.1f, float max = 1000.0f);

    /// @brief Get the position of this light
    glm::vec3 position() const;

    /// @brief Get the view matrix of this light
    glm::mat4 viewMatrix(glm::vec3 worldUp = glm::vec3{ 0.0f, 1.0f, 0.0f }) const;

    /// @brief Get the ambient intensity
    inline const glm::vec3& ambient() const { return m_ambient; }

    /// @brief Get the diffuse intensity
    inline const glm::vec3& diffuse() const { return m_diffuse; }

    /// @brief Get the specular intensity
    inline const glm::vec3& specular() const { return m_specular; }
};