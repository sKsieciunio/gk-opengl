#include "Shape.h"
#include <cstddef> // for offsetof

SceneObject::SceneObject(const std::vector<Vertex> &vertices, const std::vector<unsigned int> &indices)
    : position(0.0f), rotationAngle(0.0f), rotationAxis(0.0f, 1.0f, 0.0f), scale(1.0f)
{
    indexCount = static_cast<unsigned int>(indices.size());

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)offsetof(Vertex, Position));

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)offsetof(Vertex, Color));

    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)offsetof(Vertex, Normal));

    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)offsetof(Vertex, TexCoords));

    glBindVertexArray(0);
}

SceneObject::~SceneObject()
{
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
}

void SceneObject::Draw(const Shader &shader)
{
    shader.setMat4("model", GetModelMatrix());

    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

glm::mat4 SceneObject::GetModelMatrix() const
{
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, position);
    model = glm::rotate(model, rotationAngle, rotationAxis);
    model = glm::scale(model, scale);
    return model;
}

void SceneObject::SetPosition(const glm::vec3 &pos)
{
    position = pos;
}

void SceneObject::SetRotation(float angle, const glm::vec3 &axis)
{
    rotationAngle = angle;
    rotationAxis = axis;
}

void SceneObject::SetScale(const glm::vec3 &scl)
{
    scale = scl;
}

void SceneObject::SetObjectColor(const glm::vec3 &color, bool useColor)
{
    objectColor = color;
    useObjectColor = useColor;
}
