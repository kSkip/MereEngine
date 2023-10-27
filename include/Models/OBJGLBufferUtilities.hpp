#ifndef OBJGLBUFFERUTILITIES_HPP
#define OBJGLBUFFERUTILITIES_HPP

#include "Platform.h"

#include "../third-party/tiny_obj_loader.h"

#ifndef STRUCT_STATICVERTEX
#define STRUCT_STATICVERTEX
struct StaticVertex{
	float position[3];
	float normal[3];
	float texcoord[2];
};
#endif

GLuint OBJCreateVertexBuffer(std::vector<tinyobj::shape_t>* shapes);

GLuint OBJCreateElementBuffer(std::vector<tinyobj::shape_t>* shapes, unsigned int* numElements);

GLuint OBJCreateTextureBuffer(std::vector<tinyobj::material_t>* materials, const char* shaderDirectory);

#endif
