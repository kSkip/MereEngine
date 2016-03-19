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

    for(i = levelObjects.begin(); i != levelObjects.end(); i++){
        delete (*i);
    }
    levelObjects.clear();

    opaqueObjects.clear();
    transparencyObjects.clear();

}

GameState::GameState(const GameState & rhs){}

bool GameState::init(std::string dir, double maxframes)
{

    rootDir = dir;

    vidinfo = SDL_GetVideoInfo();
    maxframerate = maxframes;

	aspectRatio = float(vidinfo->current_w)/float(vidinfo->current_h);

    /*
     * Indicate no state has been loaded yet
     */
	loaded = false;

	return true;

}

void GameState::clean()
{

    std::list<GameObject*>::iterator i;
    std::map<std::string,GLuint>::iterator textureIt;

    delete levelShader;

    for(i = levelObjects.begin(); i != levelObjects.end(); i++){
        delete (*i);
    }
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

void GameState::loadObjectData(struct ObjectFiles* files)
{

    std::map<std::string,std::string>::iterator it;
    ObjectData* newObjectData = new ObjectData;

    if(!files->boundsFile.empty()){

        newObjectData->objectBounds = new boundary;
        newObjectData->objectBounds->LoadBoundaries(files->boundsFile);

    }else{

        newObjectData->objectBounds = NULL;

    }

    if(files->type == std::string("OBJ")){

        unsigned int numElements;
        std::vector<tinyobj::shape_t> shapes;
        std::vector<tinyobj::material_t> materials;

        std::string err = tinyobj::LoadObj(shapes, materials, files->meshFile.c_str(), files->dir.c_str());

        newObjectData->vertexBuffer  =  OBJCreateVertexBuffer(&shapes);
        newObjectData->elementBuffer =  OBJCreateElementBuffer(&shapes, &numElements);
        newObjectData->elementCount  =  3*numElements;
        newObjectData->diffuseTex    =  OBJCreateTextureBuffer(&materials, files->dir.c_str());

    }
    else if(files->type == std::string("MD5"))
    {

        unsigned int numElements;

        struct md5meshdata* md5data = getMD5MeshData(files->meshFile.c_str());

        for( it=files->animFiles.begin(); it != files->animFiles.end(); it++)
        {

            struct md5animdata* md5anim = getMD5AnimData(it->second.c_str());

            Armature* newArm = new Armature;

            newArm->buildArmature(md5anim);

            newObjectData->armatures[it->first] = newArm;

            freeMD5AnimData(md5anim);

        }

        newObjectData->vertexBuffer  = MD5CreateVertexBuffer(md5data,&newObjectData->vertices,&newObjectData->unskinned_vertices,&newObjectData->num_vertices);
        newObjectData->elementBuffer = MD5CreateElementBuffer(md5data,&numElements);
        newObjectData->elementCount  = 3*numElements;
        newObjectData->diffuseTex    = MD5CreateTextureBuffer(md5data,files->dir.c_str());

        freeMD5MeshData(md5data);

    }

    newObjectData->name = files->objectName;
    objectMap[files->objectName] = newObjectData;

}

void GameState::handleObjectDataCommand(std::vector<std::string>& args)
{

    std::ifstream ifs;
    std::string line;
    struct ObjectFiles files;

    if(args.size() > 1){

        files.objectName = args[0];

        ifs.open(args[1].c_str());

        handleObjectSpec(ifs,&files);

        ifs.close();

    }

    loadObjectData(&files);

}

void GameState::handleObjectSpec(std::ifstream& ifs, struct ObjectFiles* files)
{

    std::string type   = "TYPE";
    std::string dir    = "DIR";
    std::string mesh   = "MESH";
    std::string anim   = "ANIM";
    std::string bounds = "BOUNDS";

    std::string line;
    std::vector<std::string> args;

    while(ifs.good()){

        std::getline(ifs,line);
        args = split(line,' ');

        if(args[0] == type && args.size() > 1){

            files->type = args[1];

        }else if(args[0] == mesh && args.size() > 1){

            files->meshFile = args[1];

        }else if(args[0] == anim && args.size() > 2){

            files->animFiles[args[1]] = args[2];

        }else if(args[0] == bounds && args.size() > 1){

            files->boundsFile = args[1];

        }else if(args[0] == dir && args.size() > 1){

            files->dir = args[1];

        }

        args.clear();

    }

}

void GameState::handleObjectCommand(std::vector<std::string>& args)
{

    std::string camera        = "camera";
    std::string static_object = "static_object";
    std::string character     = "character";
    std::string sky_box       = "sky_box";

    if(args.size() > 1){

        ObjectData* object = objectMap[args[0]];

        std::string objectType = args[1];

        if(objectType == camera){

            float windowSize[2];
            windowSize[0] = vidinfo->current_w;
            windowSize[1] = vidinfo->current_h;
            float position[3];
            position[0] = atof(args[2].c_str());
            position[1] = atof(args[3].c_str());
            position[2] = atof(args[4].c_str());
            float rotY = atof(args[5].c_str());

            player = new Camera(object->name, windowSize, position, rotY, this);

            insertOpaqueObject(player);

        }else if(objectType == static_object){

            float position[3];
            position[0] = atof(args[2].c_str());
            position[1] = atof(args[3].c_str());
            position[2] = atof(args[4].c_str());
            float rotY = atof(args[5].c_str());

            StaticObject* newGameObject = new StaticObject(object->name, position, rotY, this);

            insertOpaqueObject(newGameObject);

        }else if(objectType == character){

            float position[3];
            position[0] = atof(args[2].c_str());
            position[1] = atof(args[3].c_str());
            position[2] = atof(args[4].c_str());
            float rotY = atof(args[5].c_str());

            Character* newGameObject = new Character(object->name, position, rotY, this);

            insertOpaqueObject(newGameObject);

        }else if(objectType == sky_box){

            SkyBox* newGameObject = new SkyBox(object->name, player, this);

            insertOpaqueObject(newGameObject);

        }

    }

}

void GameState::handleStateSection(std::ifstream& ifs, std::string& section)
{

    std::string shaders    = "SHADERS";
    std::string objectdata = "OBJECTDATA";
    std::string objects    = "OBJECTS";
    std::string end        = "END";

    std::string line, vs, fs;
    strvec args;


    if(section == shaders){

        std::getline(ifs,line);

        while(line != end){

            if(!line.empty()){

                args = split(line,' ');

                if(args[0] == std::string("VS")){
                    vs = args[1];
                    args.clear();
                }
                if(args[0] == std::string("FS")){
                    fs =  args[1];
                    args.clear();
                }

            }

            std::getline(ifs,line);

        }

        if(!(vs.empty() || fs.empty())){

            vs = rootDir + vs;
            fs = rootDir + fs;

            levelShader = new Shader;
            levelShader->loadShader(vs.c_str(),fs.c_str());

        }

    }else if(section == objectdata){

        std::getline(ifs,line);

        while(line != end){

            if(!line.empty()){

                args = split(line,' ');
                handleObjectDataCommand(args);

                args.clear();

            }

            std::getline(ifs,line);

        }


    }else if(section == objects){

        std::getline(ifs,line);

        while(line != end){

            if(!line.empty()){

                args = split(line,' ');
                handleObjectCommand(args);

                args.clear();

            }

            std::getline(ifs,line);

        }

    }

}

void GameState::loadNew(std::string levelfile)
{

    std::ifstream ifs;
    std::string line;

    SDL_ShowCursor(0);

    ifs.open(levelfile.c_str());

	while(ifs.good()){

        std::getline(ifs,line);
        if(!line.empty()){
            handleStateSection(ifs,line);
        }

	}

    ifs.close();

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

    while(eventHandler()){ //handle inputs like mouse movements and key presses, if returns 0 then escape to menu loop

        inittime = double(SDL_GetTicks());

        move(deltatime); //move the gamestate through time

        render(); //preform ogl rendering
        deltatime = (double(SDL_GetTicks()) - inittime) / 1000.0f; //get the next time step

        //Swap control
        if(deltatime < (1.0f/maxframerate)){
            SDL_Delay(1000.0f*((1.0f/maxframerate) - deltatime));
            deltatime = (1.0f/maxframerate);
        }
        std::cout << "frame rate: " << 1.0f / deltatime << "\n";

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
                                if(!player->playerIsFiring){

                                    GameObject* newobject = new Bullet(player->getHead()+0.1f*glm::normalize(player->getOrigin()-player->getHead()),player->getHead()+100.0f*glm::normalize(player->getOrigin()-player->getHead()),player);

                                    /*char meshFile[50];
                                    char boundsFile[50];
                                    sprintf(meshFile,"Data/bullet.3ds");
                                    sprintf(boundsFile,"Data/bullet.dat");
                                    GameObject* newobject = new ProjectileObject(meshFile,boundsFile,player->getHead()+2.0f*glm::normalize(player->getOrigin()-player->getHead()),5.0f*glm::normalize(player->getOrigin()-player->getHead()),glm::normalize(player->getOrigin()-player->getHead()),0.0f,this);
                                    */

                                    levelObjects.push_back(newobject);
                                    opaqueObjects.push_back(newobject);

                                    player->fire();

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

    for(i = levelObjects.begin(); i != levelObjects.end(); i++){
        (*i)->move(deltatime,this->player,&levelObjects);
    }

    for(i = levelObjects.begin(); i != levelObjects.end(); i++){
        for(j = i; j != levelObjects.end(); j++){
            if(i != j){
                GameObject::testResolveCollision((*i),(*j)); //detect and resolve collisions
            }
        }
    }

    for(i = levelObjects.begin(); i != levelObjects.end();){
        (*i)->commitMovement(this); //commit to the new changes to the objects
        if((*i)->isDestroyed()){ //deallocate and remove objects that no longer exist
            delete *i;
            opaqueObjects.remove(*i);
            transparencyObjects.remove(*i);
            i = levelObjects.erase(i);
        }else{
            i++;
        }
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
    P = glm::perspective(60.0f,aspectRatio,0.1f,1000.0f);
    MV = glm::lookAt(player->getHead(),player->getOrigin(),glm::vec3(0.0f,1.0f,0.0f));

    levelShader->activate(ENABLE_POSITION | ENABLE_NORMAL | ENABLE_TEXCOORD); //enable all attributes

    //pass MVP to shader
    glUniformMatrix4fv(levelShader->modelView,1, GL_FALSE,glm::value_ptr(MV));
    glUniformMatrix4fv(levelShader->projection,1, GL_FALSE,glm::value_ptr(P));


    glActiveTexture(GL_TEXTURE0);
    glUniform1i(levelShader->texture, 0);

    for(i = opaqueObjects.begin(); i != opaqueObjects.end(); i++){
        if((*i) != player){
                (*i)->render(this);
        }
    }

    transparencyObjects.sort(GameObject::pGameObjectComp);

    for(i = transparencyObjects.begin(); i != transparencyObjects.end(); i++){
        if((*i) != player){
                (*i)->render(this);
        }
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
