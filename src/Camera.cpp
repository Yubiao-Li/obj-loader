#include "Camera.h"

const float SPEED = 0.05f;
const float SENSITIVITY = 0.1f;
const float ZOOM = 45.0f;

Camera::Camera(vec3 position, vec3 up, float yaw, float pitch)
    : front(vec3(0.0f, 0.0f, -1.0f)),
      movementSpeed(SPEED),
      mouseSensitivity(SENSITIVITY),
      zoom(ZOOM)
{
    this->position = position;
    this->worldUp = up;
    this->yawAngle = yaw;
    this->pitchAngle = pitch;
    updateCameraVectors();
}

Camera::Camera(float posX, float posY, float posZ, float upX, float upY, float upZ, float yaw, float pitch)
    : front(vec3(0.0f, 0.0f, -1.0f)),
      movementSpeed(SPEED),
      mouseSensitivity(SENSITIVITY),
      zoom(ZOOM)
{
    position.x = posX;
    position.y = posY;
    position.z = posZ;
    worldUp.x = upX;
    worldUp.y = upY;
    worldUp.z = upZ;
    this->yawAngle = yaw;
    this->pitchAngle = pitch;
}

mat4 Camera::getViewMatrix()
{
    vec3 target = position + front;
    return lookAt(position, target, worldUp);
}

void Camera::processKeyboard(Camera_Movement direction)
{
    switch (direction)
    {
    case FORWARD:
        position += front * movementSpeed;
        break;
    case BACKWARD:
        position -= front * movementSpeed;
        break;
    case LEFT:
        position -= normalize(cross(front, worldUp)) * movementSpeed;
        break;
    case RIGHT:
        position += normalize(cross(front, worldUp)) * movementSpeed;
        break;
    default:
        break;
    }
}

void Camera::processMouseMovement(float xoffset, float yoffset, GLboolean constrainPitch)
{
    yawAngle += xoffset * mouseSensitivity;
    pitchAngle += yoffset * mouseSensitivity;
    if (pitchAngle < -constrainPitch)
    {
        pitchAngle = -constrainPitch;
    }
    if (pitchAngle > constrainPitch)
    {
        pitchAngle = constrainPitch;
    }
    updateCameraVectors();
}

void Camera::processMouseScroll(float yoffset)
{
    zoom -= 5 * yoffset;
    if (zoom <= 1.0f)
        zoom = 1.0f;
    if (zoom >= 90.0f)
        zoom = 90.0f;
}

void Camera::updateCameraVectors()
{
    front.x = cos(radians(pitchAngle)) * cos(radians(yawAngle));
    front.y = sin(radians(pitchAngle));
    front.z = cos(radians(pitchAngle)) * sin(radians(yawAngle));

    right = normalize(cross(worldUp, front));
    up = normalize(cross(front, right));
}