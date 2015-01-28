#include "GameObject.h"

StaticObject::StaticObject(){}

StaticObject::StaticObject(char* objectfile, char* boundsfile, float x, float y, float z, float roty, GameState* state){

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

    position = glm::vec3(x,y,z);
    translation = glm::translate(glm::mat4(1.0f),position);
    rotation = glm::rotate(glm::mat4(1.0f),roty,glm::vec3(0.0,1.0,0.0));
    rotY = roty;
    collisionTypeValue = COLLIDEE;
    destroy = false;

    skipBones = true;
    skipMVP = false;
    skipLighting = false;

}

StaticObject::~StaticObject(){}

StaticObject::StaticObject(const StaticObject & rhs){}

void StaticObject::move(double deltatime, Camera* player, std::list<GameObject*>* levelObjects){}

void StaticObject::render(GameState* state){
        renderMesh(data->VertexBuffer,data->ElementBuffer,data->ElementCount,state);
}
