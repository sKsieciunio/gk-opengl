#pragma once
#include <glm/glm.hpp>
#include "Shader.h"

enum class LightType
{
    DIRECTIONAL = 0,
    POINT = 1,
    SPOT = 2
};

class Light
{
public:
    Light(glm::vec3 color);
    virtual ~Light() = default;

    virtual void SetUniforms(Shader &shader, int index) = 0;
    virtual void SetUniformsViewSpace(Shader &shader, int index, const glm::mat4 &view) = 0;

    glm::vec3 color;
};

class DirectionalLight : public Light
{
public:
    DirectionalLight(glm::vec3 direction, glm::vec3 color);
    void SetUniforms(Shader &shader, int index) override;
    void SetUniformsViewSpace(Shader &shader, int index, const glm::mat4 &view) override;

    glm::vec3 direction;
};

class PointLight : public Light
{
public:
    PointLight(glm::vec3 position, glm::vec3 color);
    void SetUniforms(Shader &shader, int index) override;
    void SetUniformsViewSpace(Shader &shader, int index, const glm::mat4 &view) override;

    glm::vec3 position;
    // Attenuation
    float constant;
    float linear;
    float quadratic;
};

class SpotLight : public Light
{
public:
    SpotLight(glm::vec3 position, glm::vec3 direction, glm::vec3 color);
    void SetUniformsViewSpace(Shader &shader, int index, const glm::mat4 &view) override;
    void SetUniforms(Shader &shader, int index) override;

    glm::vec3 position;
    glm::vec3 direction;
    // Attenuation
    float constant;
    float linear;
    float quadratic;
    // Spotlight factors
    float cutOff;
    float outerCutOff;
};
