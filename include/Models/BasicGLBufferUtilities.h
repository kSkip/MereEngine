#ifndef BASICGLBUFFERUTILITIES_H
#define BASICGLBUFFERUTILITIES_H

#include "Platform.h"

#ifndef STRUCT_VERTEX
#define STRUCT_VERTEX
struct vertex{

	GLfloat position[3];
	GLfloat texcoord[2];

};
#endif

GLuint basicQuadVertexBuffer();

GLuint basicQuadElementBuffer();

#endif // BASICGLBUFFERUTILITIES_H
