#include "GameObjects/StaticObject.h"
#include "Models/OBJGLBufferUtilities.h"
#include "Utilities/TextManipulation.h"
#include "Utilities/DataBlock.h"

#include <cstring>

StaticObject::StaticObject(){}

StaticObject::StaticObject(ObjectData* objectData, DataBlock& def, GameState* state){

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

    position = glm::vec3(pos_init[0],pos_init[1],pos_init[2]);
    translation = glm::translate(glm::mat4(1.0f),position);
    rotation = glm::rotate(glm::mat4(1.0f),rotY,glm::vec3(0.0,1.0,0.0));

    collisionTypeValue = COLLIDEE;
    destroy = false;

    skipMVP = false;
    skipLighting = false;

}

StaticObject::~StaticObject(){}

StaticObject::StaticObject(const StaticObject & rhs){}

void StaticObject::move(double deltatime, Camera* player, std::list<GameObject*>* levelObjects){}

void StaticObject::render(GameState* state){

    if(data){

        renderMeshElements(data->vertexBuffer,data->elementBuffer,data->elementCount,PTYPE_TRIANGLES,state);

    }

}
