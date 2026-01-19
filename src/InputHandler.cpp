#include "InputHandler.h"
#include "Scene.h"
#include <iostream>

float lastX = 500.0f;
float lastY = 400.0f;
bool firstMouse = true;

// Helper to get camera from window user pointer
Camera* getCamera(GLFWwindow* window) {
    Scene* scene = static_cast<Scene*>(glfwGetWindowUserPointer(window));
    if (scene) {
        return scene->GetCamera();
    }
    return nullptr;
}

void processInput(GLFWwindow *window, float deltaTime)
{
    Camera* camera = getCamera(window);
    if (!camera) return;

    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera->ProcessKeyboard(FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera->ProcessKeyboard(BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera->ProcessKeyboard(LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera->ProcessKeyboard(RIGHT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
        camera->ProcessKeyboard(UP, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
        camera->ProcessKeyboard(DOWN, deltaTime);
}

void framebuffer_size_callback(GLFWwindow *window, int width, int height)
{
    glViewport(0, 0, width, height);
}

void mouse_callback(GLFWwindow *window, double xposIn, double yposIn)
{
    Camera* camera = getCamera(window);
    if (!camera) return;

    float xpos = static_cast<float>(xposIn);
    float ypos = static_cast<float>(yposIn);

    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos;

    lastX = xpos;
    lastY = ypos;

    camera->ProcessMouseMovement(xoffset, yoffset);
}

void scroll_callback(GLFWwindow *window, double xoffset, double yoffset)
{
    Camera* camera = getCamera(window);
    if (!camera) return;

    camera->ProcessMouseScroll(static_cast<float>(yoffset));
}
