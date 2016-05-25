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

void GameState::init(std::string dir, double maxframes)
{

    rootDir = dir;

    vidinfo = SDL_GetVideoInfo();
    maxframerate = maxframes;

	aspectRatio = float(vidinfo->current_w) / float(vidinfo->current_h);

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
            windowSize[0] = vidinfo->current_w;
            windowSize[1] = vidinfo->current_h;

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

    SDL_ShowCursor(0);

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

bool GameState::loop()
{

    double deltatime = 0.0f;
    double inittime;

    deltatime = 0.0f;

    SDL_ShowCursor(0);

    while(eventHandler()){ //Returns 0 on Escape key

        inittime = double(SDL_GetTicks());

        move(deltatime); //move the GameState through time

        render(); //preform ogl rendering

        deltatime = (double(SDL_GetTicks()) - inittime) / 1000.0f; //get the next time step

        //Manual swap control
        if(deltatime < (1.0f/maxframerate)){
            SDL_Delay(1000.0f*((1.0f/maxframerate) - deltatime));
            deltatime = (1.0f/maxframerate);
        }

        //std::cout << "frame rate: " << 1.0f / deltatime << "\n";

        //end of frame
    }

    return true;

}

//handles user inputs like mouse movements and key presses
bool GameState::eventHandler()
{

    SDL_Event event;

    while(SDL_PollEvent(&event)){
			switch(event.type){
				case SDL_KEYDOWN:
					switch(event.key.keysym.sym){
						case SDLK_ESCAPE:
							return false;
						case SDLK_w:
							player->holdingForward = true;
							break;
						case SDLK_s:
							player->holdingBackward = true;
							break;
						case SDLK_a:
							player->holdingLeftStrafe = true;
							break;
						case SDLK_d:
							player->holdingRightStrafe = true;
							break;
						case SDLK_SPACE:
							if(player->onGround()){
								player->setYSpeed(1.5f);
								player->isNotGrounded();
							}
							break;
                        case SDLK_PRINT:
                            const SDL_VideoInfo* vidinfo;
                            vidinfo = SDL_GetVideoInfo();
                            SOIL_save_screenshot("screenshot.bmp",SOIL_SAVE_TYPE_BMP,0, 0, vidinfo->current_w, vidinfo->current_h);
                            break;
                        default:
                            break;
					}
					break;
				case SDL_KEYUP:
					switch(event.key.keysym.sym){
						case SDLK_w:
							player->holdingForward = false;
							break;
						case SDLK_s:
							player->holdingBackward = false;
							break;
						case SDLK_a:
							player->holdingLeftStrafe = false;
							break;
						case SDLK_d:
							player->holdingRightStrafe = false;
							break;
                        default:
							break;
					}
					break;
                case SDL_MOUSEBUTTONDOWN:
                    switch(event.button.button){
						case SDL_BUTTON_LEFT:{
                                /*
                                 * This case handles the player's use
                                 * of a weapon
                                 */
                                if(!player->playerIsFiring){

                                    //GameObject* newobject = new Bullet(player->getHead()+0.1f*glm::normalize(player->getOrigin()-player->getHead()),player->getHead()+100.0f*glm::normalize(player->getOrigin()-player->getHead()),player);

                                    /*char meshFile[50];
                                    char boundsFile[50];
                                    sprintf(meshFile,"Data/bullet.3ds");
                                    sprintf(boundsFile,"Data/bullet.dat");
                                    GameObject* newobject = new ProjectileObject(meshFile,boundsFile,player->getHead()+2.0f*glm::normalize(player->getOrigin()-player->getHead()),5.0f*glm::normalize(player->getOrigin()-player->getHead()),glm::normalize(player->getOrigin()-player->getHead()),0.0f,this);
                                    */

                                    //levelObjects.push_back(newobject);
                                    //opaqueObjects.push_back(newobject);

                                    //player->fire();

                                }
                            }
							break;
                        default:
							break;
					}
					break;
				case SDL_MOUSEMOTION:
					player->handleMouseMove(event.motion.x,event.motion.y);
					break;
				case SDL_QUIT:
					return false;
				default:
                    return true;
					break;
			}
		}
		return true;

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
    P = glm::perspective(1.04f/*60.0f*/,aspectRatio,0.1f,1000.0f);
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


    glClear(GL_DEPTH_BUFFER_BIT);
    player->render(this);

    levelShader->deactivate(ENABLE_POSITION | ENABLE_NORMAL | ENABLE_TEXCOORD); //disable all attributes

    SDL_GL_SwapBuffers(); //swap the buffers

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
