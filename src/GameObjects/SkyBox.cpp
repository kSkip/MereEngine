#include "GameObjects/SkyBox.h"
#include <iostream>

SkyBox::SkyBox(){}

SkyBox::SkyBox(ObjectData* objectData, Camera* p){

    player = p;

    data = objectData;

    translation = mat4(1.0f);
    vec3 pos = player->getPosition();
    translation[3].x = pos.x;
    translation[3].y = pos.y;
    translation[3].z = pos.z;

    collisionTypeValue = NONE;
    destroy = false;

    skipMVP      = false;
    skipLighting = true;

}

SkyBox::~SkyBox(){}

SkyBox::SkyBox(const SkyBox & rhs){}

void SkyBox::move(double deltatime, Camera* player){}

void SkyBox::commitMovement()
{
    translation = mat4(1.0f);
    vec3 pos = player->getPosition();
    translation[3].x = pos.x;
    translation[3].y = pos.y;
    translation[3].z = pos.z;
}

void SkyBox::render(Shader& levelShader){

        renderMeshElements(data->vertexBuffer,data->elementBuffer,data->elementCount,PTYPE_TRIANGLES, levelShader);

}
