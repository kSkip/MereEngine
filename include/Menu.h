#ifndef MENU_H
#define MENU_H

#include <string>
#include <vector>

#include "Platform.h"

enum menu_action {
    MENU_ACTION_NULL,
    MENU_ACTION_NEWGAME,
    MENU_ACTION_LOADGAME,
    MENU_ACTION_SAVEGAME,
    MENU_ACTION_EXIT
};

class Menu
{

    public:
        menu_action getSelection() {
            return selectedAction;
        }

        void render(int, int);

        void handleButtonDown()
        {
            selectedAction = hoveringAction;
        }

        void handleMouseMove(int x, int y) {
            mouseX = x;
            mouseY = y;
        }

        bool isHovering(int, int);

        void clear() {
            selectedAction = MENU_ACTION_NULL;
        }

    private:
        int mouseX;
        int mouseY;
        menu_action selectedAction;
        menu_action hoveringAction;
};

#endif
