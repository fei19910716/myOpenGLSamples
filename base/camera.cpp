#include "camera.h"


#define MovementSpeed       2.5f
#define MouseSensitivity    0.1f

Camera::Camera(glm::vec3 position, glm::vec3 front, glm::vec3 up) : 
    FOV(45.0f),Yaw(-90.0f),Pitch(0.0f), // Set these values to make the front equal to our caculations
    Position(position),Front(front),WorldUp(up)
{
    UpdateCameraVectors();
}

glm::mat4 Camera::GetViewMatrix()
{
    return glm::lookAt(Position, Position + Front, Up);
}


void Camera::ProcessKeyboard(KEY Key, float deltaTime)
{
    float velocity = MovementSpeed * deltaTime * 1000;
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

void Camera::ProcessMouseMovement(float xoffset, float yoffset)
{
    xoffset *= MouseSensitivity;
    yoffset *= MouseSensitivity;

    Yaw   += xoffset;
    Pitch += yoffset;

    // make sure that when pitch is out of bounds, screen doesn't get flipped
    // if (Pitch > 89.0f)
    //     Pitch = 89.0f;
    // if (Pitch < -89.0f)
    //     Pitch = -89.0f;

    // update Front, Right and Up Vectors using the updated Euler angles
    UpdateCameraVectors();
}

void Camera::ProcessMouseScroll(float yoffset)
{
    FOV -= (float)yoffset;
    
    // if (FOV < 1.0f)
    //     FOV = 1.0f;
    // if (FOV > 45.0f)
    //     FOV = 45.0f;
}

// calculates the front vector from the Camera's (updated) Euler Angles
void Camera::UpdateCameraVectors()
{
    // calculate the new Front vector
    glm::vec3 front;
    front.x = cos(glm::radians(Yaw)) * cos(glm::radians(Pitch));
    front.y = sin(glm::radians(Pitch));
    front.z = sin(glm::radians(Yaw)) * cos(glm::radians(Pitch));
    
    // normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
    Front = glm::normalize(front);
    Right = glm::normalize(glm::cross(Front, WorldUp)); 
    Up    = glm::normalize(glm::cross(Right, Front));
}