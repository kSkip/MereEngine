#ifndef BASICGLBUFFERUTILITIES_H
#define BASICGLBUFFERUTILITIES_H

#ifndef GL_GLEXT_PROTOTYPES
#define GL_GLEXT_PROTOTYPES
#endif

#include <GL/gl.h>

#ifndef _WIN32
#include <GL/glext.h>
#endif // NOT WIN32

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
