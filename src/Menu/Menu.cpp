#include "Menu.h"

bool Menu::isHovering(int x, int y)
{
    if (mouseX > x && mouseX < x + 100 && mouseY > y - 18 && mouseY < y + 18) {
        return true;
    }
    else {
        return false;
    }
}

void Menu::render(int width, int height)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

    int x = width / 2 - 50;
    int y = height / 2 - 50;

    glColor3f(0.5f, 0.5f, 0.5f);
    glRasterPos3f(float(x - width / 2) / width, float(height / 2 - y)/ height, 1.0f);
    glCallLists(15, GL_UNSIGNED_BYTE, "MereEngine Demo");
    y += 36;

    if (isHovering(x, y)) {
        glColor3f(0.0f, 0.7f, 0.0f);
        hoveringAction = MENU_ACTION_NEWGAME;
    }
    else {
        glColor3f(0.9f, 0.9f, 0.9f);
    }
    glRasterPos3f(float(x - width / 2) / width, float(height / 2 - y) / height, 1.0f);
    glCallLists(9, GL_UNSIGNED_BYTE, "New Game");
    y += 36;

    if (isHovering(x, y)) {
        glColor3f(0.0f, 0.7f, 0.0f);
        hoveringAction = MENU_ACTION_EXIT;
    }
    else {
        glColor3f(0.9f, 0.9f, 0.9f);
    }
    glRasterPos3f(float(x - width / 2) / width, float(height / 2 - y) / height, 1.0f);
    glCallLists(5, GL_UNSIGNED_BYTE, "Exit");
    y += 36;

}
