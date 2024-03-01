#ifndef WEAPON_H
#define WEAPON_H

#include "GameObject.h"

class Weapon {
public:
    Weapon();

    void setTransformations(mat4& translation, mat4& rotation);

    void move(double);
    void render(Shader&);

    void fire() { isFiring = 1; animTime = 0.0; }

    void setObjectData(ObjectData* objData) { data = objData; }

    double animTime;
    int isFiring;

private:
    ObjectData *data;

    vec3 position;
    float rotY;
    vec3 movement;
    vec3 velocity;

    mat4 translation, rotation;

    bool skipMVP;
    bool skipLighting;
};

#endif
