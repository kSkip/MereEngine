#ifndef TEXTURES_H
#define TEXTURES_H

#include <string>
#include <vector>

#ifdef _WIN32
#include <windows.h>
#endif // WIN32

#include <GL/gl.h>

struct Texture{
            GLuint TextureId;
            std::string TextureName;
};

class Textures{
    public:
        Textures();
        ~Textures();
        Textures(const Textures & rhs);

        std::vector<Texture> TextureList;
};

#endif
