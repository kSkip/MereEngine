#ifndef MD5MESHUTILITIES_H
#define MD5MESHUTILITIES_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "MD5CommonUtilities.h"

struct md5vertex{
	unsigned int index;
	float s,t;
	unsigned int startWeight, countWeight;
};

struct md5triangle{
	unsigned int index;
	unsigned int vertIndices[3];
};

struct md5weight{
	unsigned int index;
	unsigned int joint;
	float bias;
	float weightPos[3];
};

struct md5mesh{
	struct md5vertex* vertices;
	unsigned int numVertices;
	struct md5triangle* triangles;
	unsigned int numTriangles;
	struct md5weight* weights;
	unsigned int numWeights;
};

struct md5joint{
	char name[128];
	unsigned int id;
	char parentName[128];
	int parentId;
	float position[3];
	float orientation[3];
};

struct md5meshdata{
	unsigned int version;
	char commandline[128];
	unsigned int numJoints;	
	unsigned int numMeshes;

	struct md5mesh* meshes;
	struct md5joint* joints;
};

unsigned int getNumVerts(FILE* md5file);
void readVertices(FILE* md5file, struct md5vertex* vertices, unsigned int numVertices); 

unsigned int getNumTris(FILE* md5file);
void readTriangles(FILE* md5file, struct md5triangle* tris, unsigned int numTris);

unsigned int getNumWeights(FILE* md5file);
void readWeights(FILE* md5file, struct md5weight* weights, unsigned int numWeights);

unsigned int getNumJoints(FILE* md5file);
void readJoints(FILE* md5file, struct md5joint* joints, unsigned int numJoints);

unsigned int getNumMeshes(FILE* md5file);
void readMesh(FILE* md5file, struct md5mesh* meshes);

struct md5meshdata* getMD5MeshData(char* filename);

void freeMD5MeshData(struct md5meshdata* md5data);

#endif
