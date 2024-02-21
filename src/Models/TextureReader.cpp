#include "Models/TextureReader.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

TextureReader::TextureReader(const char* fileName) {
	data = stbi_load(fileName, &width, &height, &channels, 4);
	channels = 4; // TODO: Expose constructor argument;
}

TextureReader::~TextureReader() {
	stbi_image_free(data);
}

GLuint TextureReader::createOGLTexture()
{
	GLuint texId;
	glGenTextures(1, &texId);

	glBindTexture(GL_TEXTURE_2D, texId);

	GLint format = 0;
	switch (channels) {
	case 1:
		format = GL_LUMINANCE;
		break;
	case 2:
		format = GL_LUMINANCE_ALPHA;
		break;
	case 3:
		format = GL_RGB;
		break;
	case 4:
		format = GL_RGBA;
		break;
	}

	for (int j = 0; j * 2 < height; ++j) {
		int topRow = j * width * channels;
		int bottomRow = (height - 1 - j) * width * channels;
		for (int i = width * channels; i > 0; --i) {
			unsigned char temp = data[topRow];
			data[topRow] = data[bottomRow];
			data[bottomRow] = temp;
			++topRow;
			++bottomRow;
		}
	}

	glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	return texId;
}