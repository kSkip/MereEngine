#ifndef CAMERA_H
#define CAMERA_H

#include "GameObjects/GameObject.h"
#include "GameObjects/Weapon.h"

#include <cmath>

class Camera : public GameObject {

    public:

        Camera(ObjectData* objectData, ObjectData* weaponData);

        bool holdingForward;
        bool holdingBackward;
        bool holdingLeftStrafe;
        bool holdingRightStrafe;

        Weapon & GetWeapon() { return *currentWeapon; }

        void handleMouseMove(int mouseX, int mouseY);
        void move(double deltaTime, Camera* player);
        void render(Shader&);

        const vec3& getHead() { return head; }
        const vec3& getFront() { return front; }

        void commitMovement()
        {
            position += movement;
            head.x += movement.x;
            head.y += movement.y;
            head.z += movement.z;
            if(movement.y == 0.0f) ground = true;
        }

        void getViewMatrix(mat4&);

    private:

        vec3 head;
        vec3 front;
        vec3 up;
        vec3 right;

        float yaw;
        float pitch;

        float movementSpeedFactor;
        float pitchSensitivity;
        float yawSensitivity;

        Weapon* currentWeapon;
};

#endif
