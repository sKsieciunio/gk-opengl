#pragma once

#include <vector>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "Camera.h"
#include "Shape.h"
#include "Shader.h"
#include "Light.h"

class Scene
{
public:
    Scene(int width, int height);
    ~Scene();

    void Update(float deltaTime);
    void Draw();
    void AddShape(Shape *shape, Shader *shader);
    void AddLight(Light *light);
    void AddCamera(Camera *camera);
    void SetActiveCamera(int index);

    Camera *GetActiveCamera() { return activeCamera; }

    const std::vector<Light *> &GetLights() const { return lights; }

private:
    Camera *activeCamera;
    std::vector<Camera *> cameras;
    std::vector<Light *> lights;

    int scrWidth, scrHeight;

    struct RenderObject
    {
        Shape *shape;
        Shader *shader;
    };
    std::vector<RenderObject> objects;
};
