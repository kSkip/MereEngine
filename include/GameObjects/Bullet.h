#ifndef BULLET_H
#define BULLET_H

#include "GameObjects/GameObject.h"

class Bullet : public GameObject{

    public:

        Bullet();
        Bullet(vec3 raystart, vec3 rayfinish, GameObject* parent);
        ~Bullet();
        Bullet(const Bullet & rhs);

        void move(double deltatime, Camera* player);
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
