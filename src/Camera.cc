#include <SDL/SDL.h>
#include "GameObject.h"

Camera::Camera(float theWindowWidth, float theWindowHeight, char* boundsfile, float x, float y, float z, float roty, GameState* state)
{
    unsigned int i, j, k;
    aiString path;
    char fullpath[200];
    unsigned int boneId;
    unsigned int numBones = 0;

	initCamera();

	position = glm::vec3(x,y,z);
	head = position + glm::vec3(0.0f,0.3f,0.0f);
	origin = head + glm::vec3(0.0f,0.0f,1000.0f);

	origin = glm::rotateY(origin-head,-1*roty);
	origin = origin + head;

	front = glm::rotateY(front,-1*roty);
	right = glm::rotateY(right,-1*roty);

    GLuint vertexbuffer, elementbuffer;

    Assimp::Importer* importer = new Assimp::Importer();
    Assimp::Importer* weaponImporter = new Assimp::Importer();
	boundary* objectBounds = new boundary;
	ObjectData* newObjectData = new ObjectData;
	weaponData = new ObjectData;

        vertex *vertex_data = (vertex*) malloc(4 * sizeof(vertex));

        vertex_data[0].position[0] = -1.0f;
        vertex_data[0].position[1] = -1.0f;
        vertex_data[0].position[2] = -1.0f;

        vertex_data[1].position[0] = 1.0f;
        vertex_data[1].position[1] = -1.0f;
        vertex_data[1].position[2] = -1.0f;

        vertex_data[2].position[0] = 1.0f;
        vertex_data[2].position[1] = 1.0f;
        vertex_data[2].position[2] = -1.0f;

        vertex_data[3].position[0] = -1.0f;
        vertex_data[3].position[1] = 1.0f;
        vertex_data[3].position[2] = -1.0f;


        vertex_data[0].texcoord[0] = 0.0f;
        vertex_data[0].texcoord[1] = 1.0f;

        vertex_data[1].texcoord[0] = 1.0f;
        vertex_data[1].texcoord[1] = 1.0f;

        vertex_data[2].texcoord[0] = 1.0f;
        vertex_data[2].texcoord[1] = 0.0f;

        vertex_data[3].texcoord[0] = 0.0f;
        vertex_data[3].texcoord[1] = 0.0f;


		glGenBuffers(1, &vertexbuffer);
		glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
		glBufferData(GL_ARRAY_BUFFER, 4*sizeof(vertex), vertex_data, GL_STATIC_DRAW);

        free(vertex_data);

    billboardVertexBuffer = vertexbuffer;

        GLuint *element_data = (GLuint*) malloc(6 * sizeof(GLuint));

        element_data[0] = 0;
        element_data[1] = 1;
        element_data[2] = 2;
        element_data[3] = 0;
        element_data[4] = 2;
        element_data[5] = 3;

        glGenBuffers(1, &elementbuffer);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementbuffer);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, 6*sizeof(GLuint) , element_data, GL_STATIC_DRAW);

        free(element_data);

    billboardElementBuffer = elementbuffer;

    billboardElementCount = 6;

    crosshairsTexture = SOIL_load_OGL_texture("Data/crosshairs.tga",SOIL_LOAD_RGBA,SOIL_CREATE_NEW_ID,SOIL_FLAG_TEXTURE_REPEATS | SOIL_FLAG_INVERT_Y | SOIL_FLAG_COMPRESS_TO_DXT);
    crosshairsScale = glm::scale(glm::mat4(1.0f),glm::vec3( 0.05f* theWindowHeight / theWindowWidth , 0.05f, 1.0f) );

    SDL_Color text_color = {0, 127, 0};
    GLuint texture_format;
    healthFont = TTF_OpenFont("Data/carbon.ttf", 32);
    SDL_Surface* surface = TTF_RenderText_Blended(healthFont,"100",text_color);

    unsigned int colors = surface->format->BytesPerPixel;
    if (colors == 4) {   // alpha
        if (surface->format->Rmask == 0x000000ff)
            texture_format = GL_RGBA;
        else
            texture_format = GL_BGRA;
    } else {             // no alpha
        if (surface->format->Rmask == 0x000000ff)
            texture_format = GL_RGB;
        else
            texture_format = GL_BGR;
    }

    glGenTextures(1, &healthTexture);
    glBindTexture(GL_TEXTURE_2D, healthTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, colors, surface->w, surface->h, 0, texture_format, GL_UNSIGNED_BYTE, surface->pixels);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    healthTranslation = glm::translate(glm::mat4(1.0f),glm::vec3(-1.0f,-1.0f,0.0f));
    healthTranslation *= glm::scale(glm::mat4(1.0f),glm::vec3( 0.1f * (1.75f) * (theWindowHeight / theWindowWidth), 0.1f, 1.0f));
    healthTranslation *= glm::translate(glm::mat4(1.0f),glm::vec3(1.0f,1.0f,0.0f));

    gunfireTexture = SOIL_load_OGL_texture("Data/GF.tga",SOIL_LOAD_RGBA,SOIL_CREATE_NEW_ID,SOIL_FLAG_TEXTURE_REPEATS | SOIL_FLAG_INVERT_Y | SOIL_FLAG_COMPRESS_TO_DXT);
    gunfireTranslation = glm::translate(glm::mat4(1.0f),glm::vec3(0.08f,-0.2,0.0f));
    gunfireTranslation *= glm::scale(glm::mat4(1.0f),glm::vec3(0.25f * (theWindowHeight / theWindowWidth),0.25f,1.0f));

    const aiScene* cpScene = weaponImporter->ReadFile("Data/gun.md5mesh",aiProcess_GenSmoothNormals);
    aiMesh* pMesh = cpScene->mMeshes[0];

            aiMatrix4x4 m_GlobalInverseTransform = cpScene->mRootNode->mTransformation;
            m_GlobalInverseTransform.Inverse();
            float temp[16]={m_GlobalInverseTransform.a1,m_GlobalInverseTransform.a2,m_GlobalInverseTransform.a3,m_GlobalInverseTransform.a4,
                            m_GlobalInverseTransform.b1,m_GlobalInverseTransform.b2,m_GlobalInverseTransform.b3,m_GlobalInverseTransform.b4,
                            m_GlobalInverseTransform.c1,m_GlobalInverseTransform.c2,m_GlobalInverseTransform.c3,m_GlobalInverseTransform.c4,
                            m_GlobalInverseTransform.d1,m_GlobalInverseTransform.d2,m_GlobalInverseTransform.d3,m_GlobalInverseTransform.d4};
            weaponData->globalInverseTransform = glm::make_mat4(temp);
            weaponData->globalInverseTransform = glm::transpose(weaponData->globalInverseTransform);

            vertex_data = (vertex*) malloc( pMesh->mNumVertices * sizeof(vertex));
            for(i=0;i<pMesh->mNumVertices;i++){
                    vertex newVertex;
                    newVertex.position[0] = pMesh->mVertices[i].x;
                    newVertex.position[1] = pMesh->mVertices[i].y;
                    newVertex.position[2] = pMesh->mVertices[i].z;

                    newVertex.normal[0] = pMesh->mNormals[i].x;
                    newVertex.normal[1] = pMesh->mNormals[i].y;
                    newVertex.normal[2] = pMesh->mNormals[i].z;

                    newVertex.texcoord[0] = pMesh->mTextureCoords[0][i].x;
                    newVertex.texcoord[1] = pMesh->mTextureCoords[0][i].y;
                    vertex_data[i] = newVertex;
            }

            for(i=0;i<pMesh->mNumBones;i++){
            boneId = 0;
            std::string thisBoneName(pMesh->mBones[i]->mName.data);

            if(weaponData->boneMap.find(thisBoneName) == weaponData->boneMap.end()){
                    BoneInfo newBone;
                    boneId = numBones;
                    numBones++;
                    newBone.boneId = boneId;
                    float temp[16] = { pMesh->mBones[i]->mOffsetMatrix.a1,pMesh->mBones[i]->mOffsetMatrix.a2,pMesh->mBones[i]->mOffsetMatrix.a3,pMesh->mBones[i]->mOffsetMatrix.a4,
                                        pMesh->mBones[i]->mOffsetMatrix.b1,pMesh->mBones[i]->mOffsetMatrix.b2,pMesh->mBones[i]->mOffsetMatrix.b3,pMesh->mBones[i]->mOffsetMatrix.b4,
                                        pMesh->mBones[i]->mOffsetMatrix.c1,pMesh->mBones[i]->mOffsetMatrix.c2,pMesh->mBones[i]->mOffsetMatrix.c3,pMesh->mBones[i]->mOffsetMatrix.c4,
                                        pMesh->mBones[i]->mOffsetMatrix.d1,pMesh->mBones[i]->mOffsetMatrix.d2,pMesh->mBones[i]->mOffsetMatrix.d3,pMesh->mBones[i]->mOffsetMatrix.d4};
                    newBone.boneOffset = glm::make_mat4(temp);
                    newBone.boneOffset = glm::transpose(newBone.boneOffset);
                    weaponData->bones.push_back(newBone);
                    weaponData->boneMap[thisBoneName] = boneId;
            }else{
                boneId = weaponData->boneMap[thisBoneName];
            }

            for(j=0;j<pMesh->mBones[i]->mNumWeights;j++){
                unsigned int VertexID = pMesh->mBones[i]->mWeights[j].mVertexId;
                for(k=0;k<4;k++){
                    if(vertex_data[VertexID].boneWeights[k] == 0.0){
                        vertex_data[VertexID].boneIds[k] = boneId;
                        vertex_data[VertexID].boneWeights[k] = pMesh->mBones[i]->mWeights[j].mWeight;
                        break;
                    }
                }
            }

            }

            glGenBuffers(1, &weaponVertexBuffer);
            glBindBuffer(GL_ARRAY_BUFFER, weaponVertexBuffer);
            glBufferData(GL_ARRAY_BUFFER, (pMesh->mNumVertices)*sizeof(vertex), vertex_data, GL_STATIC_DRAW);
            free(vertex_data);

            element_data = (GLuint*) malloc( (pMesh->mNumFaces * 3) * sizeof(GLuint));

            for(i=0;i<(pMesh->mNumFaces);i++){
                element_data[3*i] = pMesh->mFaces[i].mIndices[0];
                element_data[3*i+1] = pMesh->mFaces[i].mIndices[1];
                element_data[3*i+2] = pMesh->mFaces[i].mIndices[2];
            }


            glGenBuffers(1, &weaponElementBuffer);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, weaponElementBuffer);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, (pMesh->mNumFaces * 3) *sizeof(GLuint) , element_data, GL_STATIC_DRAW);
            free(element_data);

            weaponElementCount = cpScene->mMeshes[0]->mNumFaces * 3;

            cpScene->mMaterials[0]->GetTexture(aiTextureType_DIFFUSE, 0, &path, NULL, NULL, NULL, NULL, NULL);
            sprintf(fullpath,"Data/%s",path.data);
            weaponTexture = SOIL_load_OGL_texture(fullpath,SOIL_LOAD_RGBA,SOIL_CREATE_NEW_ID,SOIL_FLAG_TEXTURE_REPEATS | SOIL_FLAG_INVERT_Y | SOIL_FLAG_COMPRESS_TO_DXT);

            weaponData->VertexBuffer = weaponVertexBuffer;
            weaponData->ElementBuffer = weaponElementBuffer;
            weaponData->ElementCount = weaponElementCount;
            weaponData->TextureId = weaponTexture;
            weaponData->Importer = weaponImporter;

	objectBounds->LoadBoundaries(std::string(boundsfile));
	newObjectData->objectBounds = objectBounds;
	newObjectData->Importer = importer;
    objectMap[std::string("Camera")] = newObjectData;
    data = newObjectData;

    destroy = false;

    skipBones = true;
    skipMVP = true;
    skipLighting = true;

    flashLife = 0.0f;

    cDistance = 0.0f;

    collisionTypeValue = COLLIDER_COLLIDEE;

	windowWidth  = theWindowWidth;
	windowHeight = theWindowHeight;

	// Calculate the middle of the window
	windowMidX = windowWidth  / 2.0f;
	windowMidY = windowHeight / 2.0f;

	SDL_WarpMouse(windowMidX,windowMidY);
}

