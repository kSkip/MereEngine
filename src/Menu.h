#ifndef MENU_H
#define MENU_H

#ifndef GL_GLEXT_PROTOTYPES
#define GL_GLEXT_PROTOTYPES
#endif

#include <iostream>

#ifdef _WIN32
#include <windows.h>
#endif

#include <SDL/SDL.h>
#include <SDL/SDL_ttf.h>

#ifdef _WIN32
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

#include "Shader.h"

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

class Menu{
    public:
        Menu();
        ~Menu();
        Menu(const Menu & rhs);

        void init(SDL_Surface* disp);
        bool loop();
    private:
        const SDL_VideoInfo* vidinfo;

        SDL_Surface* display;
        SDL_Surface* text;
        TTF_Font* font;

        Shader* menuShader;
        GLuint MenuList;
        GLuint MenuVertexBuffer;
        GLuint MenuElementBuffer;
        void renderMenu();
};

#endif
