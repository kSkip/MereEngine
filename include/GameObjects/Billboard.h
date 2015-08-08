#ifndef BILLBOARD_H
#define BILLBOARD_H

#include "GameObjects/GameObject.h"

class Billboard : public GameObject{

    public:

        Billboard();
        Billboard(const char* directory, const char* texturefile, float position[2], float scale[2], GameState* state);
        Billboard(GLuint texture, float position[2], float scale[2], GameState* state);
        ~Billboard();
        Billboard(const Billboard & rhs);

        GLuint getTextureId() const {return data->diffuseTex;}
        void setTextureId(GLuint newTex){ data->diffuseTex = newTex;}

        void render(GameState* state);

};

#endif