Camera::~Camera(){}

void Camera::initCamera()
{
	// Set position, rotation and speed values to zero
	position = glm::vec3(0.0f,0.0f,0.0f);
	head = glm::vec3(0.0f,0.0f,0.0f);
	origin = glm::vec3(0.0f,0.0f,0.0f);
	front = glm::vec3(0.0f,0.0f,1.0f);
	right = glm::vec3(-1.0f,0.0f,0.0f);
	up = glm::vec3(0.0f, 1.0f, 0.0f);
	velocity = glm::vec3(0.0f,0.0f,0.0f);

    rotHorizontal = 0.0f;
    rotVertical = 0.0f;

    cameraAnimTime = 0.0f;

	// How fast we move (higher values mean we move and strafe faster)
	movementSpeedFactor = 1.2f;

	pitchSensitivity = 0.1; // How sensitive mouse movements affect looking up and down
	yawSensitivity   = 0.1; // How sensitive mouse movements affect looking left and right

	ground = false;

	// To begin with, we aren't holding down any keys
	holdingForward     = false;
	holdingBackward    = false;
	holdingLeftStrafe  = false;
	holdingRightStrafe = false;

	playerIsFiring = false;

}

// Function to deal with mouse position changes
void Camera::handleMouseMove(int mouseX, int mouseY)
{
    glm::vec3 temp;
	// Calculate our horizontal and vertical mouse movement from middle of the window
	float horizMovement = (mouseX - windowMidX) * yawSensitivity;
	float vertMovement  = (mouseY - windowMidY) * pitchSensitivity;

	// Apply the mouse movement to vector indicating our view direction
	rotHorizontal -= horizMovement;
	if(rotHorizontal <= 360.0f) rotHorizontal += 360.0f;
	if(rotHorizontal >= 360.0f) rotHorizontal -= 360.0f;
	origin = glm::rotateY(origin-head,-1*horizMovement);
	origin = origin + head;

	front = glm::rotateY(front,-1*horizMovement);
	right = glm::rotateY(right,-1*horizMovement);
	up = glm::rotateY(right,-1*horizMovement);

    temp = glm::rotate(origin-head,-1*vertMovement,right);
    if(fabs(glm::dot(glm::normalize(temp),glm::vec3(0.0f,1.0f,0.0f))) < 0.8f){
        origin = temp + head;
        rotVertical -= vertMovement;
        if(rotVertical <= 360.0f) rotVertical += 360.0f;
        if(rotVertical >= 360.0f) rotVertical -= 360.0f;
    }

	up = glm::rotate(up,-1*vertMovement,right);

	// Reset the mouse position to the centre of the window each frame
	SDL_WarpMouse(windowMidX,windowMidY);
}

