#include "GameObject.h"

BulletObject::BulletObject(){}

BulletObject::BulletObject(glm::vec3 raystart, glm::vec3 rayfinish, GameObject* p){
    position = raystart;
    movement = rayfinish;
    time = 1.0f;
    tenativeHitObject = NULL;

    destroy = false;
    skipBones = true;
    skipMVP = true;

    collisionTypeValue = RAY;
    parent = p;
}

BulletObject::~BulletObject(){}

BulletObject::BulletObject(const BulletObject & rhs){}

void BulletObject::move(double deltatime, Camera* player, std::list<GameObject*>* levelObjects){}

void BulletObject::registerHit(GameObject* object, float point_on_ray){

    if(point_on_ray < time && point_on_ray > 0){
        tenativeHitObject = object;
        time = point_on_ray;
    }

}

void BulletObject::render(GameState* state){}
