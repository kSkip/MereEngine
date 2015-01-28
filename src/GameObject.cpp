#include "GameObject.h"

GameObject::GameObject(){}

GameObject::~GameObject(){}

GameObject::GameObject(const GameObject & rhs){}

void GameObject::move(double deltatime, Camera* player, std::list<GameObject*>* levelObjects){}

void GameObject::render(GameState* state){}

bool GameObject::SameTexId(GLuint texid){
    if(data->TextureId == texid) {return true;}
    else {return false;}
}

GLuint GameObject::makeGetTexBuf(aiMaterial* material, GameState* state){
    GLuint newtex;
    Texture newtexstruct;
    aiString path;
    char fullpath[200];


    material->GetTexture(aiTextureType_DIFFUSE, 0, &path, NULL, NULL, NULL, NULL, NULL);
    sprintf(fullpath,"Data/%s",path.data);

    if(state->levelTextures.find(std::string(fullpath)) == state->levelTextures.end()){

        newtex = SOIL_load_OGL_texture(fullpath,SOIL_LOAD_RGBA,SOIL_CREATE_NEW_ID,SOIL_FLAG_TEXTURE_REPEATS | SOIL_FLAG_INVERT_Y | SOIL_FLAG_COMPRESS_TO_DXT);
        newtexstruct.TextureId = newtex;
        newtexstruct.TextureName = std::string(fullpath);
        state->levelTextures[std::string(fullpath)] = newtex;

        return newtex;
    }else{
        return state->levelTextures[std::string(fullpath)];
    }

}

void GameObject::addTexBuf(GLuint newtex, const char* fullpath, GameState* state){
    state->levelTextures[std::string(fullpath)] = newtex;
}

void GameObject::addOpaqueObject(GameObject* newObject, GameState* state){
    state->levelObjects.push_back(newObject);
    state->opaqueObjects.push_back(newObject);
}

void GameObject::addTransparentObject(GameObject* newObject, GameState* state){
    state->levelObjects.push_back(newObject);
    state->transparencyObjects.push_back(newObject);
}

void GameObject::clean(){
    std::map<std::string,ObjectData*>::iterator objIt;

    for(objIt = objectMap.begin(); objIt != objectMap.end(); objIt++){
        glDeleteBuffers(1,&objIt->second->VertexBuffer);
        glDeleteBuffers(1,&objIt->second->ElementBuffer);
        delete objIt->second->objectBounds;
        delete objIt->second->Importer;

    }
    objectMap.clear();
}

void GameObject::renderMesh(GLuint vertex_buffer, GLuint vertex_element_buffer, GLsizei element_count, GameState* state){
    unsigned int i;

    glUniformMatrix4fv(state->levelShader->translation,1, GL_FALSE,glm::value_ptr(this->translation));
    glUniformMatrix4fv(state->levelShader->rotation,1, GL_FALSE,glm::value_ptr(this->rotation));
    glUniform1ui(state->levelShader->skipBones,this->skipBones);
    glUniform1ui(state->levelShader->skipMVP,this->skipMVP);
    glUniform1ui(state->levelShader->skipLighting,this->skipLighting);
    for(i=0;i<20;i++){
        glUniformMatrix4fv(state->levelShader->boneTransformations[i],1, GL_FALSE,glm::value_ptr(this->boneTransformations[i]));
    }

    glUniform1i(state->levelShader->fades,this->fades());
    glUniform1i(state->levelShader->life,this->getLife());

    glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
    glVertexAttribPointer(
        state->levelShader->position,
        3, GL_FLOAT, GL_FALSE,sizeof(struct vertex),
        (void*)offsetof(struct vertex, position)
    );
    glVertexAttribPointer(
        state->levelShader->normal,
        3, GL_FLOAT, GL_FALSE,sizeof(struct vertex),
        (void*)offsetof(struct vertex, normal)
    );
    glVertexAttribPointer(
        state->levelShader->texcoord,
        2, GL_FLOAT, GL_FALSE, sizeof(struct vertex),
        (void*)offsetof(struct vertex, texcoord)
    );
    glVertexAttribIPointer(
        state->levelShader->boneIds,
        4, GL_UNSIGNED_INT, sizeof(struct vertex),
        (void*)offsetof(struct vertex, boneIds)
    );
    glVertexAttribPointer(
        state->levelShader->boneWeights,
        4, GL_FLOAT, GL_FALSE, sizeof(struct vertex),
        (void*)offsetof(struct vertex, boneWeights)
    );

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vertex_element_buffer);
    glDrawElements(
        GL_TRIANGLES,
        element_count,
        GL_UNSIGNED_INT,
        (void*)0
    );

}

GLuint GameObject::makeArrayBuffer(aiMesh* pMesh,ObjectData* objectData){
	GLuint buffer;
	unsigned int i;

	vertex *vertex_data = (vertex*) malloc( pMesh->mNumVertices * sizeof(vertex));

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

		loadBones(pMesh,vertex_data,objectData);

		glGenBuffers(1, &buffer);
		glBindBuffer(GL_ARRAY_BUFFER, buffer);
		glBufferData(GL_ARRAY_BUFFER, (pMesh->mNumVertices)*sizeof(vertex), vertex_data, GL_STATIC_DRAW);
	free(vertex_data);
	return buffer;
}

