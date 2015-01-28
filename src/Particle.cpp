
#include "GameObject.h"

Particle::Particle(){}

Particle::Particle(const char* objectfile, glm::vec3 p, glm::vec3 v, float l, bool g, GameState* state){

    Assimp::Importer importer;
    if(objectMap.find(std::string(objectfile)) == objectMap.end()){

        Assimp::Importer* importer = new Assimp::Importer();
        boundary* objectBounds = new boundary;
        ObjectData* newObjectData = new ObjectData;

        const aiScene* cpScene = importer->ReadFile(objectfile,aiProcess_GenSmoothNormals);

        aiMatrix4x4 m_GlobalInverseTransform = cpScene->mRootNode->mTransformation;
        m_GlobalInverseTransform.Inverse();
        float temp[16]={m_GlobalInverseTransform.a1,m_GlobalInverseTransform.a2,m_GlobalInverseTransform.a3,m_GlobalInverseTransform.a4,
                        m_GlobalInverseTransform.b1,m_GlobalInverseTransform.b2,m_GlobalInverseTransform.b3,m_GlobalInverseTransform.b4,
                        m_GlobalInverseTransform.c1,m_GlobalInverseTransform.c2,m_GlobalInverseTransform.c3,m_GlobalInverseTransform.c4,
                        m_GlobalInverseTransform.d1,m_GlobalInverseTransform.d2,m_GlobalInverseTransform.d3,m_GlobalInverseTransform.d4};
        newObjectData->globalInverseTransform = glm::make_mat4(temp);
        newObjectData->globalInverseTransform = glm::transpose(newObjectData->globalInverseTransform);

        newObjectData->VertexBuffer = makeArrayBuffer(cpScene->mMeshes[0],newObjectData);
        newObjectData->ElementBuffer = makeElementBuffer(cpScene->mMeshes[0]);
        newObjectData->ElementCount = cpScene->mMeshes[0]->mNumFaces * 3;
        newObjectData->TextureId = makeGetTexBuf(cpScene->mMaterials[0],state);
        newObjectData->Importer = importer;
        newObjectData->objectBounds = objectBounds;

        objectMap[std::string(objectfile)] = newObjectData;
        data = newObjectData;
    }else{
        data = objectMap[std::string(objectfile)];
    }

    position = p;
    velocity = v;
    life = l;
    gravity = g;

    destroy = false;
    skipBones = true;
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

void Particle::commitMovement(GameState* state){
}

void Particle::render(GameState* state){
    renderMesh(data->VertexBuffer,data->ElementBuffer,data->ElementCount,state);
}
