#ifndef MENU_H
#define MENU_H

#ifndef GL_GLEXT_PROTOTYPES
#define GL_GLEXT_PROTOTYPES
#endif

#include <iostream>
#include <fstream>
#include <vector>
#include <stdexcept>

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

#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Shader.h"
#include "Utilities/FileToString.h"
#include "Utilities/SplitString.h"

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

#define MENU_ACTION_NULL 0
#define MENU_ACTION_NEWGAME 1
#define MENU_ACTION_LOADGAME 2
#define MENU_ACTION_SAVEGAME 3
#define MENU_ACTION_QUIT 4

class MenuItem
{

    public:

        glm::mat4 translation;
        glm::mat4 scale;
        GLuint vertexBuffer;
        GLuint elementBuffer;
        GLuint tex;
        unsigned int action;
        bool mouseOver;

        void handleMouseMove(int mouseX, int mouseY, const SDL_VideoInfo* vidinfo);

};

class MenuState
{

    public:

        MenuState();
        ~MenuState();
        MenuState(const MenuState & rhs);

        bool init(std::string rootDir, double maxframerate);
        bool loop();

        unsigned int currentAction;

    private:

        std::string rootDir;

        const SDL_VideoInfo* vidinfo;
        double maxframerate;

        glm::mat4 MV, P;

        Shader* menuShader;
        std::vector<MenuItem*> MenuList;

        MenuItem* makeMenuItem(const char* name, const char* action, float x, float y, float yscale);
        void handleMouseMove(int mouseX, int mouseY, const SDL_VideoInfo* vidinfo);
        void renderMenu();

};

#endif
