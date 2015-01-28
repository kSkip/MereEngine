#include "GameState.h"

GameState::GameState(){}

GameState::~GameState(){
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

void GameState::init(double maxframes){
    vidinfo = SDL_GetVideoInfo();
    maxframerate = maxframes;

	aspectRatio = float(vidinfo->current_w)/float(vidinfo->current_h); //useful quantity

	loaded = false;
}

void GameState::clean(){
    std::list<GameObject*>::iterator i;
    std::map<std::string,GLuint>::iterator textureIt;

    //cleanup everything
    delete levelShader;

    for(i = levelObjects.begin(); i != levelObjects.end(); i++){
        delete (*i);
    }
    levelObjects.clear();

    opaqueObjects.clear();
    transparencyObjects.clear();

    for(textureIt = levelTextures.begin(); textureIt != levelTextures.end(); textureIt++){
        glDeleteTextures(1,&textureIt->second);
    }
    levelTextures.clear();

    GameObject::clean();

    loaded = false;
}

void GameState::loadNew(std::string levelfile){
    char token;
	char objectfile[100];
	char boundsfile[100];
    char vertexfile[100];
    char fragmentfile[100];
    char line[100];
    char action[10];
    float x, y, z, roty;
    unsigned int animStart, animStop;
    std::ifstream ifs;
    GameObject* newobject;

    SDL_ShowCursor(0);

	levelShader = new Shader; //allocate memory for all the data relavent to the shaders

    ifs.open(levelfile.c_str());

    ifs.getline(vertexfile,100);
    ifs.getline(fragmentfile,100);

    levelShader->loadShader(vertexfile,fragmentfile);

	while(ifs.good()){

        token = ifs.get();
	    switch(token){
	        case 'p':{
                ifs.get();
                ifs.getline(line,100);
                ifs.getline(boundsfile,100);
                sscanf(line,"%f %f %f %f",&x,&y,&z,&roty);
                const SDL_VideoInfo* vidinfo = SDL_GetVideoInfo();
                player = new Camera(float(vidinfo->current_w),float(vidinfo->current_h),boundsfile,x,y,z,roty,this);
                levelObjects.push_back(player);
                transparencyObjects.push_back(player);}
                break;
	        case 's':
                ifs.get();
                ifs.getline(objectfile,100);
                ifs.getline(line,100);
                sscanf(line,"%f %f %f %f",&x,&y,&z,&roty);
                ifs.getline(boundsfile,100);
                newobject = new StaticObject(objectfile,boundsfile,x,y,z,roty,this);
                levelObjects.push_back(newobject);
                opaqueObjects.push_back(newobject);
                break;
            case 'c':{
                std::map<std::string,AnimBounds> animations;
                ifs.get();
                ifs.getline(objectfile,100);
                ifs.getline(line,100);
                sscanf(line,"%f %f %f %f",&x,&y,&z,&roty);
                ifs.getline(boundsfile,100);

                ifs.getline(line,100);
                sscanf("%s %d %d",action,&animStart,&animStop);
                animations[std::string(action)] = AnimBounds(animStart,animStop,24.0f);

                ifs.getline(line,100);
                sscanf("%s %d %d",action,&animStart,&animStop);
                animations[std::string(action)] = AnimBounds(animStart,animStop,24.0f);

                ifs.getline(line,100);
                sscanf("%s %d %d",action,&animStart,&animStop);
                animations[std::string(action)] = AnimBounds(animStart,animStop,24.0f);

                ifs.getline(line,100);
                sscanf("%s %d %d",action,&animStart,&animStop);
                animations[std::string(action)] = AnimBounds(animStart,animStop,24.0f);

                newobject = new Character(objectfile,boundsfile,x,y,z,roty,this);
                levelObjects.push_back(newobject);
                opaqueObjects.push_back(newobject);}
                break;
            case 'b':
                ifs.get();
                ifs.getline(objectfile,100);
                newobject = new SkyBox(objectfile,player,this);
                levelObjects.push_back(newobject);
                opaqueObjects.push_back(newobject);
                break;
            default:
                break;
	    }

	}

    ifs.close();

    loaded = true;

}

void GameState::loadSave(std::string savefile){
}

void GameState::save(){}

bool GameState::loop(){
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
bool GameState::eventHandler(){
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
                                    GameObject* newobject = new BulletObject(player->getHead()+0.1f*glm::normalize(player->getOrigin()-player->getHead()),player->getHead()+100.0f*glm::normalize(player->getOrigin()-player->getHead()),player);

                                    /*char meshFile[50];
                                    char boundsFile[50];
                                    sprintf(meshFile,"Data/bullet.3ds");
                                    sprintf(boundsFile,"Data/bullet.dat");
                                    GameObject* newobject = new ProjectileObject(meshFile,boundsFile,player->getHead()+2.0f*glm::normalize(player->getOrigin()-player->getHead()),5.0f*glm::normalize(player->getOrigin()-player->getHead()),glm::normalize(player->getOrigin()-player->getHead()),0.0f,this);*/

                                    levelObjects.push_back(newobject);
                                    opaqueObjects.push_back(newobject);

                                    player->Fire();
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
void GameState::move(double deltatime){
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
void GameState::render(){
    GLuint currentTex;
    std::map<std::string,GLuint>::iterator textureIt;
    std::list<GameObject*>::iterator i;
    glm::mat4 MV, P;

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    //get Model View Projection Matrix
    P = glm::perspective(60.0f,aspectRatio,0.1f,1000.0f);
    MV = glm::lookAt(player->getHead(),player->getOrigin(),glm::vec3(0.0f,1.0f,0.0f));

    levelShader->activate(); //enable all attributes

    //pass MVP to shader
    glUniformMatrix4fv(levelShader->modelView,1, GL_FALSE,glm::value_ptr(MV));
    glUniformMatrix4fv(levelShader->projection,1, GL_FALSE,glm::value_ptr(P));


    glActiveTexture(GL_TEXTURE0);
    glUniform1i(levelShader->texture, 0);

    for(textureIt = levelTextures.begin(); textureIt != levelTextures.end(); textureIt++){ //we want to render everything in groups of their textures to minimize texture switching

        currentTex = textureIt->second;
        glBindTexture(GL_TEXTURE_2D,currentTex);

        for(i = opaqueObjects.begin(); i != opaqueObjects.end(); i++){
            if((*i)->SameTexId(currentTex) && (*i) != player){
                    (*i)->render(this);
            }
        }

    }

    transparencyObjects.sort(GameObject::pGameObjectComp);
    for(textureIt = levelTextures.begin(); textureIt != levelTextures.end(); textureIt++){ //we want to render everything in groups of their textures to minimize texture switching

        currentTex = textureIt->second;
        glBindTexture(GL_TEXTURE_2D,currentTex);

        for(i = transparencyObjects.begin(); i != transparencyObjects.end(); i++){
            if((*i)->SameTexId(currentTex) && (*i) != player){
                    (*i)->render(this);
            }
        }

    }

    glClear(GL_DEPTH_BUFFER_BIT);
    player->render(this);

    levelShader->deactivate(); //disable all attributes

    SDL_GL_SwapBuffers(); //swap the buffers
}
