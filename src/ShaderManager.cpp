#include "ShaderManager.h"
#include <iostream>

ShaderManager::~ShaderManager()
{
    for (auto &pair : shaders)
    {
        delete pair.second;
    }
    shaders.clear();
}

Shader *ShaderManager::LoadShader(const std::string &name, const std::string &vertexPath, const std::string &fragmentPath)
{
    if (shaders.find(name) != shaders.end())
    {
        return shaders[name];
    }

    Shader *shader = new Shader(vertexPath.c_str(), fragmentPath.c_str());
    shaders[name] = shader;

    return shader;
}

Shader *ShaderManager::GetShader(const std::string &name)
{
    if (shaders.find(name) != shaders.end())
    {
        return shaders[name];
    }
    return nullptr;
}