// Function to calculate which direction we need to move the camera and by what amount
void Camera::move(double deltatime, Camera* player, std::list<GameObject*>* levelObjects)
{
	// Vector to break up our movement into components along the X, Y and Z axis
	movement = glm::vec3(0.0f,0.0f,0.0f);
	// Vector that resolves an invalid attempted movement
	glm::vec4 allowed(0.0f,0.0f,0.0f,0.0f);
	glm::vec4 temp(0.0f,0.0f,0.0f,0.0f);

	if (holdingForward)
	{
		movement.x += front.x;
		movement.z += front.z;
	}

	if (holdingBackward)
	{
		movement.x += -1*front.x;
		movement.z += -1*front.z;
	}

	if (holdingRightStrafe)
	{
		movement.x += right.x;
		movement.z += right.z;
	}

	if (holdingLeftStrafe)
	{
		movement.x += -1*right.x;
		movement.z += -1*right.z;
	}
	velocity.y -= GRAVITATIONAL_ACCELERATION*deltatime;

	movement.x = movementSpeedFactor * deltatime * movement.x;
    movement.y = deltatime * velocity.y;
    movement.z = movementSpeedFactor * deltatime * movement.z;

    //now we deal with the weapon animations
    if(playerIsFiring){
        cameraAnimTime += 0.15f*deltatime;
        flashLife -= 5.0f * deltatime;
    }
    if(cameraAnimTime > 0.083f){
        cameraAnimTime = 0.0f;
        playerIsFiring = false;
    }
    float TicksPerSecond = (float)(weaponData->Importer->GetScene()->mAnimations[0]->mTicksPerSecond != 0 ? weaponData->Importer->GetScene()->mAnimations[0]->mTicksPerSecond : 25.0f);
    float TimeInTicks = cameraAnimTime * TicksPerSecond;
    float AnimationTime = std::fmod(TimeInTicks, (float)weaponData->Importer->GetScene()->mAnimations[0]->mDuration);
    boneTransform(AnimationTime);

}

