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
#include "ModelLoader.h"
#include "cubeGenerator.h"

const unsigned int SCR_WIDTH = 1280;
const unsigned int SCR_HEIGHT = 720;

int main()
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow *window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "gk OpenGL Project", NULL, NULL);
    if (window == NULL)
    {
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);

    if (glewInit() != GLEW_OK)
        return -1;

    glEnable(GL_DEPTH_TEST);

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330");

    Scene scene(SCR_WIDTH, SCR_HEIGHT);
    InputHandler inputHandler(window, &scene);
    glfwSetWindowUserPointer(window, &inputHandler);

    glfwSetCursorPosCallback(window, InputHandler::MouseCallback);
    glfwSetScrollCallback(window, InputHandler::ScrollCallback);
    glfwSetFramebufferSizeCallback(window, InputHandler::FramebufferSizeCallback);

    ShaderManager shaderManager;
    Shader *phongShader = shaderManager.LoadShader("phong", "shaders/phong.vs.glsl", "shaders/phong.fs.glsl");

    // Deferred Shading Setup
    Shader *gbufferShader = shaderManager.LoadShader("gbuffer", "shaders/gbuffer.vs.glsl", "shaders/gbuffer.fs.glsl");
    Shader *lightingPassShader = shaderManager.LoadShader("lighting_pass", "shaders/lighting_pass.vs.glsl", "shaders/lighting_pass.fs.glsl");

    // Configure samplers for lighting pass (texture unit indices)
    lightingPassShader->use();
    lightingPassShader->setInt("gPosition", 0);
    lightingPassShader->setInt("gNormal", 1);
    lightingPassShader->setInt("gAlbedoSpec", 2);

    scene.SetDeferredShaders(gbufferShader, lightingPassShader);

    // --- Cameras ---
    Camera *camStatic = new Camera(glm::vec3(0.0f, 15.0f, 25.0f));
    camStatic->Pitch = -30.0f;
    camStatic->updateCameraVectors();
    scene.AddCamera(camStatic);

    Camera *camTracking = new Camera(glm::vec3(15.0f, 8.0f, 15.0f));
    scene.AddCamera(camTracking);

    Camera *camAttached = new Camera(glm::vec3(0.0f));
    scene.AddCamera(camAttached);

    int currentCamIdx = 0;

    // --- Lights ---
    // Spotlights (Headlights)
    SpotLight *leftHeadlight = new SpotLight(glm::vec3(0.0f), glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(1.0f, 0.9f, 0.8f));
    leftHeadlight->cutOff = glm::cos(glm::radians(12.5f));
    leftHeadlight->outerCutOff = glm::cos(glm::radians(17.5f));
    scene.AddLight(leftHeadlight);

    SpotLight *rightHeadlight = new SpotLight(glm::vec3(0.0f), glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(1.0f, 0.9f, 0.8f));
    rightHeadlight->cutOff = glm::cos(glm::radians(12.5f));
    rightHeadlight->outerCutOff = glm::cos(glm::radians(17.5f));
    scene.AddLight(rightHeadlight);

    DirectionalLight *sunLight = new DirectionalLight(glm::normalize(glm::vec3(-0.5f, -1.0f, -0.5f)), glm::vec3(0.5f));
    scene.AddLight(sunLight);

    PointLight *pointLight = new PointLight(glm::vec3(0.0f, 10.0f, 0.0f), glm::vec3(0.8f, 0.8f, 1.0f));
    scene.AddLight(pointLight);

    std::vector<Vertex> sphereV;
    std::vector<unsigned int> sphereI;
    generateSphere(1.0f, 36, sphereV, sphereI);

    SceneObject *sphere = new SceneObject(sphereV, sphereI);
    sphere->SetPosition(glm::vec3(0.0f, 3.0f, 0.0f));
    sphere->SetScale(glm::vec3(2.0f));
    // sphere->SetObjectColor(glm::vec3(0.2f, 0.2f, 0.7f), true); // Red
    scene.AddShape(sphere, phongShader);

    // floor
    std::vector<Vertex> cubeV;
    std::vector<unsigned int> cubeI;
    generateCube(1.0f, cubeV, cubeI);

    SceneObject *floor = new SceneObject(cubeV, cubeI);
    floor->SetPosition(glm::vec3(0.0f, -0.1f, 0.0f)); // Just below 0
    floor->SetScale(glm::vec3(40.0f, 0.1f, 40.0f));
    floor->SetObjectColor(glm::vec3(0.5f, 0.9f, 0.5f), true); // Greenish
    scene.AddShape(floor, phongShader);

    SceneObject *carModel = ModelLoader::LoadObj("models/Porsche_911_GT2.obj");
    if (!carModel)
    {
        // Fallback or exit
        std::cout << "Failed to load car model!" << std::endl;
        return -1;
    }
    carModel->SetPosition(glm::vec3(0.0f, 0.0f, 0.0f));
    carModel->SetScale(glm::vec3(1.0f));                         // Adjust if needed
    carModel->SetObjectColor(glm::vec3(1.0f, 0.2f, 0.2f), true); // Red
    scene.AddShape(carModel, phongShader);

    float timeOfDay = 0.5f;

    // Headlight calibration
    // Adjusted for car model (Front is likely -Z)
    glm::vec3 hlOffsetLeft(-0.6f, 0.8f, -2.2f);
    glm::vec3 hlOffsetRight(0.6f, 0.8f, -2.2f);

    // Angles in degrees
    float hlPitch = 10.0f; // Downward
    float hlToe = 2.0f;    // Inward

    float deltaTime = 0.0f;
    float lastTime = 0.0f;
    while (!glfwWindowShouldClose(window))
    {
        float time = (float)glfwGetTime();
        deltaTime = time - lastTime;
        lastTime = time;

        inputHandler.ProcessInput(deltaTime);

        glm::vec3 carPos(0.0f);
        glm::vec3 carFront(0.0f, 0.0f, 1.0f);
        float carRotAngle = 0.0f;

        float angle = time * 0.5f;
        float radius = 15.0f; // Bigger circle for car
        // Y position raised to 0.55f to sit on wheels above floor
        carPos = glm::vec3(sin(angle) * radius, 0.55f, cos(angle) * radius);
        carFront = glm::normalize(glm::vec3(cos(angle), 0.0f, -sin(angle)));

        // Car rotation
        // Add 180 degrees (PI) because model faces -Z (Forward) but standard atan2 aligns +Z
        carRotAngle = atan2(carFront.x, carFront.z) + glm::radians(180.0f);

        carModel->SetPosition(carPos);
        carModel->SetRotation(carRotAngle, glm::vec3(0.0f, 1.0f, 0.0f));

        // --- Update Headlights ---
        // Construct rotation matrix for the car
        glm::mat4 carMat = glm::rotate(glm::mat4(1.0f), carModel->rotationAngle, carModel->rotationAxis);

        // Base forward vector (-Z)
        glm::vec4 baseForward(0.0f, 0.0f, -1.0f, 0.0f);

        // Pitch rotation (Around X axis) - Positive pitch = down (local convention here)
        glm::mat4 pitchRot = glm::rotate(glm::mat4(1.0f), glm::radians(-hlPitch), glm::vec3(1.0f, 0.0f, 0.0f));

        // Toe rotation (Around Y axis)
        // Right light: Toe-in = rotate left (+angle around Y)
        // Left light: Toe-in = rotate right (-angle around Y)
        glm::mat4 toeLeftRot = glm::rotate(glm::mat4(1.0f), glm::radians(-hlToe), glm::vec3(0.0f, 1.0f, 0.0f));
        glm::mat4 toeRightRot = glm::rotate(glm::mat4(1.0f), glm::radians(hlToe), glm::vec3(0.0f, 1.0f, 0.0f));

        // Combined Direction Vectors (still in Car Space)
        glm::vec3 dirLeftLocal = glm::vec3(toeLeftRot * pitchRot * baseForward);
        glm::vec3 dirRightLocal = glm::vec3(toeRightRot * pitchRot * baseForward);

        // Transform to World Space
        glm::vec3 worldDirLeft = glm::normalize(glm::vec3(carMat * glm::vec4(dirLeftLocal, 0.0f)));
        glm::vec3 worldDirRight = glm::normalize(glm::vec3(carMat * glm::vec4(dirRightLocal, 0.0f)));

        // Calculate World Positions of headlights
        glm::vec3 leftPos = carPos + glm::vec3(carMat * glm::vec4(hlOffsetLeft, 1.0f));
        glm::vec3 rightPos = carPos + glm::vec3(carMat * glm::vec4(hlOffsetRight, 1.0f));

        leftHeadlight->position = leftPos;
        leftHeadlight->direction = worldDirLeft;

        rightHeadlight->position = rightPos;
        rightHeadlight->direction = worldDirRight;

        // Cameras
        // Tracking Cam: Looks at car
        camTracking->LookAt(carPos);

        // Attached Cam: Behind the car
        // Offset backward from carFront.
        // If carFront is the tangent, normalize(carFront) is forward direction.
        glm::vec3 camOffset = glm::normalize(carFront) * 8.0f;                    // 8 units behind
        camAttached->Position = carPos - camOffset + glm::vec3(0.0f, 3.0f, 0.0f); // Up a bit
        camAttached->LookAt(carPos + glm::vec3(0.0f, 1.0f, 0.0f));                // Look slightly above center

        scene.SetActiveCamera(currentCamIdx);

        // Env
        glm::vec3 nightColor(0.05f, 0.05f, 0.1f);
        glm::vec3 dayColor(0.6f, 0.8f, 1.0f);
        glm::vec3 clearCol = glm::mix(nightColor, dayColor, sin(timeOfDay * 3.14159f));
        sunLight->color = glm::vec3(sin(timeOfDay * 3.14159f));

        // Sync fog color with environment
        scene.fogColor = clearCol;

        if (scene.fogEnabled)
        {
            glClearColor(scene.fogColor.r, scene.fogColor.g, scene.fogColor.b, 1.0f);
        }
        else
        {
            glClearColor(clearCol.r, clearCol.g, clearCol.b, 1.0f);
        }

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        scene.Draw();

        ImGui::Begin("Controls");
        if (ImGui::CollapsingHeader("Cameras", ImGuiTreeNodeFlags_DefaultOpen))
        {
            const char *items[] = {"Static Observer", "Tracking", "Attached (TPP)"};
            ImGui::Combo("Camera Mode", &currentCamIdx, items, 3);

            Camera *activeCam = scene.GetActiveCamera();
            if (ImGui::Button("Perspective"))
                activeCam->Type = ProjectionType::Perspective;
            ImGui::SameLine();
            ImGui::Button("Orthographic");
            if (ImGui::IsItemClicked())
                activeCam->Type = ProjectionType::Orthographic;
            if (activeCam->Type == ProjectionType::Orthographic)
                ImGui::SliderFloat("Ortho Size", &activeCam->OrthoHeight, 1.0f, 50.0f);
            if (activeCam->Type == ProjectionType::Perspective)
                ImGui::SliderFloat("FOV", &activeCam->Zoom, 1.0f, 90.0f);
        }
        if (ImGui::CollapsingHeader("Deferred Shading"))
        {
            const char *modes[] = {"Combined Lighting", "Position (View Space)", "Normal (View Space)", "Albedo", "Specular"};
            ImGui::Combo("Display Mode", &scene.gBufferDisplayMode, modes, 5);
        }
        if (ImGui::CollapsingHeader("Environment"))
        {
            ImGui::SliderFloat("Time of Day", &timeOfDay, 0.0f, 1.0f);
            ImGui::Checkbox("Fog Enabled", &scene.fogEnabled);
            // ImGui::ColorEdit3("Fog Color", &scene.fogColor[0]); // Now automatic
            ImGui::SliderFloat("Fog Start", &scene.fogStart, 0.1f, 50.0f);
            ImGui::SliderFloat("Fog End", &scene.fogEnd, 20.0f, 200.0f);
        }
        if (ImGui::CollapsingHeader("Headlights Calibration"))
        {
            // ImGui::Text("Position Offsets (Local car space)");
            // ImGui::SliderFloat3("Left Offset", &hlOffsetLeft[0], -5.0f, 5.0f);
            // ImGui::SliderFloat3("Right Offset", &hlOffsetRight[0], -5.0f, 5.0f);

            ImGui::Text("Alignment");
            ImGui::SliderFloat("Pitch (Up/Down)", &hlPitch, -20.0f, 20.0f);
            ImGui::SliderFloat("Toe (In/Out)", &hlToe, -20.0f, 20.0f);

            ImGui::Text("Properties");
            ImGui::ColorEdit3("Color", &leftHeadlight->color[0]);
            // Sync colors
            rightHeadlight->color = leftHeadlight->color;

            // Cutoff
            float cutOffDeg = glm::degrees(acos(leftHeadlight->cutOff));
            if (ImGui::SliderFloat("Cutoff Angle", &cutOffDeg, 5.0f, 45.0f))
            {
                leftHeadlight->cutOff = glm::cos(glm::radians(cutOffDeg));
                if (leftHeadlight->outerCutOff > leftHeadlight->cutOff)
                    leftHeadlight->cutOff = leftHeadlight->outerCutOff;
                rightHeadlight->cutOff = leftHeadlight->cutOff;
            }
            float outerCutOffDeg = glm::degrees(acos(leftHeadlight->outerCutOff));
            if (ImGui::SliderFloat("Outer Cutoff Angle", &outerCutOffDeg, 5.0f, 60.0f))
            {
                leftHeadlight->outerCutOff = glm::cos(glm::radians(outerCutOffDeg));
                if (leftHeadlight->outerCutOff > leftHeadlight->cutOff)
                    leftHeadlight->outerCutOff = leftHeadlight->cutOff;
                rightHeadlight->outerCutOff = leftHeadlight->outerCutOff;
            }
        }
        ImGui::End();

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
