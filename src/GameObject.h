#ifndef GAMEOBJECT_H
#define GAMEOBJECT_H

#ifndef GL_GLEXT_PROTOTYPES
#define GL_GLEXT_PROTOTYPES
#endif

#include <cstdio>
#include <cstdlib>
#include <cassert>
#include <ctime>
#include <algorithm>
#include <vector>
#include <list>
#include <map>
#include <cmath>

#ifdef _WIN32
#include <windows.h>
#ifndef GLEW_H
#define GLEW_H
#include <GL/glew.h>
#endif // GLEW_H
#endif // WIN32

#include <GL/gl.h>

#ifndef _WIN32
#include <GL/glext.h>
#endif // WIN32

#include <SOIL/SOIL.h>

#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/norm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>

#ifdef _WIN32
#include <assimp.hpp>
#include <aiScene.h>
#include <aiPostProcess.h>
#else
#include <assimp/assimp.hpp>
#include <assimp/aiScene.h>
#include <assimp/aiPostProcess.h>
#endif

#include "GameState.h"
#include "boundary.h"

class GameState;

#ifndef COLLISION_TYPES
#define COLLISION_TYPES

#define COLLIDER 1
#define COLLIDEE 2
#define COLLIDER_COLLIDEE 3
#define RAY 4
#define NONE 5

#endif

#ifndef STRUCT_VERTEX
#define STRUCT_VERTEX
struct vertex{
	GLfloat position[3];
	GLfloat normal[3];
	GLfloat texcoord[2];
	GLuint boneIds[4];
	GLfloat boneWeights[4];

    vertex(){
        position[0] = 0;
        position[1] = 0;
        position[2] = 0;
        normal[0] = 0;
        normal[1] = 0;
        normal[2] = 0;
        texcoord[0] = 0;
        texcoord[1] = 0;
        boneIds[0] = 0;
        boneIds[1] = 0;
        boneIds[2] = 0;
        boneIds[3] = 0;
        boneWeights[0] = 0;
        boneWeights[1] = 0;
        boneWeights[2] = 0;
        boneWeights[3] = 0;
    }
};
#endif

#ifndef STRUCT_BONEINFO
#define STRUCT_BONEINFO
struct BoneInfo{
    unsigned int boneId;
    glm::mat4 boneOffset;
};
#endif

#ifndef STRUCT_OBJECTBUFFERS
#define STRUCT_OBJECTBUFFERS
struct ObjectData{
            GLuint VertexBuffer;
            GLuint ElementBuffer;
            GLsizei ElementCount;
            GLuint TextureId;
            Assimp::Importer* Importer;
            glm::mat4 globalInverseTransform;
            std::vector<BoneInfo> bones;
            std::map<std::string,unsigned int> boneMap;
            boundary* objectBounds;
        };
#endif

#ifndef STRUCT_ANIMBOUNDS
#define STRUCT_ANIMBOUNDS
struct AnimBounds{
    unsigned int start;
    unsigned int stop;
    float fps;

    AnimBounds(){
        start = 0;
        stop = 0;
        fps = 24.0f;
    }

    AnimBounds(unsigned int start_in,unsigned int stop_in, float fps_in){
        start = start_in;
        stop = stop_in;
        fps = fps_in;
    }
};
#endif

#ifndef STRUCT_TEXTURE
#define STRUCT_TEXTURE
struct Texture{
            GLuint TextureId;
            std::string TextureName;
};
#endif

const float PI = 3.1415927;

#ifndef GRAVITATIONAL_ACCELERATION
#define GRAVITATIONAL_ACCELERATION 5.0f
#endif

class Camera;

class GameObject{
    public:
        GameObject();
        ~GameObject();
        GameObject(const GameObject & rhs);

        virtual void move(double deltatime, Camera* player, std::list<GameObject*>* levelObjects);
        virtual void render(GameState* state);
        bool SameTexId(GLuint texid);

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
        virtual void damage(float magnitude, glm::vec3 damageLocation, GameState* state){}

        unsigned int collisionType(){return collisionTypeValue;}
        static void testResolveCollision(GameObject* object1, GameObject* object2);
        virtual void registerHit(GameObject* object, float point){}
        virtual void commitMovement(GameState* state){position += movement;}

        static void clean();

        static void addOpaqueObject(GameObject* newObject, GameState* state);
        static void addTransparentObject(GameObject* newObject, GameState* state);

        static bool pGameObjectComp(const GameObject * const & a, const GameObject * const & b)
        {
            return a->cameraDistance() > b->cameraDistance();
        }