void GameObject::loadBones(const aiMesh* pMesh, vertex* vertexData, ObjectData* objectData){
    unsigned int i, j, k;
    unsigned int boneId;
    unsigned int numBones = 0;

    for(i=0;i<pMesh->mNumBones;i++){
        boneId = 0;
        std::string thisBoneName(pMesh->mBones[i]->mName.data);

        if(objectData->boneMap.find(thisBoneName) == objectData->boneMap.end()){
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
                objectData->bones.push_back(newBone);
                objectData->boneMap[thisBoneName] = boneId;
        }else{
            boneId = objectData->boneMap[thisBoneName];
        }

        for(j=0;j<pMesh->mBones[i]->mNumWeights;j++){
             unsigned int VertexID = pMesh->mBones[i]->mWeights[j].mVertexId;
             for(k=0;k<4;k++){
                if(vertexData[VertexID].boneWeights[k] == 0.0){
                    vertexData[VertexID].boneIds[k] = boneId;
                    vertexData[VertexID].boneWeights[k] = pMesh->mBones[i]->mWeights[j].mWeight;
                    break;
                }
             }
        }

    }

}

GLuint GameObject::makeElementBuffer(aiMesh* pMesh){
	GLuint buffer;
	int i;

	GLuint *element_data = (GLuint*) malloc( (pMesh->mNumFaces * 3) * sizeof(GLuint));

		for(i=0;i<(pMesh->mNumFaces);i++){
			element_data[3*i] = pMesh->mFaces[i].mIndices[0];
			element_data[3*i+1] = pMesh->mFaces[i].mIndices[1];
			element_data[3*i+2] = pMesh->mFaces[i].mIndices[2];
		}


	glGenBuffers(1, &buffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, (pMesh->mNumFaces * 3) *sizeof(GLuint) , element_data, GL_STATIC_DRAW);
	free(element_data);
	return buffer;
}

