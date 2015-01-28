#include "GameObject.h"

Character::Character(){}

Character::Character(char* objectfile, char* boundsfile, float x, float y, float z, float roty, GameState* state){

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
    front = glm::rotateY(glm::vec3(0.0f,0.0f,1.0f),roty);
    right = glm::rotateY(glm::vec3(1.0f,0.0f,0.0f),roty);
    velocity = glm::vec3(0.0f,0.0f,0.0f);
    movementSpeedFactor = 0.5f;
    ground = false;
    health = 5.0f;
    destroy = false;
    skipBones = false;
    skipMVP = false;
    skipLighting = false;

    collisionTypeValue = COLLIDER_COLLIDEE;
    charAnimTime = 0;
}

Character::~Character(){}

Character::Character(const Character & rhs){}

void Character::AIDecision(double deltatime, Camera* player, std::list<GameObject*>* levelObjects){

    glm::vec4 allowed, temp;
    movement = glm::vec3(0.0f,0.0f,0.0f);
    allowed = glm::vec4(0.0f,0.0f,0.0f,0.0f);
    temp = glm::vec4(0.0f,0.0f,0.0f,0.0f);

    currentTarget = player->getPosition();

    glm::vec3 vectorToTarget = currentTarget - position;
    vectorToTarget = glm::vec3(vectorToTarget.x,0.0f,vectorToTarget.z);
    double frontDotProduct = glm::dot(vectorToTarget,front)/glm::length(vectorToTarget);
    double rightDotProduct = glm::dot(vectorToTarget,right)/glm::length(vectorToTarget);

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
    front = glm::rotateY(glm::vec3(0.0f,0.0f,1.0f),rotY);
    right = glm::rotateY(glm::vec3(1.0f,0.0f,0.0f),rotY);

    velocity.y -= GRAVITATIONAL_ACCELERATION*deltatime;

    movement.x = movementSpeedFactor * deltatime * front.x;
    movement.y = deltatime * velocity.y;
    movement.z = movementSpeedFactor * deltatime * front.z;

}

void Character::move(double deltatime, Camera* player, std::list<GameObject*>* levelObjects){
    charAnimTime +=  deltatime;
    float TicksPerSecond = (float)(data->Importer->GetScene()->mAnimations[0]->mTicksPerSecond != 0 ? data->Importer->GetScene()->mAnimations[0]->mTicksPerSecond : 25.0f);
    float TimeInTicks = charAnimTime * TicksPerSecond;
    float AnimationTime = std::fmod(TimeInTicks, (float)data->Importer->GetScene()->mAnimations[0]->mDuration);
    boneTransform(AnimationTime);

    AIDecision(deltatime,player,levelObjects);
}

void Character::render(GameState* state){
    renderMesh(data->VertexBuffer,data->ElementBuffer,data->ElementCount,state);
}

void Character::boneTransform(float animationTime){
    glm::mat4 identity = glm::mat4(1.0f);
    readNodeHeirarchy(animationTime, data->Importer->GetScene()->mRootNode, identity);
}

void Character::readNodeHeirarchy(float animationTime, const aiNode* pNode, glm::mat4 parentTransform){
    unsigned int i;
    std::string nodeName(pNode->mName.data);

    const aiAnimation* pAnimation = data->Importer->GetScene()->mAnimations[0];
    float temp[16] = {pNode->mTransformation.a1,pNode->mTransformation.a2,pNode->mTransformation.a3,pNode->mTransformation.a4,
                        pNode->mTransformation.b1,pNode->mTransformation.b2,pNode->mTransformation.b3,pNode->mTransformation.b4,
                        pNode->mTransformation.c1,pNode->mTransformation.c2,pNode->mTransformation.c3,pNode->mTransformation.c4,
                        pNode->mTransformation.d1,pNode->mTransformation.d2,pNode->mTransformation.d3,pNode->mTransformation.d4};
    glm::mat4 nodeTransformation = glm::make_mat4(temp);
    nodeTransformation = glm::transpose(nodeTransformation);
    const aiNodeAnim* pNodeAnim = findNodeAnim(pAnimation,nodeName);

    if (pNodeAnim) {
        aiQuaternion RotationQ;
        CalcInterpolatedRotation(RotationQ, animationTime, pNodeAnim);
        glm::quat glmRotationQ(RotationQ.w,RotationQ.x,RotationQ.y,RotationQ.z);
        glm::mat4 rotationMat = glm::mat4_cast(glmRotationQ);

        aiVector3D Translation;
        CalcInterpolatedPosition(Translation, animationTime, pNodeAnim);
        glm::vec3 trans(Translation.x,Translation.y,Translation.z);
        glm::mat4 translationMat = glm::translate(glm::mat4(1.0f),trans);

        nodeTransformation = translationMat * rotationMat;
    }


    glm::mat4 globalTransformation =  parentTransform * nodeTransformation;

        if(data->boneMap.find(nodeName) != data->boneMap.end()){
            unsigned int index = data->boneMap[nodeName];
            boneTransformations[index] = data->globalInverseTransform * globalTransformation * data->bones[index].boneOffset;
        }


    for (i=0;i<pNode->mNumChildren;i++){
        readNodeHeirarchy(animationTime, pNode->mChildren[i], globalTransformation);
    }

}

