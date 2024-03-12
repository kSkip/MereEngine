#ifndef WEAPON_H
#define WEAPON_H

#include "VectorMath.h"
#include "Shader.h"
#include "Models/MD5Model.h"

class Weapon {
public:
    Weapon();

    void move(double);
    void draw(Shader&);

    void fire() { isFiring = 1; animTime = 0.0; }

    double animTime;
    int isFiring;

    MD5Model* model;

private:
    vec3 position;
    float rotY;
    vec3 movement;
    vec3 velocity;

    mat4 translation, rotation;

    bool skipMVP;
    bool skipLighting;
};

#endif
