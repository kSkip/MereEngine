#ifndef GLBUFFERUTILITIES_H
#define GLBUFFERUTILITIES_H

#include "Platform.h"
#include "MD5MeshUtilities.h"

#ifndef STRUCT_UNSKINNEDVERTEX
#define STRUCT_UNSKINNEDVERTEX
struct UnskinnedVertex{

	float normal[4][3];
	float texcoord[2];
	unsigned int countWeight;
	float weightBias[4];
	float weightPosition[4][3];
	float weightNormal[4][3];
	unsigned int jointId[4];

};
#endif

#ifndef STRUCT_VERTEX
#define STRUCT_VERTEX
struct vertex{

	float position[3];
	float normal[3];
	float texcoord[2];
	float color[3];

};
#endif

GLuint MD5CreateVertexBuffer(struct md5meshdata* md5data, unsigned int* numVertices); /*DO NOT USE! GPU SKINNING HAS NOT BEEN COMPLETED!*/

GLuint MD5CreateVertexBuffer(struct md5meshdata* md5data, std::vector<vertex>& buffer_vertices, std::vector<UnskinnedVertex>& unskinned, unsigned int numVertices);

GLuint MD5CreateElementBuffer(struct md5meshdata* md5data, unsigned int* numElements);

GLuint MD5CreateTextureBuffer(struct md5meshdata* md5data, const char* shaderDirectory);

#endif
