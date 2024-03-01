#ifndef BULLET_H
#define BULLET_H

#include "GameObjects/GameObject.h"

class Bullet : public GameObject{
public:
    Bullet(vec3, vec3, GameObject*);

    void move(double deltatime, Player&);
    void registerHit(GameObject* object, float point_on_ray);

    void commitMovement()
    {
        if(tenativeHitObject != NULL){
            tenativeHitObject->damage(1.0f,time*(movement-position)+position);
        }
        destroy = true;

    }

    void render(Shader&);

    GameObject* getParent(){ return parent;}

private:

    GameObject* parent;
    GameObject* tenativeHitObject;
    float time;
};

#endif
