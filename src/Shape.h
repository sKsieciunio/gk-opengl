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

class Shape
{
public:
    Shape(const std::vector<Vertex> &vertices, const std::vector<unsigned int> &indices);
    virtual ~Shape();

    void Draw(const Shader &shader);

    glm::mat4 GetModelMatrix() const;
    void SetPosition(const glm::vec3 &pos);
    void SetRotation(float angle, const glm::vec3 &axis);
    void SetScale(const glm::vec3 &scl);

    glm::vec3 position;
    float rotationAngle;
    glm::vec3 rotationAxis;
    glm::vec3 scale;

private:
    unsigned int VAO, VBO, EBO;
    unsigned int indexCount;
};
