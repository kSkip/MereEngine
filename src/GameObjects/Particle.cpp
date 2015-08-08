#include "GameObjects/Particle.h"
#include "GameObjects/Camera.h"

Particle::Particle(){}

Particle::Particle(ParticleParams& params, GameState* state){

    data = getObjectData(params.objectName,state);

    position = params.initPosition;
    velocity = params.initVelocity;
    life = params.life;
    gravity = params.gravity;

    destroy = false;
    skipMVP = false;
    skipLighting = false;

    collisionTypeValue = NONE;

}

Particle::~Particle(){}

Particle::Particle(const Particle & rhs){}

void Particle::move(double deltatime, Camera* player, std::list<GameObject*>* levelObjects){

    glm::vec3 camPosition = player->getHead();

        glm::vec3 front = glm::vec3(0.0f,0.0f,1.0f);
        glm::vec3 right = glm::vec3(-1.0f,0.0f,0.0f);
        glm::vec3 up = glm::vec3(0.0f,1.0f,0.0f);

        position += float(deltatime) * velocity;

        //next few blocks are for implementing billboarding of the particles' quads
        glm::vec3 vectorToCamera = glm::vec3(camPosition.x - position[0],camPosition.y - position[1],camPosition.z - position[2]);
        cDistance = glm::length2(vectorToCamera);
        glm::vec3 vectorToTarget = glm::normalize(glm::vec3(vectorToCamera.x,0.0f,vectorToCamera.z));

        double frontDotProduct = glm::dot(vectorToTarget,front);
        double rightDotProduct = glm::dot(vectorToTarget,right);
        float rotHorizontal, rotVertical;

        if(frontDotProduct > 1.0f){
            frontDotProduct = 1.0f;
        }
        if(frontDotProduct < -1.0f){
            frontDotProduct = -1.0f;
        }

        if(rightDotProduct >= 0){

            rotHorizontal = (-180.0f/PI)*acos(frontDotProduct);

        }else if(rightDotProduct < 0){

            rotHorizontal = (180.0f/PI)*acos(frontDotProduct);

        }

        glm::mat4 firstRot = glm::rotate(glm::mat4(1.0f),rotHorizontal,glm::vec3(0.0,1.0,0.0));

        glm::vec4 tempFront = (firstRot * glm::vec4(front,0.0f));
        front = glm::vec3(tempFront.x,tempFront.y,tempFront.z);

        vectorToTarget = glm::normalize(vectorToCamera);
        frontDotProduct = glm::dot(vectorToTarget,front);
        double upDotProduct = glm::dot(vectorToTarget,up);

        if(frontDotProduct > 1.0f){
            frontDotProduct = 1.0f;
        }
        if(frontDotProduct < -1.0f){
            frontDotProduct = -1.0f;
        }

        if(upDotProduct >= 0){

            rotVertical = (-180.0f/PI)*acos(frontDotProduct);

        }else if(upDotProduct < 0){

            rotVertical = (180.0f/PI)*acos(frontDotProduct);

        }

        rotation = firstRot;
        rotation = glm::rotate(glm::mat4(1.0f),rotVertical,glm::normalize(glm::cross(up,vectorToTarget))) * rotation;

        translation = glm::translate(glm::mat4(1.0f),position);

        //simulate gravity
        velocity.y -= GRAVITATIONAL_ACCELERATION*float(deltatime);

        //decrease the life of the particle
        life -= deltatime;

}

void Particle::commitMovement(GameState* state){}

void Particle::render(GameState* state){

    renderMeshElements(data->vertexBuffer,data->elementBuffer,data->elementCount,PTYPE_TRIANGLES,state);

}
