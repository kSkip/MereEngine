#ifndef GLBUFFERUTILITIES_H
#define GLBUFFERUTILITIES_H

#include "Platform.h"
#include "MD5MeshUtilities.h"
#include "VectorMath/VectorMath.h"

struct MD5Vertex {
	mat4x3 normal;
	vec2 texcoord;
	unsigned int countWeight;
	float weightBias[4];
	mat4x3 weightPosition;
	mat4x3 weightNormal;
	unsigned int jointId[4];
};

struct vertex{

	vec3 position;
	vec3 normal;
	vec2 texcoord;
	vec3 color;

};

GLuint MD5CreateVertexBuffer(struct md5meshdata* md5data, std::vector<vertex>& buffer_vertices, std::vector<MD5Vertex>& unskinned, size_t numVertices);

GLuint MD5CreateElementBuffer(struct md5meshdata* md5data, unsigned int* numElements);

GLuint MD5CreateTextureBuffer(struct md5meshdata* md5data, const char* shaderDirectory);

#endif
