#include "GameObjects/StaticObject.h"

StaticObject::StaticObject(OBJMesh* model) : model(model)
{
    rotY = 0.0f;
    translation = mat4(1.0f);
    translation[3].x = position.x;
    translation[3].y = position.y;
    translation[3].z = position.z;

    float sinRotY = sin(rotY);
    float cosRotY = cos(rotY);

    rotation = mat4(1.0f);
    rotation[0][0] = cosRotY;
    rotation[0][2] = -sinRotY;
    rotation[2][0] = sinRotY;
    rotation[2][2] = cosRotY;

    collisionTypeValue = COLLIDEE;
    destroy = false;

    skipMVP = false;
    skipLighting = false;

}

StaticObject::~StaticObject(){}

void StaticObject::draw(Shader& shader)
{
    model->draw(shader, translation, rotation, skipMVP, skipLighting);
}