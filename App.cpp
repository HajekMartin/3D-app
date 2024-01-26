#include <iostream>
#include <atomic>
#include <chrono>
#include <GL/glew.h> 
#include <GL/wglew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include "glErrCallback.h";
#include "ShaderProgram.h"
#include "Mesh.h"
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "OBJloader.h"
#include "texture.h"
#include "App.h"

App::App()
{
}

bool App::init()
{
    try {
        capture = cv::VideoCapture(cv::CAP_DSHOW);

        if (!capture.isOpened())
        {
            std::cerr << "no source?" << std::endl;
            capture = cv::VideoCapture("video.mkv");
            if (!capture.isOpened())
            {
                std::cerr << "failed to load video" << std::endl;
                exit(EXIT_FAILURE);
            }
        }
        else
        {
            std::cout << "Source: " <<
                ": width=" << capture.get(cv::CAP_PROP_FRAME_WIDTH) <<
                ", height=" << capture.get(cv::CAP_PROP_FRAME_HEIGHT) << '\n';
        }
    }
    catch (std::exception const& e) {
        std::cerr << "Init failed : " << e.what() << std::endl;
        throw;
    }

    return true;
}
cv::Mat frame;
int App::run(void)
{

    try {
        std::thread cameraRead(&App::cameraThreadCode, this);
        runGraphics();
        end = true;
        cameraRead.join();

    }
    catch (std::exception const& e) {
        std::cerr << "App failed : " << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

App::~App()
{
    if (capture.isOpened())
        capture.release();
    cv::destroyAllWindows();
    std::cout << "Bye...\n";
}

void App::cameraThreadCode(void)
{
    cv::Mat frame;

    try {
        while (!end) {
            capture >> frame;

            if (frame.empty()) {
                throw std::exception("Empty file? Wrong path?");
            }
            cv::Point2f centerNormalized = findCenterNormalized(frame);
            
            normalizedCenterQueue.push_back(centerNormalized);

            int key = cv::pollKey();
            if (key == 27)
            {
                end = true;
                break;

            }

        }

    }
    catch (std::exception const& e) {
        std::cerr << "App failed : " << e.what() << std::endl;
    }
}

cv::Point2f App::findCenterNormalized(cv::Mat& frame)
{
    //zelená barva
    double h_low = 38.0;
    double s_low = 85;
    double v_low = 50.0;
    double h_hi = 79.0;
    double s_hi = 255.0;
    double v_hi = 255.0;


    cv::Mat scene_hsv, scene_threshold;

    cv::cvtColor(frame, scene_hsv, cv::COLOR_BGR2HSV);

    cv::Scalar lower_threshold = cv::Scalar(h_low, s_low, v_low);
    cv::Scalar upper_threshold = cv::Scalar(h_hi, s_hi, v_hi);
    cv::inRange(scene_hsv, lower_threshold, upper_threshold, scene_threshold);

    int nPoints = 0;
    int x_sum = 0;
    int y_sum = 0;

    for (int y = 0; y < frame.rows; y++) //y
    {
        for (int x = 0; x < frame.cols; x++) //x
        {
            cv::Vec3b pixel = frame.at<cv::Vec3b>(y, x);
            if (scene_threshold.at<unsigned char>(y, x) < 255) {

            }
            else {
                nPoints++;
                x_sum += x;
                y_sum += y;
            }

        }
    }
    cv::Point2f center;
    cv::Point2f centerNormalized;
    if (nPoints > 0)
    {
        float avgX = x_sum / nPoints;
        float avgY = y_sum / nPoints;
        center.x = avgX;
        center.y = avgY;
        centerNormalized.x = avgX / frame.cols;
        centerNormalized.y = avgY / frame.rows;
    }
    return centerNormalized;
}

void App::initGlew(void)
{
    {
        GLenum glew_ret;
        glew_ret = glewInit();
        if (glew_ret != GLEW_OK) {
            std::cerr << "WGLEW failed with error: " << glewGetErrorString(glew_ret) << std::endl;
            exit(EXIT_FAILURE);
        }
        else {
            std::cout << "GLEW successfully initialized to version: " << glewGetString(GLEW_VERSION) << std::endl;
        }

        glew_ret = wglewInit();
        if (glew_ret != GLEW_OK) {
            std::cerr << "WGLEW failed with error: " << glewGetErrorString(glew_ret) << std::endl;
            exit(EXIT_FAILURE);
        }
        else {
            std::cout << "WGLEW successfully initialized platform specific functions." << std::endl;
        }
    }
    {
        GLint n = 0;
        glGetIntegerv(GL_NUM_EXTENSIONS, &n);
        for (GLint i = 0; i < n; i++) {
            const char* extension_name = (const char*)glGetStringi(GL_EXTENSIONS, i);
            std::cout << extension_name << '\n';
        }
    }
}

void App::showGlData()
{
    GLenum sym_const[] = { GL_VENDOR, GL_RENDERER, GL_VERSION,
    GL_SHADING_LANGUAGE_VERSION };
    GLint profileMask;
    glGetIntegerv(GL_CONTEXT_PROFILE_MASK, &profileMask);
    if (profileMask & GL_CONTEXT_CORE_PROFILE_BIT)
        std::cout << "GL context CORE profile enabled\n";
    if (profileMask & GL_CONTEXT_COMPATIBILITY_PROFILE_BIT)
        std::cout << "GL context COMPATIBILITY profile enabled\n";

    std::cout << "App card info: " << '\n';
    for (GLenum key : sym_const)
    {
        const char* value = (const char*)glGetString(key);
        std::cout << "Value: " << value << '\n';
    }
}
void App::enableGlDebug() {
    if (GLEW_ARB_debug_output)
    {
        glDebugMessageCallback(MessageCallback, 0);
        glEnable(GL_DEBUG_OUTPUT);

        //default is asynchronous debug output, use this to simulate glGetError() functionality
        glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);

        std::cout << "GL_DEBUG enabled." << std::endl;
    }
    else
        std::cout << "GL_DEBUG NOT SUPPORTED!" << std::endl;
}

void App::setGlfwCallback(GLFWwindow* window) {
    glfwSetErrorCallback(errorCallback);
    glfwSetKeyCallback(window, KeyCallback);
    glfwSetFramebufferSizeCallback(window, FrameBufferSizeCallback);
    glfwSetMouseButtonCallback(window, mouseButtonCallback);
    glfwSetCursorPosCallback(window, cursorPositionCallback);
    glfwSetScrollCallback(window, scrollCallback);
}

void App::updateCamera()
{
    double nowGLFW = glfwGetTime();
    double delta_t = nowGLFW - lastFrameTime;

    glm::vec3 offset = camera.ProcessMovement(keys, delta_t);
    camera.Position += calculateCollisions(camera.Position, offset, scene);
    lastFrameTime = nowGLFW;
}

void App::measureFPS() {
    std::chrono::steady_clock::time_point now = std::chrono::steady_clock::now();
    if (std::chrono::duration_cast<std::chrono::milliseconds>(now - start).count() >= 1000)
    {
        start = std::chrono::steady_clock::now();
        std::cout << fpsCount << " FPS\n";
        fpsCount = 0;
    }
}

bool checkCollision(glm::vec3 position, glm::vec3 offset, Mesh object)
{
    bool collisionX = position[0] + 1.0f + offset[0] >= object.position[0] &&
        object.position[0] + object.width >= position[0] + offset[0];

    bool collisionZ = position[1] + offset[1] >= object.position[1] &&
        object.position[1] + object.height >= position[1] - 1.5f + offset[1];

    bool collisionY = position[2] + 1.0f + offset[2] >= object.position[2] &&
        object.position[2] + object.width >= position[2] + offset[2];

    return collisionX && collisionZ && collisionY;
}

glm::vec3 App::calculateCollisions(glm::vec3 position, glm::vec3 offset, std::unordered_map<std::string, Mesh>& scene)
{
    for (auto& model : scene) {
        if (checkCollision(position, offset, model.second)) {
            return glm::vec3(0.0);
        }
    }
    return offset;
}

void App::updateProjectionMatrix(void)
{
    if (height < 1)
        height = 1;

    float ratio = static_cast<float>(width) / height;
    projectionMatrix = glm::perspective(
        glm::radians(fov),
        ratio,
        0.1f,
        20000.0f
    );

    shader->setUniform("uPm", projectionMatrix);
}
void App::sendUniformLights(ShaderProgram shader) {
    glm::vec3 pointLightPositions[3] = {
        glm::vec3(0.0f,  2.0f,  0.0f),
        glm::vec3(10.0f, 2.0f, 10.0f),
        glm::vec3(0.0f,  2.0f, 8.0f),
    };

    struct PointLight {
        glm::vec3 position;
        glm::vec3 diffuse;
        glm::vec3 specular;
        float constant;
        float linear;
        float quadratic;
    };
    PointLight pointLights[4];

    pointLights[0].position = pointLightPositions[0];
    pointLights[1].position = pointLightPositions[1];
    pointLights[2].position = pointLightPositions[2];

    pointLights[0].diffuse = glm::vec3(0.9f, 0.0f, 0.0f);
    pointLights[1].diffuse = glm::vec3(1.0f, 0.8f, 0.0f);
    pointLights[2].diffuse = glm::vec3(0.2f, 0.0f, 0.8f);

    pointLights[0].specular = glm::vec3(0.9f, 0.0f, 0.0f);
    pointLights[1].specular = glm::vec3(1.0f, 0.8f, 0.0f);
    pointLights[2].specular = glm::vec3(0.2f, 0.0f, 0.8f);

    pointLights[0].constant = 1.0f;
    pointLights[1].constant = 1.0f;
    pointLights[2].constant = 1.0f;

    pointLights[0].linear = 0.09f;
    pointLights[1].linear = 0.09f;
    pointLights[2].linear = 0.09f;

    pointLights[0].quadratic = 0.032f;
    pointLights[1].quadratic = 0.032f;
    pointLights[2].quadratic = 0.032f;

    for (int i = 0; i < 3; i++) {
        std::string name = "pointLights[" + std::to_string(i) + "]";
        shader.setUniform(name + ".position", pointLights[i].position);
        shader.setUniform(name + ".diffuse", pointLights[i].diffuse);
        shader.setUniform(name + ".specular", pointLights[i].specular);
        shader.setUniform(name + ".constant", pointLights[i].constant);
        shader.setUniform(name + ".linear", pointLights[i].linear);
        shader.setUniform(name + ".quadratic", pointLights[i].quadratic);
    }
}

bool App::initAssets(std::unordered_map<std::string, Mesh>& scene) {

    std::filesystem::path path = std::filesystem::current_path() / "resources";
    std::filesystem::path vsPath = (path / "shader.vert");
    std::filesystem::path fsPath = (path / "shader.frag");

    GLuint floorTextureId = textureInit((path / "floor.png").u8string(), false);
    GLuint boxTextureId = textureInit((path / "box.jpg").u8string(), false);
    GLuint porcelainTextureId = textureInit((path / "porcelain.jpg").u8string(), false);

    ShaderProgram* sh = new ShaderProgram(vsPath, fsPath);
    this->shader = sh;
    shader->activate();

    shader->setUniform("dirLight.diffuse", glm::vec3(0.99f, 0.98f, 0.82f));
    shader->setUniform("dirLight.specular", glm::vec3(0.99f, 0.98f, 0.82f));
    shader->setUniform("dirLight.direction", glm::vec3(1.0f, -1.0f, 1.0f));

    shader->setUniform("material.specular", glm::vec3(0.25));
    shader->setUniform("material.shininess", 32.0f);
    shader->setUniform("material.transparency", 1.0f);

    sendUniformLights(*sh);

    updateProjectionMatrix();
    scene["floor"] = Mesh((path / "obj" / "plane_tri_vnt.obj").u8string(), *sh, floorTextureId, 100.0f, 0.01f, 100.0f, glm::vec3(-50.0f, 0.0f, -50.0f));
    scene["floor"].setModelMatrix(glm::scale(glm::translate(glm::identity<glm::mat4>(), glm::vec3(0.0f, 0.0f, 0.0f)), glm::vec3(20.0f, 1.0f, 20.0f)));

    scene["box"] = Mesh((path / "obj" / "cube_triangles_normals_tex.obj").u8string(), *sh, boxTextureId, 1.0f, 1.0f, 1.0f, glm::vec3(5.0f, 0.5f, 5.0f));
    scene["box"].setModelMatrix(glm::translate(glm::identity<glm::mat4>(), glm::vec3(5.0f, 0.5f, 5.0f)));

    scene["box1"] = Mesh((path / "obj" / "cube_triangles_normals_tex.obj").u8string(), *sh, boxTextureId, 1.0f, 1.0f, 1.0f, glm::vec3(-3.0f, 0.5f, 7.0f));
    scene["box1"].setModelMatrix(glm::translate(glm::identity<glm::mat4>(), glm::vec3(-3.0f, 0.5f, 7.0f)));

    scene["transparentBox"] = Mesh((path / "obj" / "cube_triangles_normals_tex.obj").u8string(), *sh, boxTextureId, 1.0f, 1.0f, 1.0f, glm::vec3(0.0f, 0.5f, 2.0f));
    scene["transparentBox"].setModelMatrix(glm::translate(glm::identity<glm::mat4>(), glm::vec3(0.0f, 0.5f, 2.0f)));
    scene["transparentBox"].transparent = true;

    scene["rotatingBunny"] = Mesh((path / "obj" / "bunny_tri_vnt.obj").u8string(), *sh, porcelainTextureId, 0.1f, 0.1f, 0.1f, glm::vec3(5.0f, 1.3f, 5.0f));
    scene["rotatingBunny"].setModelMatrix(glm::scale(glm::translate(glm::identity<glm::mat4>(), glm::vec3(5.0f, 1.3f, 5.0f)), glm::vec3(0.1f)));

    scene["cameraBunny"] = Mesh((path / "obj" / "bunny_tri_vnt.obj").u8string(), *sh, porcelainTextureId, 0.1f, 0.1f, 0.1f, glm::vec3(5.0f, 1.3f, 5.0f));
    scene["cameraBunny"].setModelMatrix(glm::scale(glm::translate(glm::identity<glm::mat4>(), glm::vec3(5.0f, 1.3f, 5.0f)), glm::vec3(0.1f)));

    scene["movingBunny"] = Mesh((path / "obj" / "bunny_tri_vnt.obj").u8string(), *sh, porcelainTextureId, 0.1f, 0.1f, 0.1f, glm::vec3(3.0f, 0.5f, 3.0f));
    scene["movingBunny"].setModelMatrix(glm::scale(glm::translate(glm::identity<glm::mat4>(), glm::vec3(3.0f, 0.5f, 3.0f)), glm::vec3(0.5f)));

    scene["movingBox"] = Mesh((path / "obj" / "cube_triangles_normals_tex.obj").u8string(), *sh, boxTextureId, 1.0f, 2.0f, 2.0f, glm::vec3(5.0f, 1.3f, 5.0f));
    scene["movingBox"].setModelMatrix(glm::scale(glm::translate(glm::identity<glm::mat4>(), glm::vec3(5.0f, 1.3f, 5.0f)), glm::vec3(0.1f)));

    scene["movingBox2"] = Mesh((path / "obj" / "cube_triangles_normals_tex.obj").u8string(), *sh, boxTextureId, 1.0f, 1.0f, 1.0f, glm::vec3(5.0f, 1.3f, 5.0f));
    scene["movingBox2"].setModelMatrix(glm::scale(glm::translate(glm::identity<glm::mat4>(), glm::vec3(5.0f, 1.3f, 5.0f)), glm::vec3(1.0f)));
    return true;
}

void App::drawScene() {

    shader->setUniform("camera.position", camera.Position);
    shader->setUniform("camera.direction", camera.Front);

    //update view matrix
    shader->setUniform("uVm", camera.GetViewMatrix());

    for (auto& model : scene) {
        if (model.second.transparent)
        {
            shader->setUniform("material.transparency", 0.4f);
            glEnable(GL_BLEND);
            model.second.draw();
            glDisable(GL_BLEND);
        }
        else {
            shader->setUniform("material.transparency", 1.0f);
            model.second.draw();
        }
    }
}
void App::updateDynamicModelMatrices()
{
    scene["rotatingBunny"].setModelMatrix(glm::rotate(glm::scale(glm::translate(glm::identity<glm::mat4>(), glm::vec3(5.0f, 1.3f, 5.0f)), glm::vec3(0.1f)),glm::radians((float)(glfwGetTime()*30.0)),glm::vec3(0.0f,1.0f,0.0f)));

    float x = 15 * cos(glfwGetTime() / 5.0);
    float y = 15 * sin(glfwGetTime() / 5.0);
    scene["movingBunny"].setModelMatrix(glm::rotate(glm::scale(glm::translate(glm::identity<glm::mat4>(), glm::vec3(x, 1.3f, y)), glm::vec3(0.5f)),glm::radians((float)(glfwGetTime()*30.0)),glm::vec3(0.0f,1.0f,0.0f)));
    scene["movingBunny"].position=glm::vec3(x, 1.3f, y);

    x = 5 * cos(glfwGetTime() / 5.0);
    y = 5 * sin(glfwGetTime() / 5.0);
    scene["movingBox"].setModelMatrix(glm::rotate(glm::scale(glm::translate(glm::identity<glm::mat4>(), glm::vec3(-x, 1.3f, -y)), glm::vec3(2.0f)), glm::radians((float)(glfwGetTime() * 30.0)), glm::vec3(1.0f, 0.0f, 0.0f)));
    scene["movingBox"].position = glm::vec3(-x, 1.3f, -y);

    float z = 5 + 5 * sin(glfwGetTime() / 5.0);
    scene["movingBox2"].setModelMatrix(glm::translate(glm::identity<glm::mat4>(), glm::vec3(3.0f, z, 0.0f)));
    scene["movingBox2"].position = glm::vec3(3.0f, z, 0.0f);
    if (!normalizedCenterQueue.empty()) {
        cv::Point2f pair = normalizedCenterQueue.pop_back();
        pair *= 10;
        scene["cameraBunny"].setModelMatrix(glm::rotate(glm::scale(glm::translate(glm::identity<glm::mat4>(), glm::vec3(pair.x, 0.5f, pair.y)), glm::vec3(0.1f)), glm::radians((float)(glfwGetTime() * 30.0)), glm::vec3(0.0f, 1.0f, 0.0f)));
        scene["cameraBunny"].position = glm::vec3(pair.x, 0.5f, pair.y);
    }
}

int App::runGraphics()
{
    GLFWwindow* window;
    if (!glfwInit())
        return -1;

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    window = glfwCreateWindow(640, 480, "ICP", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    setGlfwCallback(window);

    glfwSetWindowUserPointer(window, this);
    glfwGetFramebufferSize(window, &width, &height);
    glfwMakeContextCurrent(window);

    initGlew();
    showGlData();
    enableGlDebug();
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glCullFace(GL_BACK);
    glEnable(GL_CULL_FACE);
    glDepthFunc(GL_LEQUAL);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LINE_SMOOTH);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    if (!initAssets(scene)) {
        std::cout << "failed to load assets!!!\n";
        return 0;
    }
    glViewport(0, 0, width, height);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwGetCursorPos(window, &cursorLastX, &cursorLastY);
    start = std::chrono::steady_clock::now();

    while (!glfwWindowShouldClose(window))
    {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        updateCamera();
        updateDynamicModelMatrices();
        drawScene();
        glfwSwapBuffers(window);
        glfwPollEvents();
        measureFPS();
        fpsCount++;
    }

    glfwTerminate();
    return 0;
}

