#ifndef TEXTTOTEXTURE_H
#define TEXTTOTEXTURE_H

#include <GL/gl.h>
#include <SDL/SDL_ttf.h>

void TextToTexture(const char* str, TTF_Font* font, SDL_Color textColor);

GLuint GenTextToTexture(const char* str, TTF_Font* font, SDL_Color textColor, float* aspect);

#endif
