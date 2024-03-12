#include "GameObjects/GameObject.h"
#include "GameObjects/Bullet.h"

GameObject::GameObject() :
    position(0.0f, 0.0f, 0.0f),
    rotY(0.0f),
    movement(0.0f, 0.0f, 0.0f),
    velocity(0.0f, 0.0f, 0.0f),
    translation(mat4(1.0f)),
    rotation(mat4(1.0f)) {}

void GameObject::testResolveCollision(GameObject* object1, GameObject* object2){

    unsigned int i, j, k;
    vec3 axes[12];
    vec3 smallest, totalCorrection1, totalCorrection2;
    float overlap = 0.0;
    bool collision;

    boundary* obj1Bounds = object1->getBounds();
    boundary* obj2Bounds = object2->getBounds();
    if ((!obj1Bounds && object1->collisionType() != RAY) ||
        (!obj2Bounds && object2->collisionType() != RAY)) return;

    if(object1->collisionType() != NONE && object2->collisionType() != NONE){

    if(object1->collisionType() != RAY && object2->collisionType() != RAY){

        totalCorrection1 = vec3(0.0f,0.0f,0.0f);
        totalCorrection2 = vec3(0.0f,0.0f,0.0f);

        for(i=0;i< obj1Bounds->blocks.size();i++){
            for(j=0;j< obj2Bounds->blocks.size();j++){

                block b1 = obj1Bounds->blocks[i];
                block b2 = obj2Bounds->blocks[j];

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
                vec3 newPosition1 = object1->getPosition()+object1->getMovement();
                vec3 newPosition2 = object2->getPosition()+object2->getMovement();
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

                    float checkSign = dot(smallest,(newPosition1+b1.getPosition())-(newPosition2+b2.getPosition()));

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
        boundary* hitBounds = hitObject->getBounds();

        if(static_cast<Bullet*>(ray)->getParent() != hitObject){

        for(i=0;i< hitBounds->blocks.size();i++){
                block b = hitBounds->blocks[i];
                projection p_l = b.project(b.lengthnorm,hitObject->getPosition(),hitObject->getRotation());
                projection p_w = b.project(b.widthnorm,hitObject->getPosition(),hitObject->getRotation());
                projection p_h = b.project(b.heightnorm,hitObject->getPosition(),hitObject->getRotation());

                float p1_l, p2_l, p1_w, p2_w, p1_h, p2_h;
                projection l_t, w_t, h_t;

                p1_l = dot(ray->position,b.lengthnorm);
                p2_l = dot(ray->movement,b.lengthnorm);


                p1_w = dot(ray->position,b.widthnorm);
                p2_w = dot(ray->movement,b.widthnorm);


                p1_h = dot(ray->position,b.heightnorm);
                p2_h = dot(ray->movement,b.heightnorm);



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
