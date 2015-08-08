#include "GameObjects/StaticObject.h"
#include "Models/OBJGLBufferUtilities.hpp"

#include <cstring>

StaticObject::StaticObject(){}

StaticObject::StaticObject(std::string objectName, float position_in[3], float rotY_in, GameState* state){

    data = getObjectData(objectName,state);

    position = glm::vec3(position_in[0],position_in[1],position_in[2]);
    translation = glm::translate(glm::mat4(1.0f),position);
    rotation = glm::rotate(glm::mat4(1.0f),rotY_in,glm::vec3(0.0,1.0,0.0));
    rotY = rotY_in;

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
