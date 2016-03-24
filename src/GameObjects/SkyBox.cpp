#include "GameObjects/SkyBox.h"
#include <iostream>
SkyBox::SkyBox(){}

SkyBox::SkyBox(ObjectData* objectData, Camera* p, GameState* state){

    player = p;

    data = objectData;

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