    protected:
        GLuint makeGetTexBuf(aiMaterial* material, GameState* state);
        void addTexBuf(GLuint newtex, const char* fullpath, GameState* state);
        GLuint makeArrayBuffer(aiMesh* pMesh,ObjectData* objectData);
        GLuint makeElementBuffer(aiMesh* pMesh);
        void loadBones(const aiMesh* pMesh, vertex* vertexData,ObjectData* objectData);
        void renderMesh(GLuint vertex_buffer, GLuint vertex_element_buffer, GLsizei element_count, GameState* state);

        ObjectData *data;

        glm::vec3 position;
        float rotY;
        glm::vec3 movement;
        glm::vec3 velocity;
        glm::mat4 translation, rotation;
        glm::mat4 boneTransformations[20];

        bool skipBones;
        bool skipMVP;
        bool skipLighting;

        unsigned int collisionTypeValue;
        bool ground;
        float life;
        bool destroy;

        float cDistance;

        bool hit;

        static std::map<std::string,ObjectData*> objectMap;
};

class Camera : public GameObject
{
    public:
        Camera(float windowWidth, float windowHeight, char* boundsfile, float x, float y, float z, float roty, GameState* state);
        ~Camera();

        bool holdingForward;
        bool holdingBackward;
        bool holdingLeftStrafe;
        bool holdingRightStrafe;

        bool playerIsFiring;

        void Fire(){
            playerIsFiring = true;
            flashLife = 1.0f;}

        void handleMouseMove(int mouseX, int mouseY);
        void move(double deltaTime, Camera* player, std::list<GameObject*>* levelObjects);
        void render(GameState* state);
        void commitMovement(GameState* state){
                position += movement;
                head += movement;
                origin += movement;
                if(movement.y == 0.0f) ground = true;}

        // Setters to allow for change of vertical (pitch) and horizontal (yaw) mouse movement sensitivity
        float getPitchSensitivity()            { return pitchSensitivity;  }
        void  setPitchSensitivity(float value) { pitchSensitivity = value; }
        float getYawSensitivity()              { return yawSensitivity;    }
        void  setYawSensitivity(float value)   { yawSensitivity   = value; }

        // Position getters and setters
        glm::vec3 getPosition() const {return position;}
        void setXPos(double x)	{position.x = x;}
        void setYPos(double y)	{position.y = y;}
        void setZPos(double z)	{position.z = z;}

        // Head getters and setters
        glm::vec3 getHead() const { return head;}
        void setXHead(double x)	{head.x = x;}
        void setYHead(double y)	{head.y = y;}
        void setZHead(double z)	{head.z = z;}

        // Origin getters and setters
        glm::vec3 getOrigin() const { return origin;}
        void setXOrigin(double x)	{origin.x = x;}
        void setYOrigin(double y)	{origin.y = y;}
        void setZOrigin(double z)	{origin.z = z;}

        // Front getters and setters
        glm::vec3 getFront() const { return front;}
        void setXFront(double x)	{front.x = x;}
        void setYFront(double y)	{front.y = y;}
        void setZFront(double z)	{front.z = z;}

        // Right getters and setters
        glm::vec3 getRight() const { return right;}
        void setXRight(double x)	{right.x = x;}
        void setYRight(double y)	{right.y = y;}
        void setZRight(double z)	{right.z = z;}

        // Up getters and setters
        glm::vec3 getUp() const { return up;}
        void setXUp(double x)	{up.x = x;}
        void setYUp(double y)	{up.y = y;}
        void setZUp(double z)	{up.z = z;}

    private:
        glm::vec3 head;
        glm::vec3 origin; //for glmLootAt
        glm::vec3 front;
        glm::vec3 up;
        glm::vec3 right;

        float rotHorizontal, rotVertical;

        float cameraAnimTime;

        GLuint billboardVertexBuffer, billboardElementBuffer, billboardElementCount;

        glm::mat4 crosshairsScale;
        GLuint crosshairsTexture;

        glm::mat4 healthTranslation;
        GLuint healthTexture;
        TTF_Font* healthFont;

        glm::mat4 gunfireTranslation;
        GLuint gunfireTexture;

        float flashLife;

        GLuint weaponVertexBuffer, weaponElementBuffer, weaponElementCount, weaponTexture;

        ObjectData *weaponData;

        double movementSpeedFactor;
        double pitchSensitivity;
        double yawSensitivity;

        int windowWidth;
        int windowHeight;
        int windowMidX;
        int windowMidY;

        void initCamera();

        //weapon animation related functions
        void boneTransform(float time);
        void readNodeHeirarchy(float animationTime, const aiNode* pNode, glm::mat4 identity);
        void CalcInterpolatedRotation(aiQuaternion& Out, float AnimationTime, const aiNodeAnim* pNodeAnim);
        void CalcInterpolatedPosition(aiVector3D& Out, float AnimationTime, const aiNodeAnim* pNodeAnim);
        const aiNodeAnim* findNodeAnim(const aiAnimation* pAnimation, const std::string NodeName);
        unsigned int findRotation(float AnimationTime, const aiNodeAnim* pNodeAnim);
        unsigned int findPosition(float AnimationTime, const aiNodeAnim* pNodeAnim);

};