void Camera::render(GameState* state){

    skipMVP = true;
    skipLighting = true;
    skipBones = true;
    rotation = glm::mat4(1.0f);
    glBindTexture(GL_TEXTURE_2D,crosshairsTexture);
    translation = crosshairsScale;
    renderMesh(billboardVertexBuffer,billboardElementBuffer,billboardElementCount,state);

    glBindTexture(GL_TEXTURE_2D,healthTexture);
    translation = healthTranslation;
    renderMesh(billboardVertexBuffer,billboardElementBuffer,billboardElementCount,state);

    if(flashLife > 0.0f){
        glBindTexture(GL_TEXTURE_2D,gunfireTexture);
        translation = gunfireTranslation;
        renderMesh(billboardVertexBuffer,billboardElementBuffer,billboardElementCount,state);
    }

    skipMVP = false;
    skipLighting = false;
    skipBones = false;
    glClear(GL_DEPTH_BUFFER_BIT);
    glBindTexture(GL_TEXTURE_2D,weaponTexture);
    translation = glm::translate(glm::mat4(1.0f),position + 0.2f*right);
    rotation = glm::rotate(glm::mat4(1.0f),rotVertical,right) * glm::rotate(glm::mat4(1.0f),rotHorizontal+2.0f,glm::vec3(0.0f,1.0f,0.0f));
    renderMesh(weaponVertexBuffer,weaponElementBuffer,weaponElementCount,state);
}

