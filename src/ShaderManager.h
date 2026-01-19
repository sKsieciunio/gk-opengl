#pragma once

#include <string>
#include <unordered_map>
#include "Shader.h"

class ShaderManager
{
public:
    ~ShaderManager();

    // Loads a shader and stores it associated with 'name'. Returns the loaded shader.
    // If a shader with 'name' already exists, it returns the existing one without reloading.
    Shader *LoadShader(const std::string &name, const std::string &vertexPath, const std::string &fragmentPath);

    // Retrieves a stored shader by name. Returns nullptr if not found.
    Shader *GetShader(const std::string &name);

private:
    std::unordered_map<std::string, Shader *> shaders;
};