class StaticObject : public GameObject{
    public:
        StaticObject();
        StaticObject(char* objectfile, char* boundsfile, float x, float y, float z, float roty, GameState* state);
        ~StaticObject();
        StaticObject(const StaticObject & rhs);

        void move(double deltatime, Camera* player, std::list<GameObject*>* levelObjects);

        void render(GameState* state);

};

class ProjectileObject : public GameObject{
    public:
        ProjectileObject();
        ProjectileObject(char* objectfile, char* boundsfile, glm::vec3 position, glm::vec3 speed, glm::vec3 align, float roty, GameState* state);
        ~ProjectileObject();
        ProjectileObject(const ProjectileObject & rhs);

        void move(double deltatime, Camera* player, std::list<GameObject*>* levelObjects);
        void commitMovement(GameState* state){position += movement; translation = glm::translate(glm::mat4(1.0f),position);}

        void render(GameState* state);

};

class BulletObject : public GameObject{
    public:
        BulletObject();
        BulletObject(glm::vec3 raystart, glm::vec3 rayfinish, GameObject* parent);
        ~BulletObject();
        BulletObject(const BulletObject & rhs);

        void move(double deltatime, Camera* player, std::list<GameObject*>* levelObjects);
        void registerHit(GameObject* object, float point_on_ray);

        void commitMovement(GameState* state){
            if(tenativeHitObject != NULL){
                tenativeHitObject->damage(1.0f,time*(movement-position)+position,state);
            }
            destroy = true;
        }

        void render(GameState* state);

        GameObject* getParent(){ return parent;}

    private:
        GameObject* parent;
        GameObject* tenativeHitObject;
        float time;
};

class Character : public GameObject{
    public:
        Character();
        Character(char* filename, char* boundsfile, float x, float y, float z, float roty, GameState* state);
        ~Character();
        Character(const Character & rhs);

        void move(double deltatime, Camera* player, std::list<GameObject*>* levelObjects);
        void commitMovement(GameState* state){
            position += movement;
            translation = glm::translate(glm::mat4(1.0f),position);
            if(health < 0.0f)destroy = true;}

        void damage(float magnitude, glm::vec3 damageLocation, GameState* state);

        void render(GameState* state);

    private:
        glm::vec3 front;
        glm::vec3 right;

        float health;

        double movementSpeedFactor;
        glm::vec3 currentTarget;
        double charAnimTime;

        //character animation functions
        void boneTransform(float time);
        void readNodeHeirarchy(float animationTime, const aiNode* pNode, glm::mat4 identity);
        void CalcInterpolatedRotation(aiQuaternion& Out, float AnimationTime, const aiNodeAnim* pNodeAnim);
        void CalcInterpolatedPosition(aiVector3D& Out, float AnimationTime, const aiNodeAnim* pNodeAnim);
        const aiNodeAnim* findNodeAnim(const aiAnimation* pAnimation, const std::string NodeName);
        unsigned int findRotation(float AnimationTime, const aiNodeAnim* pNodeAnim);
        unsigned int findPosition(float AnimationTime, const aiNodeAnim* pNodeAnim);

        void AIDecision(double deltatime, Camera* player, std::list<GameObject*>* levelObjects);
};

class Particle : public GameObject{
    public:
        Particle();
        Particle(const char* filename, glm::vec3 position, glm::vec3 velocity, float life, bool gravity, GameState* state);
        ~Particle();
        Particle(const Particle & rhs);

        void move(double deltatime, Camera* player, std::list<GameObject*>* levelObjects);
        void commitMovement(GameState* state);

        void render(GameState* state);

    private:
        bool gravity;
};

class ParticleSource : public GameObject{
    public:
        ParticleSource();
        ParticleSource(const char* filename, glm::vec3 position, glm::vec3 maxVelocity, float numParticles, float lifetime, bool continuous, int generationRate, bool gravity, GameState* state);
        ~ParticleSource();
        ParticleSource(const ParticleSource & rhs);


        void move(double deltatime, Camera* player, std::list<GameObject*>* levelObjects);
        void commitMovement(GameState* state);

        void render(GameState* state);

    private:
        std::list<Particle*> particles;
};

class SkyBox : public GameObject{
    public:
        SkyBox();
        SkyBox(const char* filename, Camera* player, GameState* state);
        ~SkyBox();
        SkyBox(const SkyBox & rhs);

        void move(double deltatime, Camera* player, std::list<GameObject*>* levelObjects);
        void commitMovement(GameState* state){
                translation = glm::translate(glm::mat4(1.0f),player->getPosition());}

        void render(GameState* state);

    private:

        Camera* player;

};

#endif
