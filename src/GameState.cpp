#include "GameState.h"
#include "GameObjects/GameObject.h"
#include "GameObjects/Camera.h"
#include "GameObjects/StaticObject.h"
#include "GameObjects/Character.h"
#include "GameObjects/SkyBox.h"
#include "GameObjects/Bullet.h"
#include "Models/OBJGLBufferUtilities.hpp"
#include "Utilities/TextManipulation.h"

GameState::GameState(){}

GameState::~GameState()
{

    std::list<GameObject*>::iterator i;

    //cleanup everything before exiting program
    delete levelShader;

    for(i = levelObjects.begin(); i != levelObjects.end(); i++)
        delete (*i);

    levelObjects.clear();

    opaqueObjects.clear();
    transparencyObjects.clear();

}

GameState::GameState(const GameState & rhs){}

void GameState::init(std::string dir, int width, int height)
{

    rootDir = dir;

    screenwidth = width;
    screenheight = height;
	aspectRatio = float(screenwidth) / float(screenheight);

    /*
     * Indicate no state has been loaded yet
     */
	loaded = false;

}

void GameState::clean()
{

    std::list<GameObject*>::iterator i;
    std::map<std::string,GLuint>::iterator textureIt;

    delete levelShader;

    for(i = levelObjects.begin(); i != levelObjects.end(); i++)
        delete (*i);

    levelObjects.clear();

    opaqueObjects.clear();
    transparencyObjects.clear();

    std::map<std::string,ObjectData*>::iterator objIt;

    for(objIt = objectMap.begin(); objIt != objectMap.end(); objIt++){

        glDeleteBuffers(1,&objIt->second->vertexBuffer);
        glDeleteBuffers(1,&objIt->second->elementBuffer);
        glDeleteTextures(1,&objIt->second->diffuseTex);
        delete objIt->second->objectBounds;

    }

    objectMap.clear();

    loaded = false;

}

void GameState::loadObjectData(DataBlock & objectDataBlock)
{

    block_it it = objectDataBlock.beginBlocks();

    while(it != objectDataBlock.endBlocks())
    {

        std::string name      = it->first;
        DataBlock& objectData = it->second;

        objectData("rootdir") = rootDir;

        objectMap[name] = new ObjectData(objectData);

        ++it;

    }

}

void GameState::loadObjects(DataBlock & objectBlock)
{

    block_it it = objectBlock.beginBlocks();

    while(it != objectBlock.endBlocks())
    {

        DataBlock& objectData = it->second;

        ObjectData* object = objectMap[it->first];

        std::string objectType = objectData("obj_type");

        objectData("rootdir") = rootDir;

        if(objectType == "camera"){

            float windowSize[2];
            windowSize[0] = screenwidth;
            windowSize[1] = screenheight;

            player = new Camera(object, windowSize, objectData, this);

            insertOpaqueObject(player);

        }else if(objectType == "static_object"){

            StaticObject* newGameObject = new StaticObject(object, objectData, this);

            insertOpaqueObject(newGameObject);

        }else if(objectType == "character"){

            Character* newGameObject = new Character(object, objectData, this);

            insertOpaqueObject(newGameObject);

        }else if(objectType == "sky_box"){

            SkyBox* newGameObject = new SkyBox(object, player, this);

            insertOpaqueObject(newGameObject);

        }

        ++it;

    }

}

void GameState::loadNew(std::string levelfile)
{

    DataBlock leveldef;

    leveldef.load(levelfile.c_str());

    try{

        /*
         * load shaders
         */
        std::string vs = rootDir + leveldef["shaders"]("vs");

        std::string fs = rootDir + leveldef["shaders"]("fs");

        levelShader = new Shader;

        levelShader->loadShader(vs.c_str(),fs.c_str());

    }
    catch(std::exception& e)
    {

        std::cerr << e.what() << "\n";
        delete levelShader;
        throw std::runtime_error("Failed to init shaders");

    }

    try{

        /*
         * load shared data and GPU buffers
         */
        loadObjectData(leveldef["objectdata"]);

    }
    catch(std::exception& e)
    {

        std::cerr << e.what() << "\n";
        throw std::runtime_error("Failed to load shared and GPU buffer");

    }

    try{

        /*
         * load object instances
         */
        loadObjects(leveldef["objects"]);

    }
    catch(std::exception& e)
    {

        std::cerr << e.what() << "\n";
        throw std::runtime_error("Failed to load object instances");

    }


    loaded = true;

}

void GameState::loadSave(std::string savefile){}

void GameState::save(){}

void GameState::movingForward(bool isMoving)
{
    player->holdingForward = isMoving;
}

void GameState::movingBackward(bool isMoving)
{
    player->holdingBackward = isMoving;
}

