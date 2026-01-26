#include "Scene.h"
#include <cmath>
#include <iostream>

Scene::Scene(int width, int height)
    : scrWidth(width), scrHeight(height), activeCamera(nullptr), quadVAO(0), gBufferShader(nullptr), lightingPassShader(nullptr)
{
    InitGBuffer();
    InitQuad();
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

void Scene::AddShape(SceneObject *shape, Shader *shader)
{
    objects.push_back({shape, shader});
}

void Scene::Draw()
{
    if (!activeCamera)
        return;

    // --- Deferred Shading ---
    if (gBufferShader && lightingPassShader)
    {
        // Save current clear color
        GLfloat clearColor[4];
        glGetFloatv(GL_COLOR_CLEAR_VALUE, clearColor);

        // 1. Geometry Pass: Render all geometric/color data to g-buffer
        // Clear g-buffer to black (0,0,0) so we can detect background (empty) pixels
        glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
        glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glm::mat4 projection = activeCamera->GetProjectionMatrix((float)scrWidth, (float)scrHeight);
        glm::mat4 view = activeCamera->GetViewMatrix();

        gBufferShader->use();
        gBufferShader->setMat4("projection", projection);
        gBufferShader->setMat4("view", view);

        for (auto &obj : objects)
        {
            if (obj.shape->useObjectColor)
            {
                gBufferShader->setBool("useObjectColor", true);
                gBufferShader->setVec3("objectColor", obj.shape->objectColor);
            }
            else
            {
                gBufferShader->setBool("useObjectColor", false);
            }
            obj.shape->Draw(*gBufferShader);
        }
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        // 2. Lighting Pass: Calculate lighting by iterating over screen filled quad
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glClearColor(clearColor[0], clearColor[1], clearColor[2], clearColor[3]); // Restore clear color
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        lightingPassShader->use();

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, gPosition);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, gNormal);
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, gAlbedoSpec);

        // Lighting
        lightingPassShader->setInt("numLights", (int)lights.size());
        for (size_t i = 0; i < lights.size(); ++i)
        {
            lights[i]->SetUniformsViewSpace(*lightingPassShader, (int)i, view);
        }

        // Fog
        lightingPassShader->setBool("fogEnabled", fogEnabled);
        lightingPassShader->setVec3("fogColor", fogColor);
        lightingPassShader->setFloat("fogStart", fogStart);
        lightingPassShader->setFloat("fogEnd", fogEnd);

        lightingPassShader->setInt("displayMode", gBufferDisplayMode);

        RenderQuad();

        // 2.5. Copy depth buffer to default framebuffer
        glBindFramebuffer(GL_READ_FRAMEBUFFER, gBuffer);
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
        glBlitFramebuffer(0, 0, scrWidth, scrHeight, 0, 0, scrWidth, scrHeight, GL_DEPTH_BUFFER_BIT, GL_NEAREST);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        return;
    }

    // --- Forward Shading (Fallback) ---
    glm::mat4 projection = activeCamera->GetProjectionMatrix((float)scrWidth, (float)scrHeight);
    glm::mat4 view = activeCamera->GetViewMatrix();

    for (auto &obj : objects)
    {
        Shader *shader = obj.shader;
        shader->use();

        shader->setMat4("projection", projection);
        shader->setMat4("view", view);
        shader->setVec3("viewPos", activeCamera->Position);

        // Lighting support
        shader->setInt("numLights", (int)lights.size());
        for (size_t i = 0; i < lights.size(); ++i)
        {
            lights[i]->SetUniforms(*shader, (int)i);
        }

        // Fog
        shader->setBool("fogEnabled", fogEnabled);
        shader->setVec3("fogColor", fogColor);
        shader->setFloat("fogStart", fogStart);
        shader->setFloat("fogEnd", fogEnd);

        if (obj.shape->useObjectColor)
        {
            shader->setBool("useObjectColor", true);
            shader->setVec3("objectColor", obj.shape->objectColor);
        }
        else
        {
            shader->setBool("useObjectColor", false);
        }

        obj.shape->Draw(*shader);
    }
}

void Scene::InitGBuffer()
{
    glGenFramebuffers(1, &gBuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);

    // - Position color buffer
    glGenTextures(1, &gPosition);
    glBindTexture(GL_TEXTURE_2D, gPosition);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, scrWidth, scrHeight, 0, GL_RGBA, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, gPosition, 0);

    // - Normal color buffer
    glGenTextures(1, &gNormal);
    glBindTexture(GL_TEXTURE_2D, gNormal);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, scrWidth, scrHeight, 0, GL_RGBA, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, gNormal, 0);

    // - Color + Specular color buffer
    glGenTextures(1, &gAlbedoSpec);
    glBindTexture(GL_TEXTURE_2D, gAlbedoSpec);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, scrWidth, scrHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, gAlbedoSpec, 0);

    // - Tell OpenGL which color attachments we'll use (of this framebuffer) for rendering
    unsigned int attachments[3] = {GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2};
    glDrawBuffers(3, attachments);

    // - Create and attach depth buffer (renderbuffer)
    glGenRenderbuffers(1, &rboDepth);
    glBindRenderbuffer(GL_RENDERBUFFER, rboDepth);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, scrWidth, scrHeight);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rboDepth);

    // - Finally check if framebuffer is complete
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cout << "Framebuffer not complete!" << std::endl;
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Scene::InitQuad()
{
    float quadVertices[] = {
        // positions        // texture Coords
        -1.0f,
        1.0f,
        0.0f,
        0.0f,
        1.0f,
        -1.0f,
        -1.0f,
        0.0f,
        0.0f,
        0.0f,
        1.0f,
        1.0f,
        0.0f,
        1.0f,
        1.0f,
        1.0f,
        -1.0f,
        0.0f,
        1.0f,
        0.0f,
    };
    // Setup plane VAO
    glGenVertexArrays(1, &quadVAO);
    glGenBuffers(1, &quadVBO);
    glBindVertexArray(quadVAO);
    glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)(3 * sizeof(float)));
}

void Scene::RenderQuad()
{
    if (quadVAO == 0)
        InitQuad();
    glBindVertexArray(quadVAO);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glBindVertexArray(0);
}

void Scene::SetDeferredShaders(Shader *gBuf, Shader *lightPass)
{
    gBufferShader = gBuf;
    lightingPassShader = lightPass;

    // Set samplers once
    lightingPassShader->use();
    lightingPassShader->setInt("gPosition", 0);
    lightingPassShader->setInt("gNormal", 1);
    lightingPassShader->setInt("gAlbedoSpec", 2);
}
