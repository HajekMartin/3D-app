#pragma once
#include <GL/glew.h>
#include <GL/wglew.h>
#include <GLFW/glfw3.h>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <unordered_set>
#include <glm/gtx/string_cast.hpp>
class Camera
{
public:
    enum class direction {
        FORWARD,
        BACKWARD,
        LEFT,
        RIGHT,
        UP,
        DOWN
    };

    // Camera Attributes
    glm::vec3 Position;
    glm::vec3 Front;
    glm::vec3 Right;
    glm::vec3 Up; // camera local UP vector

    GLfloat Yaw = -90.0f;
    GLfloat Pitch = 0.0f;;
    GLfloat Roll = 0.0f;

    // Camera options
    GLfloat MovementSpeed = 3.0f;
    GLfloat MouseSensitivity = 0.25f;

    Camera(glm::vec3 position) :Position(position)
    {
        this->Up = glm::vec3(0.0f, 1.0f, 0.0f);
        this->updateCameraVectors();
    }

    glm::mat4 GetViewMatrix()
    {
        return glm::lookAt(this->Position, this->Position + this->Front, this->Up);
    }
    glm::vec3 ProcessMovement(std::unordered_set<int> keys,GLfloat deltaTime) {
        auto window = glfwGetCurrentContext();
        glm::vec3 movement = { 0.0f, 0.0f, 0.0f };
        for (const int& key : keys) {
            switch (key)
            {
            case GLFW_KEY_W:
                movement += ProcessInput(Camera::direction::FORWARD, deltaTime);
                break;
            case GLFW_KEY_S:
                movement += ProcessInput(Camera::direction::BACKWARD, deltaTime);
            break;
            case GLFW_KEY_A:
                movement += ProcessInput(Camera::direction::LEFT, deltaTime);
                break;
            case GLFW_KEY_D:
                movement += ProcessInput(Camera::direction::RIGHT, deltaTime);
                break;
            case GLFW_KEY_SPACE:
                movement += ProcessInput(Camera::direction::UP, deltaTime);
                break;
            case GLFW_KEY_LEFT_CONTROL:
                movement += ProcessInput(Camera::direction::DOWN, deltaTime);
                break;
            default:
                break;
            }
        }

        return movement;

    }
    glm::vec3 ProcessInput(Camera::direction direction, GLfloat deltaTime)
    {
        GLfloat velocity = this->MovementSpeed * deltaTime;
        glm::vec3 dir;
        switch (direction)
        {
        case direction::FORWARD:
            return this->Front * velocity;
        case direction::BACKWARD:
            return -this->Front * velocity;
        case direction::LEFT:
            return -this->Right * velocity;
        case direction::RIGHT:
            return this->Right * velocity;
        case direction::UP:
            return this->Up * velocity;
        case direction::DOWN:
            return -this->Up * velocity;
        }

        return glm::vec3(0.0f);
    }

    void ProcessMouseMovement(GLfloat xoffset, GLfloat yoffset, GLboolean constraintPitch = GL_TRUE)
    {
        xoffset *= this->MouseSensitivity;
        yoffset *= this->MouseSensitivity;

        this->Yaw += xoffset;
        this->Pitch += yoffset;

        if (constraintPitch)
        {
            if (this->Pitch > 89.0f)
                this->Pitch = 89.0f;
            if (this->Pitch < -89.0f)
                this->Pitch = -89.0f;
        }

        this->updateCameraVectors();
    }

private:
    void updateCameraVectors()
    {
        glm::vec3 front;
        front.x = cos(glm::radians(this->Yaw)) * cos(glm::radians(this->Pitch));
        front.y = sin(glm::radians(this->Pitch));
        front.z = sin(glm::radians(this->Yaw)) * cos(glm::radians(this->Pitch));

        this->Front = glm::normalize(front);
        this->Right = glm::normalize(glm::cross(this->Front, glm::vec3(0.0f, 1.0f, 0.0f)));
        this->Up = glm::normalize(glm::cross(this->Right, this->Front));
    }
};
