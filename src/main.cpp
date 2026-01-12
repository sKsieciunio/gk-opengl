#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>

#include "Shader.h"
#include "Camera.h"
#include "InputHandler.h"
#include "sphereGenerator.h"

#include <vector>
#include <cmath>

const unsigned int SCR_WIDTH = 1000;
const unsigned int SCR_HEIGHT = 800;

Camera camera(glm::vec3(0.0f, 0.0f, 6.0f));

glm::vec3 lightPos(6.0f, 5.0f, 0.0f);
glm::vec3 lightColor(1.0f, 1.0f, 1.0f);
glm::vec3 sphereColor(1.0f, 0.5f, 0.31f);

float deltaTime = 0.0f;
float lastFrame = 0.0f;

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

    Shader colorShader("shaders/color.vs.glsl", "shaders/color.fs.glsl");
    Shader normalShader("shaders/normal.vs.glsl", "shaders/normal.fs.glsl");
    Shader phongShader("shaders/phong.vs.glsl", "shaders/phong.fs.glsl");
    float vertices1[] = {
        // positions      // colors
        1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f,   // v0 - red
        -1.0f, -1.0f, 1.0f, 0.0f, 1.0f, 0.0f, // v1 - green
        -1.0f, 1.0f, -1.0f, 0.0f, 0.0f, 1.0f, // v2 - blue
        1.0f, -1.0f, -1.0f, 1.0f, 1.0f, 0.0f  // v3 - yellow
    };

    unsigned int indices1[] = {
        0, 1, 2,
        0, 3, 1,
        0, 2, 3,
        1, 3, 2};

    unsigned int VAO1, VBO1, EBO1;
    glGenVertexArrays(1, &VAO1);
    glGenBuffers(1, &VBO1);
    glGenBuffers(1, &EBO1);

    glBindVertexArray(VAO1);

    glBindBuffer(GL_ARRAY_BUFFER, VBO1);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices1), vertices1, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO1);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices1), indices1, GL_STATIC_DRAW);

    // position
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);
    // color
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);

    // Sphere data
    std::vector<float> sphereVertices;
    std::vector<unsigned int> sphereIndices;
    generateSphere(1.0f, 36, sphereVertices, sphereIndices);

    unsigned int VAO2, VBO2, EBO2;
    glGenVertexArrays(1, &VAO2);
    glGenBuffers(1, &VBO2);
    glGenBuffers(1, &EBO2);

    glBindVertexArray(VAO2);

    glBindBuffer(GL_ARRAY_BUFFER, VBO2);
    glBufferData(GL_ARRAY_BUFFER, sphereVertices.size() * sizeof(float), sphereVertices.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO2);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sphereIndices.size() * sizeof(unsigned int), sphereIndices.data(), GL_STATIC_DRAW);

    // position
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);
    // normal
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *)(3 * sizeof(float)));
    glEnableVertexAttribArray(2);

    glBindVertexArray(0);

    while (!glfwWindowShouldClose(window))
    {
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        processInput(window);

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        glm::mat4 view = camera.GetViewMatrix();
        glm::mat4 model = glm::mat4(1.0f);

        // tetrahedron
        colorShader.use();
        colorShader.setMat4("projection", projection);
        colorShader.setMat4("view", view);

        model = glm::rotate(model, (float)glfwGetTime(), glm::vec3(0.5f, 1.0f, 0.0f));
        colorShader.setMat4("model", model);

        glBindVertexArray(VAO1);
        glDrawElements(GL_TRIANGLES, 12, GL_UNSIGNED_INT, 0);

        // Sphere
        normalShader.use();
        normalShader.setMat4("projection", projection);
        normalShader.setMat4("view", view);

        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(3.0f, 0.0f, 0.0f));
        normalShader.setMat4("model", model);

        glBindVertexArray(VAO2);
        glDrawElements(GL_TRIANGLES, (GLsizei)sphereIndices.size(), GL_UNSIGNED_INT, 0);

        // second sphere
        phongShader.use();
        phongShader.setMat4("projection", projection);
        phongShader.setMat4("view", view);

        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(6.0f, 0.0f, 0.0f));
        phongShader.setMat4("model", model);
        phongShader.setVec3("objectColor", sphereColor);
        phongShader.setVec3("lightColor", lightColor);
        phongShader.setVec3("lightPos", lightPos + 3.0f * glm::vec3(sin(glfwGetTime()), 0.0f, cos(glfwGetTime())));
        phongShader.setVec3("viewPos", camera.Position);

        glBindVertexArray(VAO2);
        glDrawElements(GL_TRIANGLES, (GLsizei)sphereIndices.size(), GL_UNSIGNED_INT, 0);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteVertexArrays(1, &VAO1);
    glDeleteBuffers(1, &VBO1);
    glDeleteBuffers(1, &EBO1);

    glDeleteVertexArrays(1, &VAO2);
    glDeleteBuffers(1, &VBO2);
    glDeleteBuffers(1, &EBO2);

    glfwTerminate();
    return 0;
}
