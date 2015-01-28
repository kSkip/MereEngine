#include "Menu.h"

Menu::Menu(){}

Menu::~Menu(){}

Menu::Menu(const Menu & rhs){}

void Menu::init(SDL_Surface* disp){
    GLuint vertexbuffer, elementbuffer;
    char vertexFile[50];
    char fragmentFile[50];

    vidinfo = SDL_GetVideoInfo();

    display = disp;

    menuShader = new Shader;
    sprintf(vertexFile,"Data/menu.v.glsl");
    sprintf(fragmentFile,"Data/menu.f.glsl");
    menuShader->loadShader(vertexFile,fragmentFile);
    MenuList = SOIL_load_OGL_texture("Data/MenuList.png",SOIL_LOAD_RGBA,SOIL_CREATE_NEW_ID,SOIL_FLAG_TEXTURE_REPEATS | SOIL_FLAG_INVERT_Y | SOIL_FLAG_COMPRESS_TO_DXT);

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
        vertex_data[0].texcoord[1] = 0.0f;

        vertex_data[1].texcoord[0] = 1.0f;
        vertex_data[1].texcoord[1] = 0.0f;

        vertex_data[2].texcoord[0] = 1.0f;
        vertex_data[2].texcoord[1] = 1.0f;

        vertex_data[3].texcoord[0] = 0.0f;
        vertex_data[3].texcoord[1] = 1.0f;


		glGenBuffers(1, &vertexbuffer);
		glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
		glBufferData(GL_ARRAY_BUFFER, 4*sizeof(vertex), vertex_data, GL_STATIC_DRAW);

	free(vertex_data);

	MenuVertexBuffer = vertexbuffer;

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

	MenuElementBuffer = elementbuffer;

	TTF_Init();

	font = TTF_OpenFont("Data/carbon.ttf", 24);

	SDL_Color text_color = {255, 255, 255};
   text = TTF_RenderText_Solid(font,
   "Yeah buddy.",
   text_color);
}

bool Menu::loop(){
    SDL_Event event;

    while(true){

        while(SDL_PollEvent(&event)){
                switch(event.type){

                    case SDL_KEYDOWN:
                        switch(event.key.keysym.sym){
                            case SDLK_1:
                                return true;
                            case SDLK_6:
                                return false;
                            case SDLK_ESCAPE:
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

                    case SDL_QUIT:
                        return false;
                        break;
                    default:
                        break;
                }
            }

        renderMenu();

    }

}

void Menu::renderMenu(){
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glScalef(float(vidinfo->current_h)/1024.0f,1.0f,1.0f);

    menuShader->activate();

    glActiveTexture(GL_TEXTURE0);
    glUniform1i(menuShader->texture, 0);
    glBindTexture(GL_TEXTURE_2D,MenuList);

    glBindBuffer(GL_ARRAY_BUFFER, MenuVertexBuffer);
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
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, MenuElementBuffer);
    glDrawElements(
        GL_TRIANGLES,
        6,
        GL_UNSIGNED_INT,
        (void*)0
    );

    menuShader->deactivate();

    SDL_BlitSurface(text, NULL, display, NULL);
    SDL_Flip(display);

    SDL_GL_SwapBuffers();
}
