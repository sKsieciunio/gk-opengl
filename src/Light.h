#pragma once
#include <glm/glm.hpp>

enum class LightType
{
    Point,
    Directional,
    Spot
};

class Light
{
public:
    Light(glm::vec3 position, glm::vec3 color = glm::vec3(1.0f));

    glm::vec3 position;
    glm::vec3 color;
    float intensity;
    LightType type;

    // For directional lights, direction is needed instead of position sometimes,
    // or position effectively acts as direction for simple directional implementations.
    // For now we stick to point lights as per existing shader logic.
};
