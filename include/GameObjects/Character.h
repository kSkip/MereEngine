#ifndef CHARACTER_H
#define CHARACTER_H

#include "GameObject.h"

class Character : public GameObject{

    public:

        Character();
        Character(ObjectData* objectData, DataBlock& def);
        ~Character();
        Character(const Character & rhs);

        void move(double deltatime, Camera* player);

        void commitMovement(){

            position += movement;
            translation = glm::translate(glm::mat4(1.0f),position);
            if(health < 0.0f)destroy = true;

        }

        void damage(float magnitude, glm::vec3 damageLocation);

        void render(Shader&);

    private:

        glm::vec3 front;
        glm::vec3 right;

        float health;

        double movementSpeedFactor;
        glm::vec3 currentTarget;
        double charAnimTime;

        void trackPlayer(double deltatime, Camera* player);

};

#endif
