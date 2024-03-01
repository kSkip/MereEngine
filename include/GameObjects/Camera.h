#ifndef CAMERA_H
#define CAMERA_H

#include "VectorMath.h"

class Camera {

public:
    Camera(float, float, float, float);
    void handleMouseMove(int mouseX, int mouseY);
    const vec3& getHead() { return head; }
    const vec3& getFront() { return front; }
    const vec3& getUp() { return up; }
    const vec3& getRight() { return right; }
    void getViewMatrix(mat4&);

    friend class Player;

private:
    vec3 head;
    vec3 front;
    vec3 up;
    vec3 right;

    float yaw;
    float pitch;
    float pitchSensitivity;
    float yawSensitivity;
};

#endif
