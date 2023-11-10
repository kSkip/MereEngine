#include "Models/OBJGLBufferUtilities.h"
#include "Models/MD5GLBufferUtilities.h"
#include "Models/TextureReader.h"
#include <iostream>

GLuint OBJCreateVertexBuffer(std::vector<tinyobj::shape_t>* shapes)
{
	GLuint vbuf;
	size_t numVertices = (*shapes)[0].mesh.positions.size() / 3;
	std::vector<vertex> vertices(numVertices);

	for (size_t i = 0; i < numVertices; i++) {
		vertices[i].position[0] = (*shapes)[0].mesh.positions[3*i];
		vertices[i].position[1] = (*shapes)[0].mesh.positions[3*i+1];
		vertices[i].position[2] = (*shapes)[0].mesh.positions[3*i+2];

		vertices[i].normal[0] = (*shapes)[0].mesh.normals[3*i];
		vertices[i].normal[1] = (*shapes)[0].mesh.normals[3*i+1];
		vertices[i].normal[2] = (*shapes)[0].mesh.normals[3*i+2];

		vertices[i].texcoord[0] = (*shapes)[0].mesh.texcoords[2*i];
		vertices[i].texcoord[1] = (*shapes)[0].mesh.texcoords[2*i+1];

	}

	glGenBuffers(1, &vbuf);
	glBindBuffer(GL_ARRAY_BUFFER, vbuf);
	glBufferData(GL_ARRAY_BUFFER, numVertices *sizeof(vertex), vertices.data(), GL_STATIC_DRAW);

	return vbuf;

}

GLuint OBJCreateElementBuffer(std::vector<tinyobj::shape_t>* shapes, unsigned int* numElements){

	GLuint ebuf;

	*numElements = (*shapes)[0].mesh.indices.size() / 3;

	glGenBuffers(1, &ebuf);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebuf);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, (*shapes)[0].mesh.indices.size()*sizeof(GLuint), &(*shapes)[0].mesh.indices[0], GL_STATIC_DRAW);

	return ebuf;

}

GLuint OBJCreateTextureBuffer(std::vector<tinyobj::material_t>* materials, const char* shaderDirectory){

	GLuint texture;

	std::string path = std::string(shaderDirectory) + (*materials)[0].diffuse_texname;

	TextureReader reader(path.c_str());
	texture = reader.createOGLTexture();

	return texture;

}
