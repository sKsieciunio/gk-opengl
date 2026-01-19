#pragma once
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "Scene.h"

class InputHandler
{
public:
    InputHandler(GLFWwindow *window, Scene *scene);

    // Updates input state, handles keys (movement, toggles)
    void ProcessInput(float deltaTime);

    // Static callbacks to be registered with GLFW
    static void MouseCallback(GLFWwindow *window, double xpos, double ypos);
    static void ScrollCallback(GLFWwindow *window, double xoffset, double yoffset);
    static void FramebufferSizeCallback(GLFWwindow *window, int width, int height);

private:
    GLFWwindow *window;
    Scene *scene;

    bool firstMouse;
    float lastX, lastY;
    bool cursorLocked;
    bool lastTabState;

    void processMouse(double xpos, double ypos);
    void processScroll(double yoffset);
};