void GameState::movingLeft(bool isMoving)
{
    player->holdingLeftStrafe = isMoving;
}

void GameState::movingRight(bool isMoving)
{
    player->holdingRightStrafe = isMoving;
}

void GameState::handleMouseMove(int x, int y)
{
    if (x || y) {
        player->handleMouseMove(x, y);
    }
}

void GameState::handleSpacebar()
{
    if (player->onGround()) {
        player->setYSpeed(1.5f);
        player->isNotGrounded();
    }
}

//moves the game objects through time
void GameState::move(double deltatime)
{

    std::list<GameObject*>::iterator i, j;

    for(i = levelObjects.begin(); i != levelObjects.end(); i++)
        (*i)->move(deltatime,this->player,&levelObjects);


    for(i = levelObjects.begin(); i != levelObjects.end(); i++)
    {
        for(j = i; j != levelObjects.end(); j++)
        {
            if(i != j) GameObject::testResolveCollision((*i),(*j)); //detect and resolve collisions
        }
    }

    for(i = levelObjects.begin(); i != levelObjects.end();)

    {
        (*i)->commitMovement(this); //commit to the new changes to the objects

        if((*i)->isDestroyed()){ //deallocate and remove objects that no longer exist

            delete *i;
            opaqueObjects.remove(*i);
            transparencyObjects.remove(*i);
            i = levelObjects.erase(i);

        }else i++;

    }

}

//renders everything
void GameState::render()
{

    std::map<std::string,GLuint>::iterator textureIt;
    std::list<GameObject*>::iterator i;
    glm::mat4 MV, P;

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    //get Model View Projection Matrix
    P = glm::perspective(1.04f,aspectRatio,0.1f,1000.0f);
    MV = glm::lookAt(player->getHead(),player->getOrigin(),glm::vec3(0.0f,1.0f,0.0f));

    levelShader->activate(ENABLE_POSITION | ENABLE_NORMAL | ENABLE_TEXCOORD); //enable all attributes

    //pass MVP to shader
    glUniformMatrix4fv(levelShader->modelView,1, GL_FALSE,glm::value_ptr(MV));
    glUniformMatrix4fv(levelShader->projection,1, GL_FALSE,glm::value_ptr(P));


    //Use of the only the first texture only for now
    glActiveTexture(GL_TEXTURE0);
    glUniform1i(levelShader->texture, 0);

    /*
     * Render the opaque objects first t ofill the depth buffer
     * Then the transparent obejcts will go next
     */
    for(i = opaqueObjects.begin(); i != opaqueObjects.end(); i++)
    {

        if((*i) != player) (*i)->render(this);

    }

    /*
     * Sort transparent objects so blending works correctly
     */
    transparencyObjects.sort(GameObject::pGameObjectComp);

    for(i = transparencyObjects.begin(); i != transparencyObjects.end(); i++)
    {

        if((*i) != player) (*i)->render(this);

    }

    player->render(this);

    levelShader->deactivate(ENABLE_POSITION | ENABLE_NORMAL | ENABLE_TEXCOORD);

    // Render health meter
    glColor3f(0.0f, 0.7f, 0.0f);
    float top = ((float)screenheight - 12.0f) / (float)screenheight;
    float left = ((float)screenwidth - 12.0f) / (float)screenwidth;
    glRasterPos3f(-left, -top, 1.0f);
    glCallLists(3, GL_UNSIGNED_BYTE, "100");

    // Render crosshairs
    glLineWidth(2.0f);
    glBegin(GL_LINES);
    glColor3f(0.5f, 0.5f, 0.0f);
    glVertex2f(0.0, 10.0f / screenheight);
    glVertex2f(0.0, 30.0f / screenheight);
    glVertex2f(0.0, -10.0f / screenheight);
    glVertex2f(0.0, -30.0f / screenheight);

    glVertex2f(10.0f / screenwidth, 0.0);
    glVertex2f(30.0f / screenwidth, 0.0);
    glVertex2f(-10.0f / screenwidth, 0.0);
    glVertex2f(-30.0f / screenwidth, 0.0);
    glEnd();
}

void GameState::insertOpaqueObject(GameObject* newGameObject)
{

    levelObjects.push_back(newGameObject);

    GLuint newDiffuseTex = newGameObject->data->diffuseTex;

    std::list<GameObject*>::iterator it;

    it = opaqueObjects.begin();

    while(it != opaqueObjects.end()){

        if( newDiffuseTex < (*it)->data->diffuseTex ){

            opaqueObjects.insert(it,newGameObject);
            return;

        }

        it++;
    }

    opaqueObjects.push_back(newGameObject);

}

void GameState::insertTransparencyObject(GameObject* newGameObject)
{

    levelObjects.push_back(newGameObject);

    transparencyObjects.push_back(newGameObject);

}
