#include "GameObjects/Weapon.h"

Weapon::Weapon() : animTime(0.0) {}

Weapon::Weapon(std::string objectName, GameState* state) :
    animTime(0.0),
    isFiring(0)
{

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

void Weapon::move(double deltatime, Camera* player, std::list<GameObject*>* levelObjects)
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
            arm->buildFrame(animTime, NULL);
            arm->setVertices(data->vertices, data->unskinned_vertices, data->num_vertices);
        }
    }

}

void Weapon::render(GameState* state){

    if(data){

        glBindBuffer(GL_ARRAY_BUFFER, data->vertexBuffer);
        glBufferSubData(GL_ARRAY_BUFFER,0,data->num_vertices*sizeof(struct vertex),data->vertices);

        renderMeshElements(data->vertexBuffer,data->elementBuffer,data->elementCount,PTYPE_TRIANGLES,state);

    }

}
