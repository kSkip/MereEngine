#include "GameObjects/Bullet.h"

Bullet::Bullet(){}

Bullet::Bullet(glm::vec3 raystart, glm::vec3 rayfinish, GameObject* p){

    position = raystart;
    movement = rayfinish;
    time     = 1.0f;

    tenativeHitObject = NULL;

    destroy   = false;
    skipMVP   = true;

    collisionTypeValue = RAY;
    parent = p;

}

Bullet::~Bullet(){}

Bullet::Bullet(const Bullet & rhs){}

void Bullet::move(double deltatime, Camera* player, std::list<GameObject*>* levelObjects){}

void Bullet::registerHit(GameObject* object, float point_on_ray){

    if(point_on_ray < time && point_on_ray > 0){
        tenativeHitObject = object;
        time = point_on_ray;
    }

}

void Bullet::render(GameState* state){}
