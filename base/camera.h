#pragma once

#include "base/math.h"
#include "base/keys.h"


#define MovementSpeed       2.5f
#define MouseSensitivity    0.1f


class Camera
{
public:
    // camera Attributes
    glm::vec3 Position;
    glm::vec3 Front;
    glm::vec3 Up;
    glm::vec3 Right;
    glm::vec3 WorldUp;

    // euler Angles
    float Yaw;
    float Pitch;
    float FOV;

    // constructor with vectors
    Camera(glm::vec3 position, glm::vec3 front, glm::vec3 up) : 
        FOV(45.0f),Yaw(-90.0f),Pitch(0.0f),
        Position(position),Front(front),WorldUp(up)
    {
        UpdateCameraVectors();
    }

    // returns the view matrix calculated using Euler Angles and the LookAt Matrix
    glm::mat4 GetViewMatrix()
    {
        return glm::lookAt(Position, Position + Front, Up);
    }
    

    // processes input received from any keyboard-like input system. Accepts input parameter in the form of camera defined ENUM (to abstract it from windowing systems)
    void ProcessKeyboard(KEY Key, float deltaTime)
    {
        float velocity = MovementSpeed;
        switch (Key)
        {
            case KEY_UP:
                Position += Front * velocity;
                break;
            case KEY_DOWN:
                Position -= Front * velocity;
                break;
            case KEY_LEFT:
                Position -= Right * velocity;
                break;
            case KEY_RIGHT:
                Position += Right * velocity;
                break;
            
            default:
                break;
        }
    }

    // processes input received from a mouse input system. Expects the offset value in both the x and y direction.
    void ProcessMouseMovement(float xoffset, float yoffset)
    {
        xoffset *= MouseSensitivity;
        yoffset *= MouseSensitivity;

        Yaw   += xoffset;
        Pitch += yoffset;

        // make sure that when pitch is out of bounds, screen doesn't get flipped
        if (Pitch > 89.0f)
            Pitch = 89.0f;
        if (Pitch < -89.0f)
            Pitch = -89.0f;

        // update Front, Right and Up Vectors using the updated Euler angles
        UpdateCameraVectors();
    }

    // processes input received from a mouse scroll-wheel event. Only requires input on the vertical wheel-axis
    void ProcessMouseScroll(float yoffset)
    {
        FOV -= (float)yoffset;
        
        if (FOV < 1.0f)
            FOV = 1.0f;
        if (FOV > 45.0f)
            FOV = 45.0f;
    }

private:
    // calculates the front vector from the Camera's (updated) Euler Angles
    void UpdateCameraVectors()
    {
        // calculate the new Front vector
        glm::vec3 front;
        front.x = cos(glm::radians(Yaw)) * cos(glm::radians(Pitch));
        front.y = sin(glm::radians(Pitch));
        front.z = sin(glm::radians(Yaw)) * cos(glm::radians(Pitch));
        
        Front = glm::normalize(front);
        Right = glm::normalize(glm::cross(Front, WorldUp));  // normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
        Up    = glm::normalize(glm::cross(Right, Front));
    }
};