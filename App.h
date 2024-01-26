#pragma once
#include <opencv2\opencv.hpp>
#include "syncedQueue.h"
#include <GL/glew.h>
#include <GL/wglew.h>
#include <GLFW/glfw3.h>
#include <vector>
#include "Vertex.h"
#include <unordered_map>
#include "Mesh.h"
#include "Camera.h"
#include <unordered_set>
#include "syncedQueue.h"

class App {
public:
    App();
    bool init(void);
    int run(void);
    cv::Point2f findCenterNormalized(cv::Mat& frame);
    ~App();

    int runGraphics();
    std::unordered_set<int> keys;
    void updateProjectionMatrix(void);
    double cursorLastX = 0;
    double cursorLastY = 0;
    static void errorCallback(int error, const char* description);
    static void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
    static void FrameBufferSizeCallback(GLFWwindow* window, int width, int height);
    static void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
    static void cursorPositionCallback(GLFWwindow* window, double xpos, double ypos);
    static void scrollCallback(GLFWwindow* window, double xoffset, double yoffset);

private:
    void cameraThreadCode(void);

    cv::VideoCapture capture;
    synced_deque<cv::Point2f> normalizedCenterQueue;
    std::atomic<bool> end;
    
    GLFWwindow* window;
    int width, height;
    int fpsCount = 0;
    double lastFrameTime = 0;
    float fov = 60.0f;
    glm::mat4 projectionMatrix;
    std::unordered_map<std::string, Mesh> scene;
    Camera camera = Camera(glm::vec3(0.0, 2.0f, 0.0));
    std::chrono::steady_clock::time_point start;
    ShaderProgram* shader;
    void initGlew(void);
    void showGlData();
    void enableGlDebug();
    void setGlfwCallback(GLFWwindow* window);
    void updateCamera();
    void measureFPS();
    bool initAssets(std::unordered_map<std::string, Mesh>& scene);
    void updateDynamicModelMatrices();
    void drawScene();
    void sendUniformLights(ShaderProgram shader);
    glm::vec3 calculateCollisions(glm::vec3 position, glm::vec3 offset, std::unordered_map<std::string, Mesh>& scene);
};

