#include "GameObject.h"

ProjectileObject::ProjectileObject(){}

ProjectileObject::ProjectileObject(char* objectfile, char* boundsfile, glm::vec3 initialPosition, glm::vec3 initialSpeed, glm::vec3 align, float roty, GameState* state){

    Assimp::Importer importer;
    if(objectMap.find(std::string(objectfile)) == objectMap.end()){

        Assimp::Importer* importer = new Assimp::Importer();
        boundary* objectBounds = new boundary;
        ObjectData* newObjectData = new ObjectData;

        const aiScene* cpScene = importer->ReadFile(objectfile,aiProcess_GenSmoothNormals);
        objectBounds->LoadBoundaries(std::string(boundsfile));

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

    position = initialPosition;
    velocity = initialSpeed;
    translation = glm::translate(glm::mat4(1.0f),position);
    //rotation = glm::rotate(glm::mat4(1.0f),roty,glm::vec3(0.0,1.0,0.0));
    rotY = roty;

    glm::vec3 axis = glm::cross(glm::vec3(0.0f,1.0f,0.0f),glm::normalize(align));
    float s = glm::length(axis);
    float c = glm::dot(glm::vec3(0.0f,1.0f,0.0f),glm::normalize(align));

    if(c >= 0.0f){
        rotation = glm::rotate(glm::mat4(1.0f),float(180*asin(s)/PI),axis);
    }else if(c < 0.0f){
        rotation = glm::rotate(glm::mat4(1.0f),float(180-180*asin(s)/PI),axis);
    }

    destroy = false;
    skipBones = true;
    skipMVP = false;
    skipLighting = false;

    collisionTypeValue = COLLIDER;
}

ProjectileObject::~ProjectileObject(){}

ProjectileObject::ProjectileObject(const ProjectileObject & rhs){}

void ProjectileObject::move(double deltatime, Camera* player, std::list<GameObject*>* levelObjects){
    movement = float(deltatime)*velocity;
}

void ProjectileObject::render(GameState* state){
        renderMesh(data->VertexBuffer,data->ElementBuffer,data->ElementCount,state);
}
