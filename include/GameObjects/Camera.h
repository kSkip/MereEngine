#ifndef CAMERA_H
#define CAMERA_H

#include "GameObjects/GameObject.h"
#include "GameObjects/Billboard.h"
#include "GameObjects/Weapon.h"
#include "Models/Armature.hpp"

#include <iostream>
#include <cmath>

#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtx/rotate_vector.hpp>

class Weapon;

class Camera : public GameObject{

    public:

        Camera(ObjectData* objectData, float windowSize[2], DataBlock& def, GameState* state);
        ~Camera();

        bool holdingForward;
        bool holdingBackward;
        bool holdingLeftStrafe;
        bool holdingRightStrafe;

        Weapon & GetWeapon() { return *currentWeapon; }

        void handleMouseMove(int mouseX, int mouseY);
        void move(double deltaTime, Camera* player, std::list<GameObject*>* levelObjects);
        void render(GameState* state);

        void commitMovement(GameState* state){

                position += movement;
                head += movement;
                origin += movement;
                if(movement.y == 0.0f) ground = true;

        }

        float getPitchSensitivity()            { return pitchSensitivity;  }
        void  setPitchSensitivity(float value) { pitchSensitivity = value; }
        float getYawSensitivity()              { return yawSensitivity;    }
        void  setYawSensitivity(float value)   { yawSensitivity   = value; }

        glm::vec3 getPosition() const {return position;}
        void setXPos(double x)	{position.x = x;}
        void setYPos(double y)	{position.y = y;}
        void setZPos(double z)	{position.z = z;}

        glm::vec3 getHead() const { return head;}
        void setXHead(double x)	{head.x = x;}
        void setYHead(double y)	{head.y = y;}
        void setZHead(double z)	{head.z = z;}

        glm::vec3 getOrigin() const { return origin;}
        void setXOrigin(double x)	{origin.x = x;}
        void setYOrigin(double y)	{origin.y = y;}
        void setZOrigin(double z)	{origin.z = z;}

        glm::vec3 getFront() const { return front;}
        void setXFront(double x)	{front.x = x;}
        void setYFront(double y)	{front.y = y;}
        void setZFront(double z)	{front.z = z;}

        glm::vec3 getRight() const { return right;}
        void setXRight(double x)	{right.x = x;}
        void setYRight(double y)	{right.y = y;}
        void setZRight(double z)	{right.z = z;}

        glm::vec3 getUp() const { return up;}
        void setXUp(double x)	{up.x = x;}
        void setYUp(double y)	{up.y = y;}
        void setZUp(double z)	{up.z = z;}

    private:

        glm::vec3 head;
        glm::vec3 origin; //for glmLootAt
        glm::vec3 front;
        glm::vec3 up;
        glm::vec3 right;

        float rotHorizontal, rotVertical;

        Weapon* currentWeapon;

        double movementSpeedFactor;
        double pitchSensitivity;
        double yawSensitivity;

        int windowWidth;
        int windowHeight;
        int windowMidX;
        int windowMidY;

        void initCamera();

};

#endif
