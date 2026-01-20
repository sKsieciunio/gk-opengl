#include "ModelLoader.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <map>
#include <tuple>

// Helper to split string by delimiter
std::vector<std::string> split(const std::string &s, char delimiter)
{
    std::vector<std::string> tokens;
    std::string token;
    std::istringstream tokenStream(s);
    while (std::getline(tokenStream, token, delimiter))
    {
        tokens.push_back(token);
    }
    return tokens;
}

SceneObject *ModelLoader::LoadObj(const std::string &path)
{
    std::vector<glm::vec3> temp_positions;
    std::vector<glm::vec2> temp_texcoords;
    std::vector<glm::vec3> temp_normals;

    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;

    // Map to reuse vertices: (posIdx, texIdx, normIdx) -> newIndex
    std::map<std::tuple<int, int, int>, unsigned int> vertexMap;

    std::ifstream file(path);
    if (!file.is_open())
    {
        std::cerr << "Failed to open OBJ file: " << path << std::endl;
        return nullptr;
    }

    std::string line;
    while (std::getline(file, line))
    {
        if (line.substr(0, 2) == "v ")
        {
            std::istringstream s(line.substr(2));
            glm::vec3 v;
            s >> v.x >> v.y >> v.z;
            temp_positions.push_back(v);
        }
        else if (line.substr(0, 3) == "vt ")
        {
            std::istringstream s(line.substr(3));
            glm::vec2 v;
            s >> v.x >> v.y;
            temp_texcoords.push_back(v);
        }
        else if (line.substr(0, 3) == "vn ")
        {
            std::istringstream s(line.substr(3));
            glm::vec3 v;
            s >> v.x >> v.y >> v.z;
            temp_normals.push_back(v);
        }
        else if (line.substr(0, 2) == "f ")
        {
            // f v1/vt1/vn1 v2/vt2/vn2 v3/vt3/vn3 ...
            std::string content = line.substr(2);
            std::vector<std::string> faceTokens;
            std::istringstream contentStream(content);
            std::string temp;
            while (contentStream >> temp)
            {
                faceTokens.push_back(temp);
            }

            // Triangulate fan: 0, 1, 2;  0, 2, 3; ...
            for (size_t i = 1; i < faceTokens.size() - 1; ++i)
            {
                std::string faceVerts[3] = {faceTokens[0], faceTokens[i], faceTokens[i + 1]};

                for (const auto &fv : faceVerts)
                {
                    std::vector<std::string> indicesStr = split(fv, '/');

                    int pIdx = -1, tIdx = -1, nIdx = -1;

                    if (indicesStr.size() > 0 && !indicesStr[0].empty())
                        pIdx = std::stoi(indicesStr[0]) - 1;
                    if (indicesStr.size() > 1 && !indicesStr[1].empty())
                        tIdx = std::stoi(indicesStr[1]) - 1;
                    if (indicesStr.size() > 2 && !indicesStr[2].empty())
                        nIdx = std::stoi(indicesStr[2]) - 1;

                    auto key = std::make_tuple(pIdx, tIdx, nIdx);
                    if (vertexMap.find(key) == vertexMap.end())
                    {
                        Vertex vert;
                        // Position
                        if (pIdx >= 0 && pIdx < temp_positions.size())
                            vert.Position = temp_positions[pIdx];
                        else
                            vert.Position = glm::vec3(0.0f);

                        // Texture
                        if (tIdx >= 0 && tIdx < temp_texcoords.size())
                            vert.TexCoords = temp_texcoords[tIdx];
                        else
                            vert.TexCoords = glm::vec2(0.0f);

                        // Normal
                        if (nIdx >= 0 && nIdx < temp_normals.size())
                            vert.Normal = temp_normals[nIdx];
                        else
                            vert.Normal = glm::vec3(0.0f); // Should optionally compute if missing?

                        vert.Color = glm::vec3(0.8f); // Default grey color

                        unsigned int newIndex = (unsigned int)vertices.size();
                        vertices.push_back(vert);
                        indices.push_back(newIndex);
                        vertexMap[key] = newIndex;
                    }
                    else
                    {
                        indices.push_back(vertexMap[key]);
                    }
                }
            }
        }
    }

    // Safety check: if normals are missing (0,0,0), we could Recalculate them,
    // but for now let's leave it simple.

    std::cout << "Loaded OBJ: " << path << " with " << vertices.size() << " vertices and " << indices.size() << " indices." << std::endl;

    return new SceneObject(vertices, indices);
}
