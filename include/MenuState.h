#ifndef MENU_H
#define MENU_H

#include <iostream>
#include <fstream>
#include <vector>
#include <stdexcept>

#include "Platform.h"

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

        void init(std::string rootDir, int width, int height);
        void renderMenu();

        void handleButtonDown();
        void handleMouseMove(int, int);

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

                void handleMouseMove(int mouseX, int mouseY, int width, int height);

        };

        std::string rootDir;

        std::string defaultFont;

        int screenwidth;
        int screenheight;

        glm::mat4 MV, P;

        Shader* menuShader;
        std::vector<MenuItem*> MenuList;

};

#endif
