#ifndef GAMESTATE_H
#define GAMESTATE_H

#ifndef GL_GLEXT_PROTOTYPES
#define GL_GLEXT_PROTOTYPES
#endif

#include <string>
#include <vector>
#include <fstream>

#ifdef _WIN32
#include <windows.h>
#ifndef GLEW_H
#define GLEW_H
#include <GL/glew.h>
#endif // GLEW_H
#endif // WIN32

#include <GL/gl.h>
#include <GL/glu.h>

#ifndef _WIN32
#include <GL/glext.h>
#endif // WIN32

#include <SDL/SDL.h>
#include <SDL/SDL_ttf.h>

#include "GameObject.h"
#include "Shader.h"
#include "boundary.h"

class GameObject;
class Camera;

class GameState{
    friend class GameObject;
    friend class Character;
    public:
        GameState();
        ~GameState();
        GameState(const GameState & rhs);

        void init(double maxframerate);
        void clean();

        void loadNew(std::string levelfile);
        void loadSave(std::string savefile);
        void save();

        bool loop();
        bool eventHandler();
        void move(double deltatime);
        void render();

        bool loaded;

    private:
        std::list<GameObject*> levelObjects; //contains all objects

        std::list<GameObject*> opaqueObjects; //contains only the opaque objects
        std::list<GameObject*> transparencyObjects; // conatins only the objects with transparent pixels

        std::map<std::string,GLuint> levelTextures;
        Shader* levelShader;
        Camera* player;

        const SDL_VideoInfo* vidinfo;
        double maxframerate;
        float aspectRatio;
};

#endif
