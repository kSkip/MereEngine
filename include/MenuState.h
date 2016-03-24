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
#endif // NOT WIN32

#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Shader.h"
#include "Utilities/TextManipulation.h"
#include "Utilities/DataBlock.h"


#define MENU_ACTION_NULL 0
#define MENU_ACTION_NEWGAME 1
#define MENU_ACTION_LOADGAME 2
#define MENU_ACTION_SAVEGAME 3
#define MENU_ACTION_QUIT 4

class MenuState
{

    public:

        MenuState();
        ~MenuState();
        MenuState(const MenuState & rhs);

        void init(std::string rootDir, double maxframerate);
        bool loop();

        unsigned int currentAction;

    private:

        class MenuItem
        {

            public:

                MenuItem();
                MenuItem(DataBlock& def);

                glm::mat4 translation;
                glm::mat4 scale;
                GLuint vertexBuffer;
                GLuint elementBuffer;
                GLuint tex;
                unsigned int action;
                bool mouseOver;

                void handleMouseMove(int mouseX, int mouseY, const SDL_VideoInfo* vidinfo);

        };

        std::string rootDir;

        std::string defaultFont;

        const SDL_VideoInfo* vidinfo;
        double maxframerate;

        glm::mat4 MV, P;

        Shader* menuShader;
        std::vector<MenuItem*> MenuList;

        void handleMouseMove(int mouseX, int mouseY, const SDL_VideoInfo* vidinfo);
        void renderMenu();


};

#endif
