#include "Scene.h"
#include <cmath>

Scene::Scene(int width, int height)
    : scrWidth(width), scrHeight(height), activeCamera(nullptr)
{
    // Create default camera
}

Scene::~Scene()
{
    for (auto &cam : cameras)
    {
        delete cam;
    }
    for (auto &light : lights)
    {
        delete light;
    }
    for (auto &obj : objects)
    {
        delete obj.shape;
    }
}

void Scene::AddCamera(Camera *camera)
{
    cameras.push_back(camera);
    if (!activeCamera)
    {
        activeCamera = camera;
    }
}

void Scene::SetActiveCamera(int index)
{
    if (index >= 0 && index < cameras.size())
    {
        activeCamera = cameras[index];
    }
}

void Scene::AddLight(Light *light)
{
    lights.push_back(light);
}

void Scene::AddShape(Shape *shape, Shader *shader)
{
    objects.push_back({shape, shader});
}

void Scene::Update(float deltaTime)
{
    // Example: animate first light if it exists
    if (!lights.empty())
    {
        lights[0]->position = glm::vec3(6.0f, 5.0f, 0.0f) + 3.0f * glm::vec3(sin(glfwGetTime()), 0.0f, cos(glfwGetTime()));
    }
}

void Scene::Draw()
{
    if (!activeCamera)
        return;

    glm::mat4 projection = activeCamera->GetProjectionMatrix((float)scrWidth, (float)scrHeight);
    glm::mat4 view = activeCamera->GetViewMatrix();

    for (auto &obj : objects)
    {
        Shader *shader = obj.shader;
        shader->use();

        shader->setMat4("projection", projection);
        shader->setMat4("view", view);
        shader->setVec3("viewPos", activeCamera->Position);

        // Lighting support (Single light backward compatibility for now)
        if (!lights.empty())
        {
            shader->setVec3("lightPos", lights[0]->position);
            shader->setVec3("lightColor", lights[0]->color);
        }
        else
        {
            // Default fallback if no lights added
            shader->setVec3("lightPos", glm::vec3(0.0f));
            shader->setVec3("lightColor", glm::vec3(1.0f));
        }

        shader->setVec3("objectColor", glm::vec3(1.0f, 0.5f, 0.31f));

        obj.shape->Draw(*shader);
    }
}
