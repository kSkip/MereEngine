#ifndef GAMEOBJECT_H
#define GAMEOBJECT_H

#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <cmath>

#include <algorithm>
#include <vector>
#include <list>
#include <map>

#include "Platform.h"
#include "Boundary.h"
#include "Shader.h"

const float PI = 3.1415927f;

#define GRAVITATIONAL_ACCELERATION 5.0f

#define COLLIDER 1
#define COLLIDEE 2
#define COLLIDER_COLLIDEE 3
#define RAY 4
#define NONE 5

class GameObject {
public:
    GameObject();
    virtual ~GameObject() {}

    virtual void move(double) {};
    virtual void draw(Shader&) {}
    virtual boundary* getBounds() { return nullptr; }
    virtual void registerHit(GameObject* object, float point) {}
    virtual void commitMovement() { position += movement; }
    virtual void damage(float magnitude, vec3 damageLocation) {}

    vec3 getPosition() const { return position;}
    float getRotation() const {return rotY;}

    void setPosition(const vec3& pos) { position = pos; }
    vec3 getVelocity() const { return velocity;}
    void setXSpeed(double x)	{velocity.x = x;}
    void setYSpeed(double y)	{velocity.y = y;}
    void setZSpeed(double z)	{velocity.z = z;}

    vec3 getMovement() const { return movement;}
    double cameraDistance() const { return cDistance;}
    bool onGround() const {return ground;}
    void isGrounded(){ground = true;}
    void isNotGrounded(){ground = false;}
    bool isDestroyed(){return destroy;}
    void makeDestroy(){destroy = true;}
    unsigned int collisionType(){return collisionTypeValue;}

    static void testResolveCollision(GameObject* object1, GameObject* object2);
    static bool pGameObjectComp(const GameObject * const & a, const GameObject * const & b)
    {
        return a->cameraDistance() > b->cameraDistance();
    }

protected:
    vec3 position;
    float rotY;
    vec3 movement;
    vec3 velocity;

    mat4 translation, rotation;

    bool skipMVP;
    bool skipLighting;

    unsigned int collisionTypeValue;
    bool ground;
    float life;
    bool destroy;

    float cDistance;

    bool hit;

};

#endif
