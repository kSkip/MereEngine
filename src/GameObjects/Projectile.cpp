#include "GameObjects/Projectile.h"

Projectile::Projectile(){}

Projectile::Projectile(std::string objectName, glm::vec3 initialPosition, glm::vec3 initialSpeed, glm::vec3 align, float roty, GameState* state){

    data = getObjectData(objectName,state);

    position = initialPosition;
    velocity = initialSpeed;
    translation = glm::translate(glm::mat4(1.0f),position);
    //rotation = glm::rotate(glm::mat4(1.0f),roty,glm::vec3(0.0,1.0,0.0));
    rotY = roty;

    glm::vec3 axis = glm::cross(glm::vec3(0.0f,1.0f,0.0f),glm::normalize(align));
    float s = glm::length(axis);
    float c = glm::dot(glm::vec3(0.0f,1.0f,0.0f),glm::normalize(align));

    if(c >= 0.0f){
        rotation = glm::rotate(glm::mat4(1.0f),float(180*asin(s)/PI),axis);
    }else if(c < 0.0f){
        rotation = glm::rotate(glm::mat4(1.0f),float(180-180*asin(s)/PI),axis);
    }

    destroy = false;
    skipMVP = false;
    skipLighting = false;

    collisionTypeValue = COLLIDER;
}

Projectile::~Projectile(){}

Projectile::Projectile(const Projectile & rhs){}

void Projectile::move(double deltatime, Camera* player, std::list<GameObject*>* levelObjects){

    movement = float(deltatime)*velocity;

}

void Projectile::render(GameState* state){

        renderMeshElements(data->vertexBuffer,data->elementBuffer,data->elementCount,PTYPE_TRIANGLES,state);

}
