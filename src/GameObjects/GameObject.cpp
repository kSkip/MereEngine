#include "GameObjects/GameObject.h"
#include "GameObjects/Bullet.h"

GameObject::GameObject(){}

GameObject::~GameObject(){}

GameObject::GameObject(const GameObject & rhs){}

void GameObject::move(double deltatime, Camera* player, std::list<GameObject*>* levelObjects){}

void GameObject::render(GameState* state){}

void GameObject::addOpaqueObject(GameObject* newObject, GameState* state){

    state->insertOpaqueObject(newObject);

}

void GameObject::addTransparentObject(GameObject* newObject, GameState* state){

    state->insertTransparencyObject(newObject);

}

void GameObject::renderMeshElements(GLuint vertex_buffer, GLuint vertex_element_buffer, GLsizei element_count, Primative type, GameState* state){

    GLenum draw_mode;

    switch(type){
        case PTYPE_TRIANGLES:
            draw_mode = GL_TRIANGLES;
            break;
        case PTYPE_POINTS:
            draw_mode = GL_POINTS;
            break;
        default:
            draw_mode = GL_TRIANGLES;
    }

    glUniformMatrix4fv(state->levelShader->translation,1, GL_FALSE,glm::value_ptr(this->translation));
    glUniformMatrix4fv(state->levelShader->rotation,1, GL_FALSE,glm::value_ptr(this->rotation));

    glUniform1ui(state->levelShader->skipMVP,this->skipMVP);
    glUniform1ui(state->levelShader->skipLighting,this->skipLighting);

    glBindTexture(GL_TEXTURE_2D,data->diffuseTex);

    glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);

    glVertexAttribPointer(
        LOCATION_POSITION,
        3, GL_FLOAT, GL_FALSE,sizeof(struct vertex),
        (void*)offsetof(struct vertex, position)
    );
    glVertexAttribPointer(
        LOCATION_NORMAL,
        3, GL_FLOAT, GL_FALSE,sizeof(struct vertex),
        (void*)offsetof(struct vertex, normal)
    );
    glVertexAttribPointer(
        LOCATION_TEXCOORD,
        2, GL_FLOAT, GL_FALSE, sizeof(struct vertex),
        (void*)offsetof(struct vertex, texcoord)
    );

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vertex_element_buffer);

    glDrawElements(
        draw_mode,
        element_count,
        GL_UNSIGNED_INT,
        (void*)0
    );

}

void GameObject::renderMeshArrays(GLuint vertex_buffer, GLsizei element_count, Primative type, GameState* state){

    GLenum draw_mode;

    switch(type){
        case PTYPE_TRIANGLES:
            draw_mode = GL_TRIANGLES;
            break;
        case PTYPE_POINTS:
            draw_mode = GL_POINTS;
            break;
        default:
            draw_mode = GL_TRIANGLES;
    }

    glUniformMatrix4fv(state->levelShader->translation,1, GL_FALSE,glm::value_ptr(this->translation));
    glUniformMatrix4fv(state->levelShader->rotation,1, GL_FALSE,glm::value_ptr(this->rotation));

    glUniform1ui(state->levelShader->skipMVP,this->skipMVP);
    glUniform1ui(state->levelShader->skipLighting,this->skipLighting);

    glBindTexture(GL_TEXTURE_2D,data->diffuseTex);

    glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);

    glVertexAttribPointer(
        LOCATION_POSITION,
        3, GL_FLOAT, GL_FALSE,sizeof(struct vertex),
        (void*)offsetof(struct vertex, position)
    );
    glVertexAttribPointer(
        LOCATION_NORMAL,
        3, GL_FLOAT, GL_FALSE,sizeof(struct vertex),
        (void*)offsetof(struct vertex, normal)
    );
    glVertexAttribPointer(
        LOCATION_TEXCOORD,
        2, GL_FLOAT, GL_FALSE, sizeof(struct vertex),
        (void*)offsetof(struct vertex, texcoord)
    );

    glDrawArrays(
        draw_mode,
        0,
        element_count
    );

}

