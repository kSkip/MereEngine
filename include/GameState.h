#ifndef GAMESTATE_H
#define GAMESTATE_H

#include <string>
#include <vector>
#include <list>
#include <map>
#include <queue>
#include <fstream>

#include "Shader.h"
#include "Boundary.h"
#include "Utilities/DataBlock.h"
#include "GameObjects/GameObject.h"
#include "GameObjects/Camera.h"

struct CursorPos {
    int x;
    int y;
    int dx;
    int dy;
    bool leftButtonDown;
};

struct WindowContext {
    CursorPos cursor;
    int width;
    int height;
};

enum class MenuAction {
    NOACTION,
    NEWGAME,
    LOADGAME,
    SAVEGAME,
    EXIT
};

class GameState {

    public:

        GameState();
        ~GameState();
        GameState(const GameState & rhs);

        void init(std::string rootDir);
        void clean();
        void setWindowSize(int, int);

        void loadNew(std::string levelfile);
        void loadSave(std::string savefile);
        void save();
        bool run(double);

        void movingForward(bool);
        void movingBackward(bool);
        void movingLeft(bool);
        void movingRight(bool);

        void handleSpacebar();
        void handleEscape();
        void handleLeftButtonDown();
        void handleMouseMove(int, int);
        void handleRelativeMouseMove(int, int);

        void firePrimaryWeapon();

    private:
        WindowContext context;
        std::string rootDir;
        std::string level;

        bool loaded;
        bool paused;
        bool cursorVisible;

        std::list<GameObject*> levelObjects; //contains all objects

        std::list<GameObject*> opaqueObjects; //contains only the opaque objects
        std::list<GameObject*> transparencyObjects; // conatins only the objects with transparent pixels

        void move(double);
        void render();
        void renderMenu(WindowContext&, MenuAction&);

        void insertOpaqueObject(GameObject* newGameObject);
        void insertTransparencyObject(GameObject* newGameObject);


        GLuint boundDiffuseTexture;

        std::map<std::string,ObjectData*> objectMap;

        Shader* levelShader;
        Camera* player;

        int screenwidth;
        int screenheight;
        float aspectRatio;

        void loadObjectData(DataBlock & objectDataBlock);
        void loadObjects(DataBlock & objectBlock);
};

#endif