void GameObject::testResolveCollision(GameObject* object1, GameObject* object2){
    unsigned int i, j, k;
    glm::vec3 axes[12];
    glm::vec3 smallest;
    float overlap = 100000.0f;
    bool collision;

    if(object1->collisionType() != NONE && object2->collisionType() != NONE){

    if(object1->collisionType() != RAY && object2->collisionType() != RAY){

        for(i=0;i<object1->data->objectBounds->blocks.size();i++){
            for(j=0;j<object2->data->objectBounds->blocks.size();j++){

                block b1 = object1->data->objectBounds->blocks[i];
                block b2 = object2->data->objectBounds->blocks[j];

                axes[0] = b1.lengthnorm;
                axes[1] = b1.widthnorm;
                axes[2] = b1.heightnorm;
                axes[3] = -1.0f*b1.lengthnorm;
                axes[4] = -1.0f*b1.widthnorm;
                axes[5] = -1.0f*b1.heightnorm;

                axes[6] = b2.lengthnorm;
                axes[7] = b2.widthnorm;
                axes[8] = b2.heightnorm;
                axes[9] = -1.0f*b2.lengthnorm;
                axes[10] = -1.0f*b2.widthnorm;
                axes[11] = -1.0f*b2.heightnorm;

                collision = true;
                glm::vec3 newPosition1 = object1->getPosition()+object1->getMovement();
                glm::vec3 newPosition2 = object2->getPosition()+object2->getMovement();
                for(k=0;k<12;k++){
                    projection p1 = b1.project(axes[k],newPosition1,object1->getRotation());
                    projection p2 = b2.project(axes[k],newPosition2,object2->getRotation());
                    if(!p1.overlap(p2)){
                        collision = false;
                        break;
                    }else{
                        double o = p1.getOverlap(p2);
                        if(o < overlap){
                            overlap = float(o);
                            smallest = axes[k];
                        }
                    }
                }

                if(collision){

                    float checkSign = glm::dot(smallest,newPosition1-newPosition2);

                    //apply the MTV to the movement of either or both objects if collision occured
                    if(object1->collisionType() == COLLIDER && object2->collisionType() == COLLIDEE){
                        object1->makeDestroy();
                        if(checkSign > 0){
                            object1->movement = object1->movement + overlap*smallest;
                        }else{
                            object1->movement = object1->movement - overlap*smallest;
                        }
                    }else if(object1->collisionType() == COLLIDEE && object2->collisionType() == COLLIDER){
                        object2->makeDestroy();
                        if(checkSign > 0){
                            object2->movement = object2->movement - overlap*smallest;
                        }else{
                            object2->movement = object2->movement + overlap*smallest;
                        }
                    }else if(object1->collisionType() == COLLIDER && object2->collisionType() == COLLIDER_COLLIDEE){
                        object1->makeDestroy();
                        if(checkSign > 0){
                            object1->movement = object1->movement + overlap*smallest;
                        }else{
                            object1->movement = object1->movement - overlap*smallest;
                        }
                    }else if(object1->collisionType() == COLLIDER_COLLIDEE && object2->collisionType() == COLLIDER){
                        object2->makeDestroy();
                        if(checkSign > 0){
                            object2->movement = object2->movement - overlap*smallest;
                        }else{
                            object2->movement = object2->movement + overlap*smallest;
                        }
                    }else if(object1->collisionType() == COLLIDER_COLLIDEE && object2->collisionType() == COLLIDEE){
                        if(smallest.y != 0.0f){
                            object1->isGrounded();
                            object1->setYSpeed(0.0f);
                        }
                        if(checkSign > 0){
                            object1->movement = object1->movement + overlap*smallest;
                        }else{
                            object1->movement = object1->movement - overlap*smallest;
                        }
                    }else if(object1->collisionType() == COLLIDEE && object2->collisionType() == COLLIDER_COLLIDEE){
                        if(smallest.y != 0.0f){
                            object2->isGrounded();
                            object2->setYSpeed(0.0f);
                        }
                        if(checkSign > 0){
                            object2->movement = object2->movement - overlap*smallest;
                        }else{
                            object2->movement = object2->movement + overlap*smallest;
                        }
                    }else if(object1->collisionType() == COLLIDER_COLLIDEE && object2->collisionType() == COLLIDER_COLLIDEE){
                        if(checkSign > 0){
                            object1->movement = object1->movement + 0.5f*overlap*smallest;
                            object2->movement = object2->movement - 0.5f*overlap*smallest;
                        }else{
                            object1->movement = object1->movement - 0.5f*overlap*smallest;
                            object2->movement = object2->movement + 0.5f*overlap*smallest;
                        }
                    }//end of collsion type test and resolution
                }// end of code if collision occured

            }//end of loop through object2 blocks
        }//end of loop through object1 blocks

    }else{
        //do what you do for ray (aka bullets)
        if(!(object1->collisionType() == RAY && object2->collisionType() == RAY)){

        GameObject* ray;
        GameObject* hitObject;
        if(object1->collisionType() == RAY && object2->collisionType() != RAY){
            ray = object1;
            hitObject = object2;
        }else if(object1->collisionType() != RAY && object2->collisionType() == RAY){
            ray = object2;
            hitObject = object1;
        }

        if(static_cast<BulletObject*>(ray)->getParent() != hitObject){

        for(i=0;i<hitObject->data->objectBounds->blocks.size();i++){
                block b = hitObject->data->objectBounds->blocks[i];
                projection p_l = b.project(b.lengthnorm,hitObject->getPosition(),hitObject->getRotation());
                projection p_w = b.project(b.widthnorm,hitObject->getPosition(),hitObject->getRotation());
                projection p_h = b.project(b.heightnorm,hitObject->getPosition(),hitObject->getRotation());

                float p1_l, p2_l, p1_w, p2_w, p1_h, p2_h;
                projection l_t, w_t, h_t;

                p1_l = glm::dot(ray->position,b.lengthnorm);
                p2_l = glm::dot(ray->movement,b.lengthnorm);


                p1_w = glm::dot(ray->position,b.widthnorm);
                p2_w = glm::dot(ray->movement,b.widthnorm);


                p1_h = glm::dot(ray->position,b.heightnorm);
                p2_h = glm::dot(ray->movement,b.heightnorm);



                float p1_b, p2_b;
                float l_diff = p2_l - p1_l;
                float w_diff = p2_w - p1_w;
                float h_diff = p2_h - p1_h;

                p1_b = (p_l.high - p1_l) / l_diff;
                p2_b = (p_l.low - p1_l) / l_diff;
                if( p1_b < p2_b ){
                    l_t = projection(p2_b,p1_b);
                }else{
                    l_t = projection(p1_b,p2_b);
                }

                p1_b = (p_w.high - p1_w) / w_diff;
                p2_b = (p_w.low - p1_w) / w_diff;
                if( p1_b < p2_b ){
                    w_t = projection(p2_b,p1_b);
                }else{
                    w_t = projection(p1_b,p2_b);
                }

                p1_b = (p_h.high - p1_h) / h_diff;
                p2_b = (p_h.low - p1_h) / h_diff;
                if( p1_b < p2_b ){
                    h_t = projection(p2_b,p1_b);
                }else{
                    h_t = projection(p1_b,p2_b);
                }

                    if(l_t.overlap(w_t) && w_t.overlap(h_t) && h_t.overlap(l_t)){
                        float t_mins[3];

                         t_mins[0] = l_t.low;
                         t_mins[1] = w_t.low;
                         t_mins[2] = h_t.low;

                        ray->registerHit(hitObject,*std::max_element(t_mins,t_mins+2));
                    }

            }

        }//end if both objects are rays

        }

    }//now all collision types have been exhausted, if object is not one of these it's ignored
    }
}

std::map<std::string,ObjectData*> GameObject::objectMap = std::map<std::string,ObjectData*>();