void Character::CalcInterpolatedRotation(aiQuaternion& Out, float AnimationTime, const aiNodeAnim* pNodeAnim)
{
    if (pNodeAnim->mNumRotationKeys == 1) {
        Out = pNodeAnim->mRotationKeys[0].mValue;
        return;
    }

    unsigned int RotationIndex = findRotation(AnimationTime,pNodeAnim);
    assert(pNodeAnim->mNumRotationKeys > 0);

    unsigned int NextRotationIndex = (RotationIndex + 1);
    assert(NextRotationIndex < pNodeAnim->mNumRotationKeys);
    float DeltaTime = (float)(pNodeAnim->mRotationKeys[NextRotationIndex].mTime - pNodeAnim->mRotationKeys[RotationIndex].mTime);
    float Factor = (AnimationTime - (float)pNodeAnim->mRotationKeys[RotationIndex].mTime) / DeltaTime;
    assert(Factor >= 0.0f && Factor <= 1.0f);
    const aiQuaternion& StartRotationQ = pNodeAnim->mRotationKeys[RotationIndex].mValue;
    const aiQuaternion& EndRotationQ   = pNodeAnim->mRotationKeys[NextRotationIndex].mValue;
    aiQuaternion::Interpolate(Out, StartRotationQ, EndRotationQ, Factor);
    Out = Out.Normalize();
}

void Character::CalcInterpolatedPosition(aiVector3D& Out, float AnimationTime, const aiNodeAnim* pNodeAnim)
{
    if (pNodeAnim->mNumPositionKeys == 1) {
        Out = pNodeAnim->mPositionKeys[0].mValue;
        return;
    }

    unsigned int PositionIndex = findPosition(AnimationTime,pNodeAnim);

    unsigned NextPositionIndex = (PositionIndex + 1);
    assert(NextPositionIndex < pNodeAnim->mNumPositionKeys);
    float DeltaTime = (float)(pNodeAnim->mPositionKeys[NextPositionIndex].mTime - pNodeAnim->mPositionKeys[PositionIndex].mTime);
    float Factor = (AnimationTime - (float)pNodeAnim->mPositionKeys[PositionIndex].mTime) / DeltaTime;
    assert(Factor >= 0.0f && Factor <= 1.0f);
    const aiVector3D& Start = pNodeAnim->mPositionKeys[PositionIndex].mValue;
    const aiVector3D& End = pNodeAnim->mPositionKeys[NextPositionIndex].mValue;
    aiVector3D Delta = End - Start;
    Out = Start + Factor * Delta;
}

const aiNodeAnim* Character::findNodeAnim(const aiAnimation* pAnimation, const std::string NodeName)
{
    for (unsigned int i = 0 ; i < pAnimation->mNumChannels ; i++) {
        const aiNodeAnim* pNodeAnim = pAnimation->mChannels[i];

        if (std::string(pNodeAnim->mNodeName.data) == NodeName) {
            return pNodeAnim;
        }
    }

    return NULL;
}

unsigned int Character::findPosition(float AnimationTime, const aiNodeAnim* pNodeAnim)
{
    for (unsigned int i = 0 ; i < pNodeAnim->mNumPositionKeys - 1 ; i++) {
        if (AnimationTime < (float)pNodeAnim->mPositionKeys[i + 1].mTime) {
            return i;
        }
    }

    assert(0);

    return 0;
}


unsigned int Character::findRotation(float AnimationTime, const aiNodeAnim* pNodeAnim)
{
    assert(pNodeAnim->mNumRotationKeys > 0);

    for (unsigned int i = 0 ; i < pNodeAnim->mNumRotationKeys - 1 ; i++) {
        if (AnimationTime < (float)pNodeAnim->mRotationKeys[i + 1].mTime) {
            return i;
        }
    }

    assert(0);

    return 0;
}

void Character::damage(float magnitude, glm::vec3 damageLocation, GameState* state){
    health -= magnitude;
    GameObject* newobject = new ParticleSource("Data/BloodDrop.3ds", damageLocation, glm::vec3(1.0f,1.0f,1.0f), 100, 0.5f,false,0, true, state);
    addOpaqueObject(newobject,state);
}
