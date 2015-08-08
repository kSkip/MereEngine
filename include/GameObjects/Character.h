#ifndef CHARACTER_H
#define CHARACTER_H

#include "GameObject.h"

class Character : public GameObject{

    public:

        Character();
        Character(std::string objectName, float position[3], float rotY, GameState* state);
        ~Character();
        Character(const Character & rhs);

        void move(double deltatime, Camera* player, std::list<GameObject*>* levelObjects);

        void commitMovement(GameState* state){

            position += movement;
            translation = glm::translate(glm::mat4(1.0f),position);
            if(health < 0.0f)destroy = true;

        }

        void damage(float magnitude, glm::vec3 damageLocation, GameState* state);

        void render(GameState* state);

    private:

        glm::vec3 front;
        glm::vec3 right;

        float health;

        double movementSpeedFactor;
        glm::vec3 currentTarget;
        double charAnimTime;

        void AIDecision(double deltatime, Camera* player, std::list<GameObject*>* levelObjects);

};

#endif