void GameObject::damage(float magnitude, glm::vec3 damageLocation, GameState* state){

}

void GameObject::testResolveCollision(GameObject* object1, GameObject* object2){

    unsigned int i, j, k;
    glm::vec3 axes[12];
    glm::vec3 smallest, totalCorrection1, totalCorrection2;
    float overlap;
    bool collision;

    if(object1->collisionType() != NONE && object2->collisionType() != NONE){

    if(object1->collisionType() != RAY && object2->collisionType() != RAY){

        totalCorrection1 = glm::vec3(0.0f,0.0f,0.0f);
        totalCorrection2 = glm::vec3(0.0f,0.0f,0.0f);

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
                        if(o < overlap || k == 0){
                            overlap = float(o);
                            smallest = axes[k];
                        }
                    }
                }

                if(collision){

                    float checkSign = glm::dot(smallest,(newPosition1+b1.getPosition())-(newPosition2+b2.getPosition()));

                    //apply the MTV to the movement of either or both objects if collision occured
                    if(object1->collisionType() == COLLIDER && object2->collisionType() == COLLIDEE){
                        object1->makeDestroy();
                        if(checkSign > 0){
                            totalCorrection1 += overlap*smallest;
                        }else{
                            totalCorrection1 -= overlap*smallest;
                        }
                    }else if(object1->collisionType() == COLLIDEE && object2->collisionType() == COLLIDER){
                        object2->makeDestroy();
                        if(checkSign > 0){
                            totalCorrection2 -= overlap*smallest;
                        }else{
                            totalCorrection2 += overlap*smallest;
                        }
                    }else if(object1->collisionType() == COLLIDER && object2->collisionType() == COLLIDER_COLLIDEE){
                        object1->makeDestroy();
                        if(checkSign > 0){
                            totalCorrection1 += overlap*smallest;
                        }else{
                            totalCorrection1 -= overlap*smallest;
                        }
                    }else if(object1->collisionType() == COLLIDER_COLLIDEE && object2->collisionType() == COLLIDER){
                        object2->makeDestroy();
                        if(checkSign > 0){
                            totalCorrection2 -= overlap*smallest;
                        }else{
                            totalCorrection2 += overlap*smallest;
                        }
                    }else if(object1->collisionType() == COLLIDER_COLLIDEE && object2->collisionType() == COLLIDEE){
                        if(smallest.y != 0.0f){
                            object1->isGrounded();
                            object1->setYSpeed(0.0f);
                        }
                        if(checkSign > 0){
                            totalCorrection1 += overlap*smallest;
                        }else{
                            totalCorrection1 -= overlap*smallest;
                        }
                    }else if(object1->collisionType() == COLLIDEE && object2->collisionType() == COLLIDER_COLLIDEE){
                        if(smallest.y != 0.0f){
                            object2->isGrounded();
                            object2->setYSpeed(0.0f);
                        }
                        if(checkSign > 0){
                            totalCorrection2 -= overlap*smallest;
                        }else{
                            totalCorrection2 += overlap*smallest;
                        }
                    }else if(object1->collisionType() == COLLIDER_COLLIDEE && object2->collisionType() == COLLIDER_COLLIDEE){
                        if(checkSign > 0){
                            totalCorrection1 += 0.5f*overlap*smallest;
                            totalCorrection2 -= 0.5f*overlap*smallest;
                        }else{
                            totalCorrection1 -= 0.5f*overlap*smallest;
                            totalCorrection2 += 0.5f*overlap*smallest;
                        }
                    }//end of collsion type test and resolution
                }// end of code if collision occured

            }//end of loop through object2 blocks
        }//end of loop through object1 blocks

        object1->movement += totalCorrection1;
        object2->movement += totalCorrection2;

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

        if(static_cast<Bullet*>(ray)->getParent() != hitObject){

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
