#pragma once

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>
#include "Shader.h"

struct Vertex
{
    glm::vec3 Position;
    glm::vec3 Color;
    glm::vec3 Normal;
    glm::vec2 TexCoords;
};

class SceneObject
{
public:
    SceneObject(const std::vector<Vertex> &vertices, const std::vector<unsigned int> &indices);
    virtual ~SceneObject();

    void Draw(const Shader &shader);

    glm::mat4 GetModelMatrix() const;
    void SetPosition(const glm::vec3 &pos);
    void SetRotation(float angle, const glm::vec3 &axis);
    void SetScale(const glm::vec3 &scl);
    void SetObjectColor(const glm::vec3 &color, bool useColor = true);

    glm::vec3 position;
    float rotationAngle;
    glm::vec3 rotationAxis;
    glm::vec3 scale;

    bool useObjectColor = false;
    glm::vec3 objectColor = glm::vec4(1.0f);

private:
    unsigned int VAO, VBO, EBO;
    unsigned int indexCount;
};
