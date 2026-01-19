#pragma once

#include <vector>
#include <cmath>
#include <glm/glm.hpp>
#include "Shape.h" // For Vertex struct

void generateSphere(float radius, int resolution, std::vector<Vertex> &vertices, std::vector<unsigned int> &indices)
{
    int sectors = resolution;
    int stacks = resolution / 2;

    float x, y, z, xz;                           // vertex position
    float nx, ny, nz, lengthInv = 1.0f / radius; // vertex normal
    float s, t;                                  // vertex texCoord

    float sectorStep = 2 * 3.14159f / sectors;
    float stackStep = 3.14159f / stacks;
    float sectorAngle, stackAngle;

    for (int i = 0; i <= stacks; ++i)
    {
        stackAngle = 3.14159f / 2 - i * stackStep;
        xz = radius * cosf(stackAngle);
        y = radius * sinf(stackAngle);

        for (int j = 0; j <= sectors; ++j)
        {
            sectorAngle = j * sectorStep;

            x = xz * cosf(sectorAngle);
            z = xz * sinf(sectorAngle);

            nx = x * lengthInv;
            ny = y * lengthInv;
            nz = z * lengthInv;

            s = (float)j / sectors;
            t = (float)i / stacks;

            Vertex v;
            v.Position = glm::vec3(x, y, z);
            v.Color = glm::vec3(1.0f); // Default white
            v.Normal = glm::vec3(nx, ny, nz);
            v.TexCoords = glm::vec2(s, t);

            vertices.push_back(v);
        }
    }

    int k1, k2;
    for (int i = 0; i < stacks; ++i)
    {
        k1 = i * (sectors + 1);
        k2 = k1 + sectors + 1;

        for (int j = 0; j < sectors; ++j, ++k1, ++k2)
        {
            if (i != 0)
            {
                indices.push_back(k1);
                indices.push_back(k2);
                indices.push_back(k1 + 1);
            }

            if (i != (stacks - 1))
            {
                indices.push_back(k1 + 1);
                indices.push_back(k2);
                indices.push_back(k2 + 1);
            }
        }
    }
}
