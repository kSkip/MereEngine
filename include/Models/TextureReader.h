#ifndef TEXTUREREADER_H
#define TEXTURE_READER_H

#include <stdio.h>
#include "Platform.h"

class TextureReader {
public:
	TextureReader(const char*);
	~TextureReader();
	GLuint createOGLTexture();

private:
	unsigned char* data;
	int width;
	int height;
	int channels;
};

#endif // !TEXTUREREADER_H
