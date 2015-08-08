#include "Utilities/TextToTexture.h"

void TextToTexture(const char* string, TTF_Font* font, SDL_Color textColor){

    GLuint texture_format;

    SDL_Surface* surface = TTF_RenderText_Blended(font,string,textColor);

    unsigned int colors = surface->format->BytesPerPixel;
    if (colors == 4) {   // alpha
        if (surface->format->Rmask == 0x000000ff)
            texture_format = GL_RGBA;
        else
            texture_format = GL_BGRA;
    } else {             // no alpha
        if (surface->format->Rmask == 0x000000ff)
            texture_format = GL_RGB;
        else
            texture_format = GL_BGR;
    }

    glTexImage2D(GL_TEXTURE_2D, 0, colors, surface->w, surface->h, 0, texture_format, GL_UNSIGNED_BYTE, surface->pixels);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    SDL_FreeSurface(surface);

}
