#ifndef TEXTTOTEXTURE_H
#define TEXTTOTEXTURE_H

#include <GL/gl.h>
#include <SDL/SDL_ttf.h>

void TextToTexture(const char* string, TTF_Font* font, SDL_Color textColor);

#endif
