#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <vector>

#include "Scene.h"
#include "Shape.h"
#include "Shader.h"
#include "ShaderManager.h"
#include "InputHandler.h"
#include "sphereGenerator.h"

const unsigned int SCR_WIDTH = 1000;
const unsigned int SCR_HEIGHT = 800;

int main()
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow *window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "gk OpenGL", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    if (glewInit() != GLEW_OK)
    {
        std::cout << "Failed to initialize GLEW" << std::endl;
        return -1;
    }

    glEnable(GL_DEPTH_TEST);

    Scene scene(SCR_WIDTH, SCR_HEIGHT);
    glfwSetWindowUserPointer(window, &scene);

    ShaderManager shaderManager;
    Shader *colorShader = shaderManager.LoadShader("color", "shaders/color.vs.glsl", "shaders/color.fs.glsl");
    Shader *normalShader = shaderManager.LoadShader("normal", "shaders/normal.vs.glsl", "shaders/normal.fs.glsl");
    Shader *phongShader = shaderManager.LoadShader("phong", "shaders/phong.vs.glsl", "shaders/phong.fs.glsl");

    // tetrahedron
    std::vector<Vertex> vertices_tetrahedron = {
        // position         // color          // normal       // texcoords
        {{1.0f, 1.0f, 1.0f}, {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f}},    // v0 - red
        {{-1.0f, -1.0f, 1.0f}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f}},  // v1 - green
        {{-1.0f, 1.0f, -1.0f}, {0.0f, 0.0f, 1.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f}},  // v2 - blue
        {{1.0f, -1.0f, -1.0f}, {1.0f, 1.0f, 0.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f}}}; // v3 - yellow

    std::vector<unsigned int> indices_tetrahedron{
        0, 1, 2,
        0, 3, 1,
        0, 2, 3,
        1, 3, 2};

    Shape *tetrahedron = new Shape(vertices_tetrahedron, indices_tetrahedron);
    scene.AddShape(tetrahedron, colorShader);

    // spheres
    std::vector<Vertex> sphereVertices;
    std::vector<unsigned int> sphereIndices;
    generateSphere(1.0f, 36, sphereVertices, sphereIndices);

    // Sphere 1 (Normal Visualization)
    Shape *sphere1 = new Shape(sphereVertices, sphereIndices);
    sphere1->SetPosition(glm::vec3(3.0f, 0.0f, 0.0f));
    scene.AddShape(sphere1, normalShader);

    // Sphere 2 (Phong Shading)
    Shape *sphere2 = new Shape(sphereVertices, sphereIndices);
    sphere2->SetPosition(glm::vec3(6.0f, 0.0f, 0.0f));
    scene.AddShape(sphere2, phongShader);

    // Main Loop
    float deltaTime = 0.0f;
    float lastFrame = 0.0f;

    while (!glfwWindowShouldClose(window))
    {
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        processInput(window, deltaTime);

        // TODO: maybe get rid of this
        scene.Update(deltaTime);

        tetrahedron->SetRotation((float)glfwGetTime(), glm::vec3(0.5f, 1.0f, 0.0f));

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        scene.Draw();

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();

    return 0;
}
