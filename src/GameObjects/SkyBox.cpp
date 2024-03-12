#include "GameObjects/SkyBox.h"

SkyBox::SkyBox(Player& p, OBJMesh* mesh) : player(p), model(mesh)
{
    translation = mat4(1.0f);
    vec3 pos = player.getPosition();
    translation[3].x = pos.x;
    translation[3].y = pos.y;
    translation[3].z = pos.z;

    collisionTypeValue = NONE;
    destroy = false;

    skipMVP = false;
    skipLighting = true;
}

void SkyBox::commitMovement()
{
    translation = mat4(1.0f);
    vec3 pos = player.getPosition();
    translation[3].x = pos.x;
    translation[3].y = pos.y;
    translation[3].z = pos.z;
}

void SkyBox::draw(Shader& shader)
{
    model->draw(shader, translation, rotation, skipMVP, skipLighting);
}