void Camera::boneTransform(float animationTime){
    glm::mat4 identity = glm::mat4(1.0f);
    readNodeHeirarchy(animationTime, weaponData->Importer->GetScene()->mRootNode, identity);
}

void Camera::readNodeHeirarchy(float animationTime, const aiNode* pNode, glm::mat4 parentTransform){
    unsigned int i;
    std::string nodeName(pNode->mName.data);

    const aiAnimation* pAnimation = weaponData->Importer->GetScene()->mAnimations[0];
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

        if(weaponData->boneMap.find(nodeName) != weaponData->boneMap.end()){
            unsigned int index = weaponData->boneMap[nodeName];
            boneTransformations[index] = weaponData->globalInverseTransform * globalTransformation * weaponData->bones[index].boneOffset;
        }

    for (i=0;i<pNode->mNumChildren;i++){
        readNodeHeirarchy(animationTime, pNode->mChildren[i], globalTransformation);
    }

}

void Camera::CalcInterpolatedRotation(aiQuaternion& Out, float AnimationTime, const aiNodeAnim* pNodeAnim)
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

void Camera::CalcInterpolatedPosition(aiVector3D& Out, float AnimationTime, const aiNodeAnim* pNodeAnim)
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

const aiNodeAnim* Camera::findNodeAnim(const aiAnimation* pAnimation, const std::string NodeName)
{
    for (unsigned int i = 0 ; i < pAnimation->mNumChannels ; i++) {
        const aiNodeAnim* pNodeAnim = pAnimation->mChannels[i];

        if (std::string(pNodeAnim->mNodeName.data) == NodeName) {
            return pNodeAnim;
        }
    }

    return NULL;
}

unsigned int Camera::findPosition(float AnimationTime, const aiNodeAnim* pNodeAnim)
{
    for (unsigned int i = 0 ; i < pNodeAnim->mNumPositionKeys - 1 ; i++) {
        if (AnimationTime < (float)pNodeAnim->mPositionKeys[i + 1].mTime) {
            return i;
        }
    }

    assert(0);

    return 0;
}


unsigned int Camera::findRotation(float AnimationTime, const aiNodeAnim* pNodeAnim)
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
