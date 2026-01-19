#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "imgui.h"
#include "backend/imgui_impl_glfw.h"
#include "backend/imgui_impl_opengl3.h"

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
    // Callbacks are now set via InputHandler below
    // glfwSetInputMode set by InputHandler constructor

    if (glewInit() != GLEW_OK)
    {
        std::cout << "Failed to initialize GLEW" << std::endl;
        return -1;
    }

    glEnable(GL_DEPTH_TEST);

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO();
    (void)io;
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330");

    Scene scene(SCR_WIDTH, SCR_HEIGHT);

    // Input Handler
    InputHandler inputHandler(window, &scene);
    glfwSetWindowUserPointer(window, &inputHandler);

    // Register callbacks
    glfwSetCursorPosCallback(window, InputHandler::MouseCallback);
    glfwSetScrollCallback(window, InputHandler::ScrollCallback);
    glfwSetFramebufferSizeCallback(window, InputHandler::FramebufferSizeCallback);

    Camera *camera = new Camera(glm::vec3(0.0f, 0.0f, 6.0f));
    scene.AddCamera(camera);

    Light *light = new Light(glm::vec3(6.0f, 5.0f, 0.0f));
    scene.AddLight(light);

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

        // Process Input via InputHandler (handled keys + toggles)
        inputHandler.ProcessInput(deltaTime);

        // TODO: maybe get rid of this
        scene.Update(deltaTime);

        tetrahedron->SetRotation((float)glfwGetTime(), glm::vec3(0.5f, 1.0f, 0.0f));

        // Start the Dear ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        ImGui::Begin("Camera Settings");
        Camera *activeCam = scene.GetActiveCamera();
        if (activeCam)
        {
            const char *projections[] = {"Perspective", "Orthographic"};
            int currentProj = (int)activeCam->Type;
            if (ImGui::Combo("Projection", &currentProj, projections, IM_ARRAYSIZE(projections)))
            {
                activeCam->Type = (ProjectionType)currentProj;
            }

            if (activeCam->Type == ProjectionType::Orthographic)
            {
                ImGui::SliderFloat("Ortho Size", &activeCam->OrthoHeight, 1.0f, 20.0f);
            }
            else
            {
                ImGui::SliderFloat("FOV", &activeCam->Zoom, 1.0f, 90.0f);
            }
        }
        ImGui::Text("Press TAB to toggle mouse cursor");
        ImGui::Text("Press ESC to exit");
        ImGui::End();

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        scene.Draw();

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwTerminate();

    return 0;
}
