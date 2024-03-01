#include "GameObjects/Character.h"
#include "GameObjects/Player.h"
#include "Models/Armature.h"
#include "Utilities/TextManipulation.h"
#include "Utilities/DataBlock.h"

Character::Character(){}

Character::Character(ObjectData* objectData, DataBlock& def)
{

    data = objectData;

    strvec pos = split(def("position"),',');

    if(pos.size() != 4)
        throw std::runtime_error("Position requires 4 values");

    float pos_init[3];
    pos_init[0] = atof(pos[0].c_str());
    pos_init[1] = atof(pos[1].c_str());
    pos_init[2] = atof(pos[2].c_str());

    rotY  = atof(pos[3].c_str());

    position    = vec3(pos_init[0],pos_init[1],pos_init[2]);
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
    front = vec3(sinRotY, 0.0f, cosRotY);
    right = vec3(cosRotY, 0.0f, -sinRotY);

    velocity = vec3(0.0f, 0.0f, 0.0f);

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

void Character::trackPlayer(double deltatime, Player& player)
{

    vec4 allowed, temp;

    movement = vec3(0.0f, 0.0f, 0.0f);
    allowed = vec4(0.0f, 0.0f, 0.0f, 0.0f);
    temp = vec4(0.0f, 0.0f, 0.0f, 0.0f);

    currentTarget = player.getPosition();

    vec3 vectorToTarget = currentTarget - position;
    float length = sqrt(dot(vectorToTarget, vectorToTarget));
    vectorToTarget = vec3(vectorToTarget.x, 0.0f, vectorToTarget.z);
    float frontDotProduct = dot(vectorToTarget, front) / length;
    float rightDotProduct = dot(vectorToTarget, right) / length;

    frontDotProduct = min(frontDotProduct, 1.0f);
    frontDotProduct = max(frontDotProduct, -1.0f);

    rightDotProduct = min(rightDotProduct, 1.0f);
    rightDotProduct = max(rightDotProduct, -1.0f);

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

void Character::move(double deltatime, Player& player)
{

    charAnimTime += deltatime;

    trackPlayer(deltatime, player);

    float sinRotY = sin(rotY);
    float cosRotY = cos(rotY);

    rotation = mat4(1.0f);
    rotation[0][0] = cosRotY;
    rotation[0][2] = -sinRotY;
    rotation[2][0] = sinRotY;
    rotation[2][2] = cosRotY;
    front = vec3(sinRotY, 0.0f, cosRotY);
    right = vec3(cosRotY, 0.0f, -sinRotY);

    velocity.y -= GRAVITATIONAL_ACCELERATION * deltatime;

    movement.x = movementSpeedFactor * deltatime * front.x;
    movement.y = deltatime * velocity.y;
    movement.z = movementSpeedFactor * deltatime * front.z;

    if(data){
        if (Armature* arm = data->armatures["run"]) {
            arm->buildFrame(charAnimTime);
            arm->setVertices(data->vertices.data(), data->unskinnedVertices.data(), data->numVertices);
        }
    }

}

void Character::commitMovement() {

    position += movement;
    translation = mat4(1.0f);
    translation[3].x = position.x;
    translation[3].y = position.y;
    translation[3].z = position.z;
    if (health < 0.0f)destroy = true;

}

void Character::render(Shader& shader){

    if(data){

        glBindBuffer(GL_ARRAY_BUFFER, data->vertexBuffer);
        glBufferSubData(GL_ARRAY_BUFFER,0,data->numVertices*sizeof(struct vertex),data->vertices.data());

        renderMeshElements(data->vertexBuffer,data->elementBuffer,data->elementCount,PTYPE_TRIANGLES, shader);

    }

}

void Character::damage(float magnitude, vec3 damageLocation)
{
    health -= magnitude;
    /*GameObject* newobject = new ParticleSource("Data/BloodDrop.3ds", damageLocation, glm::vec3(1.0f, 1.0f, 1.0f), 100, 0.5f, false, 0, true, state);
    addOpaqueObject(newobject,state);*/

}
