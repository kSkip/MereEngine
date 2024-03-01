#ifndef CHARACTER_H
#define CHARACTER_H

#include "GameObject.h"

class Character : public GameObject{

    public:

        Character();
        Character(ObjectData* objectData, DataBlock& def);
        ~Character();
        Character(const Character & rhs);

        void move(double, Player&);

        void commitMovement();

        void damage(float magnitude, vec3 damageLocation);

        void render(Shader&);

    private:

        vec3 front;
        vec3 right;

        float health;

        double movementSpeedFactor;
        vec3 currentTarget;
        double charAnimTime;

        void trackPlayer(double, Player&);

};

#endif
