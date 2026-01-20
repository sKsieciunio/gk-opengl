#pragma once
#include <string>
#include <vector>
#include "Shape.h"

class ModelLoader
{
public:
    static SceneObject *LoadObj(const std::string &path);
};
