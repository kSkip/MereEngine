#include "MenuState.h"

MenuState::MenuState(){}

MenuState::~MenuState(){}

MenuState::MenuState(const MenuState & rhs){}

void MenuState::init(double maxframes){
    char vertexFile[50];
    char fragmentFile[50];
    char menulistFile[50];
    char line[50], text[50], action[50];
    std::ifstream ifs;
	char t;
	float values[6];

    vidinfo = SDL_GetVideoInfo();
    maxframerate = maxframes;

    menuShader = new Shader;
    sprintf(vertexFile,"Data/menustate.v.glsl");
    sprintf(fragmentFile,"Data/menustate.f.glsl");
    menuShader->loadShader(vertexFile,fragmentFile);

    TTF_Init();

    sprintf(menulistFile,"Data/MenuList");
    ifs.open(menulistFile);

	while(ifs >> t){
		switch(t){
			case 't': //the title
                ifs.get();
                ifs.getline(text,100);
                ifs.getline(line,100);
                sscanf(line,"%f %f %f",&values[0],&values[1],&values[2]); //x, y, and y scale factor
				MenuList.push_back(makeMenuItem(text,NULL,values[0],values[1],values[2]));
				break;
            case 'e': //a clickable entry
                ifs.get();
                ifs.getline(text,100);
                ifs.getline(action,100);
                ifs.getline(line,100);
                sscanf(line,"%f %f %f",&values[0],&values[1],&values[2]); //x, y, and y scale factor
                MenuList.push_back(makeMenuItem(text,action,values[0],values[1],values[2]));
				break;
		}
	}

	ifs.close();

    //MenuList.push_back(makeMenuItem("New Game","loadnew",0,0));
    //MenuList.push_back(makeMenuItem("Quit","quit",0,100));

}

MenuItem* MenuState::makeMenuItem(const char* name, const char* action, float x, float y, float yscale){
    GLuint vertexbuffer, elementbuffer;
    MenuItem* newItem = new MenuItem;

        newItem->translation = glm::translate(glm::mat4(1.0f),glm::vec3(x,y,0.0f));

        vertex *vertex_data = (vertex*) malloc(4 * sizeof(vertex));

        vertex_data[0].position[0] = -1.0f;
        vertex_data[0].position[1] = -1.0f;
        vertex_data[0].position[2] = 0.0f;

        vertex_data[1].position[0] = 1.0f;
        vertex_data[1].position[1] = -1.0f;
        vertex_data[1].position[2] = 0.0f;

        vertex_data[2].position[0] = 1.0f;
        vertex_data[2].position[1] = 1.0f;
        vertex_data[2].position[2] = 0.0f;

        vertex_data[3].position[0] = -1.0f;
        vertex_data[3].position[1] = 1.0f;
        vertex_data[3].position[2] = 0.0f;


        vertex_data[0].texcoord[0] = 0.0f;
        vertex_data[0].texcoord[1] = 1.0f;

        vertex_data[1].texcoord[0] = 1.0f;
        vertex_data[1].texcoord[1] = 1.0f;

        vertex_data[2].texcoord[0] = 1.0f;
        vertex_data[2].texcoord[1] = 0.0f;

        vertex_data[3].texcoord[0] = 0.0f;
        vertex_data[3].texcoord[1] = 0.0f;


		glGenBuffers(1, &vertexbuffer);
		glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
		glBufferData(GL_ARRAY_BUFFER, 4*sizeof(vertex), vertex_data, GL_STATIC_DRAW);

        free(vertex_data);

        newItem->vertexBuffer = vertexbuffer;

        GLuint *element_data = (GLuint*) malloc(6 * sizeof(GLuint));

        element_data[0] = 0;
        element_data[1] = 1;
        element_data[2] = 2;
        element_data[3] = 0;
        element_data[4] = 2;
        element_data[5] = 3;

        glGenBuffers(1, &elementbuffer);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementbuffer);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, 6*sizeof(GLuint) , element_data, GL_STATIC_DRAW);

        free(element_data);

        newItem->elementBuffer = elementbuffer;

        TTF_Font* font = TTF_OpenFont("Data/carbon.ttf", 20);
        GLuint texture;
        GLuint texture_format;

        SDL_Color text_color = {127, 0, 0};
        SDL_Surface* surface = TTF_RenderText_Blended(font,name,text_color);

        unsigned int colors = surface->format->BytesPerPixel;
        if (colors == 4) {   // alpha
            if (surface->format->Rmask == 0x000000ff)
                texture_format = GL_RGBA;
            else
                texture_format = GL_BGRA;
        } else {             // no alpha
            if (surface->format->Rmask == 0x000000ff)
                texture_format = GL_RGB;
            else
                texture_format = GL_BGR;
        }

        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);
        glTexImage2D(GL_TEXTURE_2D, 0, colors, surface->w, surface->h, 0, texture_format, GL_UNSIGNED_BYTE, surface->pixels);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        newItem->scale = glm::scale(glm::mat4(1.0f),glm::vec3( (surface->w/surface->h) * yscale, yscale, 0.0f));

        newItem->tex = texture;

        newItem->mouseOver = false;

        if(action == NULL){
            newItem->action = MENU_ACTION_NULL;
        }else if(!strcmp(action,"loadnew")){
            newItem->action = MENU_ACTION_NEWGAME;
        }else if(!strcmp(action,"quit")){
            newItem->action = MENU_ACTION_QUIT;
        }

        SDL_FreeSurface(surface);
        TTF_CloseFont(font);

    return newItem;
}

