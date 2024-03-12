#include "GameObjects/Bullet.h"

Bullet::Bullet(vec3 raystart, vec3 rayfinish, GameObject* p){

    position = raystart;
    movement = rayfinish;
    time     = 1.0f;

    tenativeHitObject = NULL;

    destroy   = false;
    skipMVP   = true;

    collisionTypeValue = RAY;
    parent = p;

}

void Bullet::move(double deltatime){}

void Bullet::registerHit(GameObject* object, float point_on_ray)
{
    if(point_on_ray < time && point_on_ray > 0){
        tenativeHitObject = object;
        time = point_on_ray;
    }
}
