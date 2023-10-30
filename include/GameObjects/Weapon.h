#ifndef WEAPON_H
#define WEAPON_H

#include "GameObjects/GameObject.h"
#include "GameObjects/Camera.h"
#include "Models/Armature.h"

class Weapon : public GameObject{

    public:

        Weapon();
        Weapon(std::string objectName, GameState* state);
        ~Weapon();
        Weapon(const Weapon & rhs);

        void setTransformations(glm::mat4& translation, glm::mat4& rotation);

        void move(double deltaTime, Camera* player, std::list<GameObject*>* levelObjects);
        void render(GameState* state);

        void commitMovement(GameState* state){

        }

        void fire() { isFiring = 1; animTime = 0.0; }

        double animTime;
        int isFiring;
};

#endif
