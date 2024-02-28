#include "GameObjects/Weapon.h"

Weapon::Weapon() : animTime(0.0) {}

Weapon::Weapon(ObjectData *data) :
    animTime(0.0),
    isFiring(0)
{

    this->data = data;

    skipMVP = false;
    skipLighting = false;

}

Weapon::~Weapon(){}

Weapon::Weapon(const Weapon & rhs){}

void Weapon::setTransformations(mat4& translation_in, mat4& rotation_in){

    translation = translation_in;
    rotation = rotation_in;

}

void Weapon::move(double deltatime, Camera* player)
{    
    if(data){
        if (Armature* arm = data->armatures["fire"]) {
            if (isFiring) {
                animTime += deltatime;
                if (animTime > arm->getTotalTime()) {
                    animTime = 0.0;
                    isFiring = 0;
                }
            }
            else {
                animTime = 0.0;
            }
            arm->buildFrame(animTime);
            arm->setVertices(data->vertices.data(), data->unskinnedVertices.data(), data->numVertices);
        }
    }

}

void Weapon::render(Shader& levelShader){

    if(data){

        glBindBuffer(GL_ARRAY_BUFFER, data->vertexBuffer);
        glBufferSubData(GL_ARRAY_BUFFER,0,data->numVertices*sizeof(struct vertex),data->vertices.data());

        renderMeshElements(data->vertexBuffer,data->elementBuffer,data->elementCount,PTYPE_TRIANGLES, levelShader);

    }

}
