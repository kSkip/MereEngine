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

#include <SOIL/SOIL.h>

#define GLM_FORCE_RADIANS

#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/norm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>

#include "Models/Armature.hpp"
#include "boundary.h"

#include "GameObjects/ObjectData.h"

enum Primative {PTYPE_TRIANGLES, PTYPE_POINTS};

#include "GameState.h"

const float PI = 3.1415927f;

#define GRAVITATIONAL_ACCELERATION 5.0f

#define COLLIDER 1
#define COLLIDEE 2
#define COLLIDER_COLLIDEE 3
#define RAY 4
#define NONE 5

class Camera;

class GameObject{

    friend class GameState;

    public:

        GameObject();
        virtual ~GameObject();
        GameObject(const GameObject & rhs);

        virtual void move(double deltatime, Camera* player, std::list<GameObject*>* levelObjects);
        virtual void render(GameState* state);

        glm::vec3 getPosition() const { return position;}
        float getRotation() const {return rotY;}

        glm::vec3 getVelocity() const { return velocity;}
        void setXSpeed(double x)	{velocity.x = x;}
        void setYSpeed(double y)	{velocity.y = y;}
        void setZSpeed(double z)	{velocity.z = z;}

        glm::vec3 getMovement() const { return movement;}
        double cameraDistance() const { return cDistance;}

        bool onGround() const {return ground;}
        void isGrounded(){ground = true;}
        void isNotGrounded(){ground = false;}

        bool isDestroyed(){return destroy;}
        void makeDestroy(){destroy = true;}

        virtual bool fades(){return false;}

        virtual float getLife(){return life;}

        virtual void damage(float magnitude, glm::vec3 damageLocation, GameState* state);

        unsigned int collisionType(){return collisionTypeValue;}
        static void testResolveCollision(GameObject* object1, GameObject* object2);
        virtual void registerHit(GameObject* object, float point){}
        virtual void commitMovement(GameState* state){position += movement;}

        static void addOpaqueObject(GameObject* newObject, GameState* state);
        static void addTransparentObject(GameObject* newObject, GameState* state);

        static bool pGameObjectComp(const GameObject * const & a, const GameObject * const & b)
        {
            return a->cameraDistance() > b->cameraDistance();
        }

    protected:

        void renderMeshElements(GLuint vertex_buffer, GLuint vertex_element_buffer, GLsizei element_count, Primative type, GameState* state);
        void renderMeshArrays(GLuint vertex_buffer, GLsizei element_count, Primative type, GameState* state);

        ObjectData* getObjectData(std::string& objectName, GameState* state)
        {
            return state->objectMap[objectName];
        }

        ObjectData *data;

        glm::vec3 position;
        float rotY;
        glm::vec3 movement;
        glm::vec3 velocity;

        glm::mat4 translation, rotation;

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
