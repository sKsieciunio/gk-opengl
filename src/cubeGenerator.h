#include <vector>
#include "Shape.h"

// Helper to generate a cube
void generateCube(float size, std::vector<Vertex> &vertices, std::vector<unsigned int> &indices)
{
    float s = size / 2.0f;
    // 24 vertices for hard edges
    float v[] = {
        // Front (Z+)
        -s, -s, s, 0, 0, 1, s, -s, s, 0, 0, 1, s, s, s, 0, 0, 1, -s, s, s, 0, 0, 1,
        // Back (Z-)
        -s, -s, -s, 0, 0, -1, -s, s, -s, 0, 0, -1, s, s, -s, 0, 0, -1, s, -s, -s, 0, 0, -1,
        // Left (X-)
        -s, -s, -s, -1, 0, 0, -s, -s, s, -1, 0, 0, -s, s, s, -1, 0, 0, -s, s, -s, -1, 0, 0,
        // Right (X+)
        s, -s, -s, 1, 0, 0, s, s, -s, 1, 0, 0, s, s, s, 1, 0, 0, s, -s, s, 1, 0, 0,
        // Top (Y+)
        -s, s, -s, 0, 1, 0, -s, s, s, 0, 1, 0, s, s, s, 0, 1, 0, s, s, -s, 0, 1, 0,
        // Bottom (Y-)
        -s, -s, -s, 0, -1, 0, s, -s, -s, 0, -1, 0, s, -s, s, 0, -1, 0, -s, -s, s, 0, -1, 0};
    unsigned int ind[] = {
        0, 1, 2, 0, 2, 3, 4, 5, 6, 4, 6, 7, 8, 9, 10, 8, 10, 11, 12, 13, 14, 12, 14, 15, 16, 17, 18, 16, 18, 19, 20, 21, 22, 20, 22, 23};

    vertices.clear();
    indices.clear();
    for (int i = 0; i < 24; i++)
    {
        Vertex vert;
        vert.Position = glm::vec3(v[i * 6 + 0], v[i * 6 + 1], v[i * 6 + 2]);
        vert.Normal = glm::vec3(v[i * 6 + 3], v[i * 6 + 4], v[i * 6 + 5]);
        vert.Color = glm::vec3(1.0f);
        vert.TexCoords = glm::vec2(0.0f);
        vertices.push_back(vert);
    }
    for (int i = 0; i < 36; i++)
        indices.push_back(ind[i]);
}