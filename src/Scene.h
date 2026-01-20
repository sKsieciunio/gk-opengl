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

    void Draw();
    void AddShape(SceneObject *shape, Shader *shader);
    void AddLight(Light *light);
    void AddCamera(Camera *camera);
    void SetActiveCamera(int index);

    void SetDeferredShaders(Shader *gBuf, Shader *lightPass);

    Camera *GetActiveCamera() { return activeCamera; }

    const std::vector<Light *> &GetLights() const { return lights; }

    // Fog settings
    bool fogEnabled = false;
    glm::vec3 fogColor = glm::vec3(0.5f, 0.5f, 0.5f);
    float fogStart = 2.0f;
    float fogEnd = 20.0f;

private:
    Camera *activeCamera;
    std::vector<Camera *> cameras;
    std::vector<Light *> lights;

    int scrWidth, scrHeight;

    struct RenderObject
    {
        SceneObject *shape;
        Shader *shader;
    };
    std::vector<RenderObject> objects;

    // Deferred Shading
    unsigned int gBuffer;
    unsigned int gPosition, gNormal, gAlbedoSpec;
    unsigned int rboDepth;
    Shader *gBufferShader;
    Shader *lightingPassShader;

    unsigned int quadVAO = 0;
    unsigned int quadVBO;

    void InitGBuffer();
    void InitQuad();
    void RenderQuad();
};
