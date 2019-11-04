#ifndef CAMERA_H
#define CAMERA_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
using namespace std;
using namespace glm;

const float YAW = -90.0f;
const float PITCH = 0.0f;

enum Camera_Movement
{
    FORWARD,
    BACKWARD,
    LEFT,
    RIGHT
};

class Camera
{
public:
    vec3 position;
    vec3 front;
    vec3 up;
    vec3 right;
    vec3 worldUp;

    float yawAngle;
    float pitchAngle;
    float movementSpeed;
    float mouseSensitivity;
    float zoom;

    Camera(vec3 position = vec3(0.0f, 0.0f, 0.0f), vec3 up = vec3(0.0f, 1.0f, 0.0f), float yaw = YAW, float pitch = PITCH);
    Camera(float posX, float posY, float posZ, float upX, float upY, float upZ, float yaw, float pitch);

    mat4 getViewMatrix();
    void processKeyboard(Camera_Movement direction);
    void processMouseMovement(float xoffset, float yoffset, GLboolean constrainPitch);
    void processMouseScroll(float yoffset);

private:
    void updateCameraVectors();
};

#endif // !CAMERA_H
