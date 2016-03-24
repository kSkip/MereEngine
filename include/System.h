#ifndef SYSTEM_H
#define SYSTEM_H

#ifdef _WIN32
#include <windows.h>
#endif

#include <stdexcept>

#include <SDL/SDL.h>

#ifdef _WIN32
#ifndef GLEW_H
#define GLEW_H
#include <GL/glew.h>
#endif // GLEW_H
#define NO_SDL_GLEXT
#undef main
#endif // WIN32

#include <SDL/SDL_opengl.h>
#include <GL/gl.h>
#include "GameState.h"
#include "MenuState.h"

class System{

    public:

        System();
        ~System();
        System(const System & rhs);

        bool init(std::string& rootDir);
        bool loop();

    private:

        SDL_Surface* win;
        const SDL_VideoInfo* vidinfo;

        GameState state;
        MenuState mainmenu;

        std::string rootDir;

};

#endif
