#pragma once

#include "base/math.h"
#include "base/keys.h"

class Camera
{
public:

    Camera(glm::vec3 position, glm::vec3 front, glm::vec3 up);
    ~Camera() = default;

    // returns the view matrix calculated using Euler Angles and the LookAt Matrix
    glm::mat4 GetViewMatrix();

   
    void ProcessKeyboard(KEY Key, float deltaTime);
    void ProcessMouseMovement(float xoffset, float yoffset);
    void ProcessMouseScroll(float yoffset);


    glm::vec3 Position;
    glm::vec3 Front;
    glm::vec3 Up;
    glm::vec3 Right;
    glm::vec3 WorldUp;

    float Yaw;
    float Pitch;
    float FOV;

private:
    // calculates the front vector from the Camera's (updated) Euler Angles
    void UpdateCameraVectors();
};