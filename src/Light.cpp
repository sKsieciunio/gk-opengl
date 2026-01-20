#include "Light.h"

// Base Light
Light::Light(glm::vec3 col) : color(col) {}

// Directional Light
DirectionalLight::DirectionalLight(glm::vec3 dir, glm::vec3 col)
    : Light(col), direction(dir)
{
}

void DirectionalLight::SetUniforms(Shader &shader, int index)
{
  std::string base = "lights[" + std::to_string(index) + "]";
  shader.setInt(base + ".type", 0); // DIRECTIONAL
  shader.setVec3(base + ".direction", direction);
  shader.setVec3(base + ".color", color);
  // Placeholder values for struct completeness in shader (if needed by some drivers, although conditional check should cover)
}

void DirectionalLight::SetUniformsViewSpace(Shader &shader, int index, const glm::mat4 &view)
{
  std::string base = "lights[" + std::to_string(index) + "]";
  shader.setInt(base + ".type", 0);
  // Transform direction to view space (ignore translation)
  glm::vec3 viewDir = glm::mat3(view) * direction;
  shader.setVec3(base + ".direction", viewDir);
  shader.setVec3(base + ".color", color);
}

// Point Light
PointLight::PointLight(glm::vec3 pos, glm::vec3 col)
    : Light(col), position(pos), constant(1.0f), linear(0.09f), quadratic(0.032f)
{
}

void PointLight::SetUniforms(Shader &shader, int index)
{
  std::string base = "lights[" + std::to_string(index) + "]";
  shader.setInt(base + ".type", 1); // POINT
  shader.setVec3(base + ".position", position);
  shader.setVec3(base + ".color", color);
  shader.setFloat(base + ".constant", constant);
  shader.setFloat(base + ".linear", linear);
  shader.setFloat(base + ".quadratic", quadratic);
}

void PointLight::SetUniformsViewSpace(Shader &shader, int index, const glm::mat4 &view)
{
  std::string base = "lights[" + std::to_string(index) + "]";
  shader.setInt(base + ".type", 1);
  // Transform position to view space
  glm::vec3 viewPos = glm::vec3(view * glm::vec4(position, 1.0f));
  shader.setVec3(base + ".position", viewPos);
  shader.setVec3(base + ".color", color);
  shader.setFloat(base + ".constant", constant);
  shader.setFloat(base + ".linear", linear);
  shader.setFloat(base + ".quadratic", quadratic);
}

// Spot Light
SpotLight::SpotLight(glm::vec3 pos, glm::vec3 dir, glm::vec3 col)
    : Light(col), position(pos), direction(dir),
      constant(1.0f), linear(0.09f), quadratic(0.032f),
      cutOff(glm::cos(glm::radians(12.5f))), outerCutOff(glm::cos(glm::radians(15.0f)))
{
}

void SpotLight::SetUniformsViewSpace(Shader &shader, int index, const glm::mat4 &view)
{
  std::string base = "lights[" + std::to_string(index) + "]";
  shader.setInt(base + ".type", 2);
  // Transform position and direction
  glm::vec3 viewPos = glm::vec3(view * glm::vec4(position, 1.0f));
  glm::vec3 viewDir = glm::mat3(view) * direction;

  shader.setVec3(base + ".position", viewPos);
  shader.setVec3(base + ".direction", viewDir);
  shader.setVec3(base + ".color", color);
  shader.setFloat(base + ".constant", constant);
  shader.setFloat(base + ".linear", linear);
  shader.setFloat(base + ".quadratic", quadratic);
  shader.setFloat(base + ".cutOff", cutOff);
  shader.setFloat(base + ".outerCutOff", outerCutOff);
}
void SpotLight::SetUniforms(Shader &shader, int index)
{
  std::string base = "lights[" + std::to_string(index) + "]";
  shader.setInt(base + ".type", 2); // SPOT
  shader.setVec3(base + ".position", position);
  shader.setVec3(base + ".direction", direction);
  shader.setVec3(base + ".color", color);
  shader.setFloat(base + ".constant", constant);
  shader.setFloat(base + ".linear", linear);
  shader.setFloat(base + ".quadratic", quadratic);
  shader.setFloat(base + ".cutOff", cutOff);
  shader.setFloat(base + ".outerCutOff", outerCutOff);
}
