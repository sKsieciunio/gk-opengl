#include "InputHandler.h"
#include "imgui.h"
#include "backend/imgui_impl_glfw.h"
#include <iostream>

InputHandler::InputHandler(GLFWwindow *window, Scene *scene)
    : window(window), scene(scene), firstMouse(true), cursorLocked(true), lastTabState(false)
{

    int width, height;
    glfwGetWindowSize(window, &width, &height);
    lastX = width / 2.0f;
    lastY = height / 2.0f;

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
}

void InputHandler::ProcessInput(float deltaTime)
{
    // 1. Always handle ESC to exit
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    // 2. Handle Cursor Lock Toggle (TAB)
    bool tabState = glfwGetKey(window, GLFW_KEY_TAB) == GLFW_PRESS;
    if (tabState && !lastTabState)
    {
        cursorLocked = !cursorLocked;
        glfwSetInputMode(window, GLFW_CURSOR, cursorLocked ? GLFW_CURSOR_DISABLED : GLFW_CURSOR_NORMAL);
    }
    lastTabState = tabState;

    // 3. Handle Movement only if cursor is locked
    if (cursorLocked)
    {
        Camera *camera = scene->GetActiveCamera();
        if (camera)
        {
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
    }
}

// --------------------------------------------------------------------------------
// Instance Methods for Callbacks
// --------------------------------------------------------------------------------

void InputHandler::processMouse(double xposIn, double yposIn)
{
    // 0. Always update ImGui
    // Note: If you have multiple windows, you might need to check if this window is the current context
    ImGui_ImplGlfw_CursorPosCallback(window, xposIn, yposIn);

    if (!cursorLocked)
        return;

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

    Camera *camera = scene->GetActiveCamera();
    if (camera)
    {
        camera->ProcessMouseMovement(xoffset, yoffset);
    }
}

void InputHandler::processScroll(double yoffset)
{
    // 0. Forward to ImGui
    // Note: ImGui scroll callback expects two doubles (xoffset, yoffset), but standard GLFW scroll callback gives them too.
    // Our processScroll only takes yoffset for camera logic, but the static callback has both.
    // Wait, the static callback calls this with just yoffset. We should fix the plumbing if we want full ImGui scroll support (horizontal).
    // For now, let's just forward y assuming x is 0, OR better, let's fix the instance method signature or call ImGui from static callback.
    // Actually, calling ImGui_ImplGlfw_ScrollCallback requires the window and both offsets.
    // Since we are inside the instance method, we don't have xoffset passed conveniently (it was dropped in the static wrapper).
    // Let's modify the static wrapper to handle ImGui dispatch.

    if (!cursorLocked)
        return;

    Camera *camera = scene->GetActiveCamera();
    if (camera)
    {
        camera->ProcessMouseScroll(static_cast<float>(yoffset));
    }
}

// --------------------------------------------------------------------------------
// Static Callback Wrappers
// --------------------------------------------------------------------------------

void InputHandler::MouseCallback(GLFWwindow *window, double xpos, double ypos)
{
    InputHandler *handler = static_cast<InputHandler *>(glfwGetWindowUserPointer(window));
    if (handler)
    {
        handler->processMouse(xpos, ypos);
    }
}

void InputHandler::ScrollCallback(GLFWwindow *window, double xoffset, double yoffset)
{
    ImGui_ImplGlfw_ScrollCallback(window, xoffset, yoffset);

    InputHandler *handler = static_cast<InputHandler *>(glfwGetWindowUserPointer(window));
    if (handler)
    {
        handler->processScroll(yoffset);
    }
}

void InputHandler::FramebufferSizeCallback(GLFWwindow *window, int width, int height)
{
    glViewport(0, 0, width, height);
}
