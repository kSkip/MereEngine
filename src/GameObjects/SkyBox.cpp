#include "GameObjects/SkyBox.h"
#include <iostream>
SkyBox::SkyBox(){}

SkyBox::SkyBox(std::string objectName, Camera* p, GameState* state){

    player = p;

    data = getObjectData(objectName,state);

    translation = glm::translate(glm::mat4(1.0f),player->getPosition());

    collisionTypeValue = NONE;
    destroy = false;

    skipMVP      = false;
    skipLighting = true;

}

SkyBox::~SkyBox(){}

SkyBox::SkyBox(const SkyBox & rhs){}

void SkyBox::move(double deltatime, Camera* player, std::list<GameObject*>* levelObjects){}

void SkyBox::render(GameState* state){

        renderMeshElements(data->vertexBuffer,data->elementBuffer,data->elementCount,PTYPE_TRIANGLES,state);

}
