#include "GameObjects/Character.h"
#include "GameObjects/Camera.h"
#include "Models/Armature.h"
#include "Utilities/TextManipulation.h"
#include "Utilities/DataBlock.h"

Character::Character(){}

Character::Character(ObjectData* objectData, DataBlock& def, GameState* state){

    data = objectData;

    strvec pos = split(def("position"),',');

    if(pos.size() != 4)
        throw std::runtime_error("Position requires 4 values");

    float pos_init[3];
    pos_init[0] = atof(pos[0].c_str());
    pos_init[1] = atof(pos[1].c_str());
    pos_init[2] = atof(pos[2].c_str());

    rotY  = atof(pos[3].c_str());

    position    = glm::vec3(pos_init[0],pos_init[1],pos_init[2]);
    translation = glm::translate(glm::mat4(1.0f),position);
    rotation    = glm::rotate(glm::mat4(1.0f),rotY,glm::vec3(0.0,1.0,0.0));

    front    = glm::rotateY(glm::vec3(0.0f,0.0f,1.0f),rotY);
    right    = glm::rotateY(glm::vec3(1.0f,0.0f,0.0f),rotY);
    velocity = glm::vec3(0.0f,0.0f,0.0f);

    movementSpeedFactor = 0.5f;

    ground              = false;
    health              = 5.0f;
    destroy             = false;
    skipMVP             = false;
    skipLighting        = false;

    collisionTypeValue = COLLIDER_COLLIDEE;
    charAnimTime = 0;

}

Character::~Character(){}

Character::Character(const Character & rhs){}

void Character::trackPlayer(double deltatime, Camera* player, std::list<GameObject*>* levelObjects){

    glm::vec4 allowed, temp;

    movement = glm::vec3(0.0f,0.0f,0.0f);
    allowed  = glm::vec4(0.0f,0.0f,0.0f,0.0f);
    temp     = glm::vec4(0.0f,0.0f,0.0f,0.0f);

    currentTarget = player->getPosition();

    glm::vec3 vectorToTarget = currentTarget - position;
    float length = glm::length(vectorToTarget);
    vectorToTarget = glm::vec3(vectorToTarget.x, 0.0f, vectorToTarget.z);
    float frontDotProduct = glm::dot(vectorToTarget, front) / length;
    float rightDotProduct = glm::dot(vectorToTarget, right) / length;

    frontDotProduct = std::min(frontDotProduct, 1.0f);
    frontDotProduct = std::max(frontDotProduct, -1.0f);

    rightDotProduct = std::min(rightDotProduct, 1.0f);
    rightDotProduct = std::max(rightDotProduct, -1.0f);

    float deltaRot = 0.1f * acos(frontDotProduct) / (length + 1);
    float twoPI = 2.0 * PI;
    if (rightDotProduct > 0.0) {
        rotY += deltaRot;
        if (rotY <= 0.0) {
            rotY += twoPI;
        }
    }
    else if (rightDotProduct < 0.0) {
        rotY -= deltaRot;
        if (rotY >= twoPI) {
            rotY -= twoPI;
        }
    }
}

void Character::move(double deltatime, Camera* player, std::list<GameObject*>* levelObjects){

    charAnimTime += deltatime;

    trackPlayer(deltatime, player, levelObjects);

    rotation = glm::rotate(glm::mat4(1.0f), rotY, glm::vec3(0.0, 1.0, 0.0));
    front = glm::rotateY(glm::vec3(0.0f, 0.0f, 1.0f), rotY);
    right = glm::rotateY(glm::vec3(1.0f, 0.0f, 0.0f), rotY);

    velocity.y -= GRAVITATIONAL_ACCELERATION * deltatime;

    movement.x = movementSpeedFactor * deltatime * front.x;
    movement.y = deltatime * velocity.y;
    movement.z = movementSpeedFactor * deltatime * front.z;

    if(data){
        if (Armature* arm = data->armatures["run"]) {
            arm->buildFrame(charAnimTime, NULL);
            arm->setVertices(data->vertices, data->unskinned_vertices, data->num_vertices);
        }
    }

}

void Character::render(GameState* state){

    if(data){

        glBindBuffer(GL_ARRAY_BUFFER, data->vertexBuffer);
        glBufferSubData(GL_ARRAY_BUFFER,0,data->num_vertices*sizeof(struct vertex),data->vertices);

        renderMeshElements(data->vertexBuffer,data->elementBuffer,data->elementCount,PTYPE_TRIANGLES,state);

    }

}

void Character::damage(float magnitude, glm::vec3 damageLocation, GameState* state)
{
    health -= magnitude;
    /*GameObject* newobject = new ParticleSource("Data/BloodDrop.3ds", damageLocation, glm::vec3(1.0f, 1.0f, 1.0f), 100, 0.5f, false, 0, true, state);
    addOpaqueObject(newobject,state);*/

}
