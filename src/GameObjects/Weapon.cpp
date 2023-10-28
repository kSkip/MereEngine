#include "GameObjects/Weapon.h"

Weapon::Weapon(){}

Weapon::Weapon(std::string objectName, GameState* state){

    data = getObjectData(objectName,state);

    skipMVP = false;
    skipLighting = false;

}

Weapon::~Weapon(){}

Weapon::Weapon(const Weapon & rhs){}

void Weapon::setTransformations(glm::mat4& translation_in, glm::mat4& rotation_in){

    translation = translation_in;
    rotation = rotation_in;

}

void Weapon::move(double deltaTime, Camera* player, std::list<GameObject*>* levelObjects){

    Armature* arm;

    if(data){

        arm = data->armatures[std::string("fire")];

    }

    if(arm){

        arm->buildFrame(0.0f,NULL);
        arm->setVertices(data->vertices,data->unskinned_vertices,data->num_vertices);

    }

}

void Weapon::render(GameState* state){

    if(data){

        glBindBuffer(GL_ARRAY_BUFFER, data->vertexBuffer);
        glBufferSubData(GL_ARRAY_BUFFER,0,data->num_vertices*sizeof(struct vertex),data->vertices);

        renderMeshElements(data->vertexBuffer,data->elementBuffer,data->elementCount,PTYPE_TRIANGLES,state);

    }

}
