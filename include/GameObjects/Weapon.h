#ifndef WEAPON_H
#define WEAPON_H

#include "GameObject.h"

class Weapon : public GameObject{

    public:

        Weapon();
        Weapon(ObjectData*);
        ~Weapon();
        Weapon(const Weapon & rhs);

        void setTransformations(mat4& translation, mat4& rotation);

        void move(double deltaTime, Camera* player);
        void render(Shader&);

        void commitMovement(){

        }

        void fire() { isFiring = 1; animTime = 0.0; }

        double animTime;
        int isFiring;
};

#endif
