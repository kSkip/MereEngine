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

void Character::AIDecision(double deltatime, Camera* player, std::list<GameObject*>* levelObjects){

    glm::vec4 allowed, temp;

    movement = glm::vec3(0.0f,0.0f,0.0f);
    allowed  = glm::vec4(0.0f,0.0f,0.0f,0.0f);
    temp     = glm::vec4(0.0f,0.0f,0.0f,0.0f);

    currentTarget = player->getPosition();

    glm::vec3 vectorToTarget = currentTarget - position;
    vectorToTarget           = glm::vec3(vectorToTarget.x,0.0f,vectorToTarget.z);
    double frontDotProduct  = glm::dot(vectorToTarget,front)/glm::length(vectorToTarget);
    double rightDotProduct  = glm::dot(vectorToTarget,right)/glm::length(vectorToTarget);

    if(frontDotProduct > 1.0f){
        frontDotProduct = 1.0f;
    }
    if(frontDotProduct < -1.0f){
        frontDotProduct = -1.0f;
    }
    if(rightDotProduct > 1.0f){
        rightDotProduct = 1.0f;
    }
    if(rightDotProduct < -1.0f){
        rightDotProduct = -1.0f;
    }

    if(rightDotProduct > 0){

        rotY += 0.1f*(180.0f/PI)*acos(frontDotProduct)/(glm::length(vectorToTarget)+1);
        if(rotY <= 0){rotY += 360.0f;}

    }else if(rightDotProduct < 0){

        rotY -= 0.1f*(180.0f/PI)*acos(frontDotProduct)/(glm::length(vectorToTarget)+1);
        if(rotY >= 360){rotY -= 360.0f;}

    }

    rotation = glm::rotate(glm::mat4(1.0f),rotY,glm::vec3(0.0,1.0,0.0));
    front    = glm::rotateY(glm::vec3(0.0f,0.0f,1.0f),rotY);
    right    = glm::rotateY(glm::vec3(1.0f,0.0f,0.0f),rotY);

    velocity.y -= GRAVITATIONAL_ACCELERATION*deltatime;

    movement.x = movementSpeedFactor * deltatime * front.x;
    movement.y = deltatime * velocity.y;
    movement.z = movementSpeedFactor * deltatime * front.z;

}

void Character::move(double deltatime, Camera* player, std::list<GameObject*>* levelObjects){

    charAnimTime += deltatime;

    AIDecision(deltatime, player, levelObjects);

    Armature* arm = nullptr;

    if(data){

        arm = data->armatures[std::string("run")];

    }

    if(arm){

        arm->buildFrame(charAnimTime,NULL);
        arm->setVertices(data->vertices,data->unskinned_vertices,data->num_vertices);

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
