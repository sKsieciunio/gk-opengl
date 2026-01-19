#include "Scene.h"
#include <cmath>

Scene::Scene(int width, int height)
    : scrWidth(width), scrHeight(height),
      lightPos(6.0f, 5.0f, 0.0f), lightColor(1.0f, 1.0f, 1.0f)
{
    camera = new Camera(glm::vec3(0.0f, 0.0f, 6.0f));
}

Scene::~Scene()
{
    delete camera;
    for (auto &obj : objects)
    {
        delete obj.shape;
        // Shaders are usually managed externally or shared, but for this simpler abstraction we won't delete them here unless we own them.
        // Assuming main owns shaders for now as they might be reused.
        // If Shapes are owned by Scene, we delete them.
    }
}

void Scene::AddShape(Shape *shape, Shader *shader)
{
    objects.push_back({shape, shader});
}

void Scene::Update(float deltaTime)
{
    lightPos = glm::vec3(6.0f, 5.0f, 0.0f) + 3.0f * glm::vec3(sin(glfwGetTime()), 0.0f, cos(glfwGetTime()));
}

void Scene::Draw()
{
    glm::mat4 projection = glm::perspective(glm::radians(camera->Zoom), (float)scrWidth / (float)scrHeight, 0.1f, 100.0f);
    glm::mat4 view = camera->GetViewMatrix();

    for (auto &obj : objects)
    {
        Shader *shader = obj.shader;
        shader->use();

        shader->setMat4("projection", projection);
        shader->setMat4("view", view);

        // Best effort to set uniforms if they exist
        // Note: setting uniforms that don't exist is usually ignored by OpenGL or flagged if strict.
        // Shader class wrapper usually handles locations quietly.

        // Setup lighting uniforms if this is the phong shader (or similar)
        // We can check shader ID or just try setting them.
        shader->setVec3("lightPos", lightPos);
        shader->setVec3("lightColor", lightColor);
        shader->setVec3("viewPos", camera->Position);

        // For simple object color, we might want to store it in Shape or pass it.
        // For now, let's hardcode a default if not set, or rely on the fact that some shaders don't use it.
        // To keep it simple as per request, the original main had a constant sphereColor.
        shader->setVec3("objectColor", glm::vec3(1.0f, 0.5f, 0.31f));

        obj.shape->Draw(*shader);
    }
}
