#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include "App.h"
#include <GLFW/glfw3.h>
#include <iostream>
#include <unordered_set>

void App::errorCallback(int error, const char* description)
{
    fprintf(stderr, "Error: %s\n", description);
}
bool vSync = true;
bool fullScreen = false;

void App::KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    auto app = static_cast<App*>(glfwGetWindowUserPointer(window));
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GLFW_TRUE);

    if (key == GLFW_KEY_F11 && action == GLFW_PRESS) {
        if (fullScreen) {
            glfwSetWindowMonitor(window, NULL, 0, 0, 640, 480, NULL);
            fullScreen = false;
            std::cout << "Full-screen: " << fullScreen << "\n";
        }
        else {
            const GLFWvidmode* mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
            glfwSetWindowMonitor(window, glfwGetPrimaryMonitor(), 0, 0, mode->width, mode->height, mode->refreshRate);
            fullScreen = true;
            std::cout << "Full-screen: " << fullScreen << "\n";
        }
        return;
    }

    if (key == GLFW_KEY_V && action == GLFW_PRESS) {
        glfwSwapInterval(vSync);
        std::cout << "V-Sync: " << vSync << "\n";
        vSync = !vSync;
        return;
    }
    if (action == GLFW_PRESS) {
        app->keys.insert(key);
    }
    else if (action == GLFW_RELEASE)
        app->keys.erase(key);

}
void App::FrameBufferSizeCallback(GLFWwindow* window, int width, int height) {
    auto app = static_cast<App*>(glfwGetWindowUserPointer(window));
    app->height = height;
    app->width = width;
    glViewport(0, 0, width, height);
    app->updateProjectionMatrix();
    std::cout << "window size changed width: " << width << " height: " << height << "\n";
}
void App::mouseButtonCallback(GLFWwindow* window, int button, int action, int mods) {
    
}
void App::cursorPositionCallback(GLFWwindow* window, double xpos, double ypos) {
    auto app = static_cast<App*>(glfwGetWindowUserPointer(window));
    app->camera.ProcessMouseMovement(xpos - app->cursorLastX, (ypos - app->cursorLastY) * -1);
    app->cursorLastX = xpos;
    app->cursorLastY = ypos;
}
void App::scrollCallback(GLFWwindow* window, double xoffset, double yoffset) {
    auto app = static_cast<App*>(glfwGetWindowUserPointer(window));
    app->fov -= 10.0f * yoffset;
    app->fov = std::clamp(app->fov, 20.0f, 170.0f);
    app->updateProjectionMatrix();
    std::cout << "scroll x: " << xoffset << " y: " << yoffset << "\n";
}
