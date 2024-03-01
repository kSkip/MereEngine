#include "GameObjects/StaticObject.h"
#include "Utilities/TextManipulation.h"
#include "Utilities/DataBlock.h"

#include <cstring>

StaticObject::StaticObject(){}

StaticObject::StaticObject(ObjectData* objectData, DataBlock& def){

    data = objectData;

    strvec pos = split(def("position"),',');

    if(pos.size() != 4)
        throw std::runtime_error("Position requires 4 values");

    float pos_init[3];
    pos_init[0] = atof(pos[0].c_str());
    pos_init[1] = atof(pos[1].c_str());
    pos_init[2] = atof(pos[2].c_str());

    rotY  = atof(pos[3].c_str());
    rotY  = -rotY * 2.0f * PI / 360.0f;

    position = vec3(pos_init[0],pos_init[1],pos_init[2]);
    translation = mat4(1.0f);
    translation[3].x = position.x;
    translation[3].y = position.y;
    translation[3].y = position.z;

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

StaticObject::StaticObject(const StaticObject & rhs){}

void StaticObject::move(double deltatime, Player& player){}

void StaticObject::render(Shader& levelShader){

    if(data){

        renderMeshElements(data->vertexBuffer,data->elementBuffer,data->elementCount,PTYPE_TRIANGLES, levelShader);

    }

}
