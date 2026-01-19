#pragma once

#include <vector>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "Camera.h"
#include "Shape.h"
#include "Shader.h"

class Scene
{
public:
    Scene(int width, int height);
    ~Scene();

    void Update(float deltaTime);
    void Draw();
    void AddShape(Shape *shape, Shader *shader);

    Camera *GetCamera() { return camera; }

    glm::vec3 GetLightPos() { return lightPos; }

private:
    Camera *camera;
    int scrWidth, scrHeight;

    struct RenderObject
    {
        Shape *shape;
        Shader *shader;
    };
    std::vector<RenderObject> objects;

    glm::vec3 lightPos;
    glm::vec3 lightColor;
};
