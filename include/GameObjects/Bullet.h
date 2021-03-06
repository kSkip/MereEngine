#ifndef BULLET_H
#define BULLET_H

#include "GameObjects/GameObject.h"

class Bullet : public GameObject{

    public:

        Bullet();
        Bullet(glm::vec3 raystart, glm::vec3 rayfinish, GameObject* parent);
        ~Bullet();
        Bullet(const Bullet & rhs);

        void move(double deltatime, Camera* player, std::list<GameObject*>* levelObjects);
        void registerHit(GameObject* object, float point_on_ray);

        void commitMovement(GameState* state){

            if(tenativeHitObject != NULL){
                tenativeHitObject->damage(1.0f,time*(movement-position)+position,state);
            }
            destroy = true;

        }

        void render(GameState* state);

        GameObject* getParent(){ return parent;}

    private:

        GameObject* parent;
        GameObject* tenativeHitObject;
        float time;

};

#endif
