#ifndef CHARACTER_H
#define CHARACTER_H

#include "GameObject.h"
#include "GameObjects/Player.h"
#include "Models/MD5Model.h"

class Character : public GameObject {

public:
    Character(const vec3&, Player&, MD5Model*);
    ~Character();

    void move(double);

    void commitMovement();

    void damage(float magnitude, vec3 damageLocation);

    void draw(Shader&);
    boundary* getBounds() { return &model->bounds; }

private:
    MD5Model* model;
    vec3 front;
    vec3 right;

    float health;

    double movementSpeedFactor;
    vec3 currentTarget;
    double charAnimTime;

    Player& player;

    void trackPlayer(double);
};

#endif