bool MenuState::loop(){
    unsigned int i;
    double deltatime = 0.0f;
    double inittime;
    SDL_Event event;

    SDL_ShowCursor(1);
    SDL_WM_GrabInput(SDL_GRAB_ON);
    SDL_WarpMouse(vidinfo->current_w / 2.0f,vidinfo->current_h / 2.0f);

    while(true){

        inittime = double(SDL_GetTicks());

        while(SDL_PollEvent(&event)){
                switch(event.type){

                    case SDL_KEYDOWN:
                        switch(event.key.keysym.sym){
                            case SDLK_ESCAPE:
                                currentAction = MENU_ACTION_NULL;
                                return true;
                            default:
                                break;
                        }
                        break;

                    case SDL_KEYUP:
                        switch(event.key.keysym.sym){
                            case SDLK_w:
                                break;
                            default:
                                break;
                        }
                        break;

                    case SDL_MOUSEBUTTONDOWN:
                        switch(event.button.button){
                            case SDL_BUTTON_LEFT:
                            for(i=0;i<MenuList.size();i++){
                                if(MenuList[i]->mouseOver){
                                    switch(MenuList[i]->action){
                                        case MENU_ACTION_NULL:
                                            break;
                                        case MENU_ACTION_NEWGAME:
                                            currentAction = MENU_ACTION_NEWGAME;
                                            return true;
                                        case MENU_ACTION_QUIT:
                                            currentAction = MENU_ACTION_QUIT;
                                            return false;
                                        default:
                                            break;
                                    }
                                }
                            }
							break;
                        default:
							break;
                        }
					break;

                    case SDL_MOUSEMOTION:
                        for(i=0;i<MenuList.size();i++){
                            MenuList[i]->handleMouseMove(event.motion.x,event.motion.y,vidinfo);
                        }
                        break;
                    case SDL_QUIT:
                        return false;
                        break;
                    default:
                        break;
                }
            }

        renderMenu();
        deltatime = (double(SDL_GetTicks()) - inittime) / 1000.0f; //get the next time step
        if(deltatime < (1.0f/maxframerate)) {
            SDL_Delay( 1000.0f*((1.0f/maxframerate) - deltatime));
            deltatime = (1.0f/maxframerate);
        }

    }

}

void MenuState::renderMenu(){
    unsigned int i;
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

    menuShader->activate();

    glActiveTexture(GL_TEXTURE0);
    glUniform1i(menuShader->texture, 0);


    for(i=0;i<MenuList.size();i++){

    glBindTexture(GL_TEXTURE_2D,MenuList[i]->tex);
    glUniformMatrix4fv(menuShader->translation,1, GL_FALSE,glm::value_ptr(MenuList[i]->translation));
    glUniformMatrix4fv(menuShader->scale,1, GL_FALSE,glm::value_ptr(MenuList[i]->scale));
    glUniform1ui(menuShader->mouseOver,MenuList[i]->mouseOver);

    glBindBuffer(GL_ARRAY_BUFFER, MenuList[i]->vertexBuffer);
    glVertexAttribPointer(
        menuShader->position,
        3, GL_FLOAT, GL_FALSE,sizeof(struct vertex),
        (void*)offsetof(struct vertex, position)
    );
    glVertexAttribPointer(
        menuShader->normal,
        3, GL_FLOAT, GL_FALSE,sizeof(struct vertex),
        (void*)offsetof(struct vertex, normal)
    );
    glVertexAttribPointer(
        menuShader->texcoord,
        2, GL_FLOAT, GL_FALSE, sizeof(struct vertex),
        (void*)offsetof(struct vertex, texcoord)
    );
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, MenuList[i]->elementBuffer);
    glDrawElements(
        GL_TRIANGLES,
        6,
        GL_UNSIGNED_INT,
        (void*)0
    );

    }

    menuShader->deactivate();

    SDL_GL_SwapBuffers();
}

void MenuItem::handleMouseMove(int mouseX, int mouseY, const SDL_VideoInfo* vidinfo){

    if(this->action == 0) return;

    float xscaled = -2.0f*(float(mouseX) / float(vidinfo->current_w))+1.0f;
    float yscaled = -2.0f*(float(mouseY) / float(vidinfo->current_h))+1.0f;

    float yHigh = (this->translation * this->scale * glm::vec4(0.0f,1.0f,0.0f,1.0f)).y;
    float yLow = (this->translation * this->scale * glm::vec4(0.0f,-1.0f,0.0f,1.0f)).y;

    float xHigh = (this->translation * this->scale * glm::vec4(1.0f,0.0f,0.0f,1.0f)).x;
    float xLow = (this->translation * this->scale * glm::vec4(-1.0f,0.0f,0.0f,1.0f)).x;

    if(xscaled > xLow && xscaled < xHigh && yscaled > yLow && yscaled < yHigh){
        this->mouseOver = true;
    }else{
        this->mouseOver = false;
    }

}

