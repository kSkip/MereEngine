#ifndef MD5MESHUTILITIES_H
#define MD5MESHUTILITIES_H

#include <stdio.h>
#include <string.h>

#include <vector>
#include <exception>

#include "DataFile.h"
#include "VectorMath/MathTypes.h"

struct md5vertex {
	unsigned int index;
	vec2 texcoord;
	unsigned int startWeight, countWeight;
};

struct md5triangle {
	unsigned int index;
	unsigned int vertIndices[3];
};

struct md5weight {
	unsigned int index;
	unsigned int joint;
	float bias;
	vec3 weightPos;
};

struct md5mesh {
	char shaderName[128];
	std::vector<md5vertex> vertices;
	unsigned int numVertices;
	std::vector<md5triangle> triangles;
	unsigned int numTriangles;
	std::vector<md5weight> weights;
	unsigned int numWeights;
};

struct md5joint {
	unsigned int id;
	int parentId;
	vec3 position;
	float orientation[3];
};

struct md5meshdata {
	unsigned int numJoints;
	unsigned int numMeshes;

	std::vector<md5mesh> meshes;
	std::vector<md5joint> joints;
};

struct md5hierarchyjoint {
	char name[128];
	unsigned int id;
	int parentId;
	int flags;
	int startIndex;
};

struct md5bounds {
	float min[3];
	float max[3];
};

struct md5baseframejoint {
	float position[3];
	float orientation[3];
};

struct md5frame {
	unsigned int index;
	std::vector<float> animatedComponents;
};

struct md5animdata {
	unsigned int version;
	char commandline[128];
	unsigned int numFrames;
	unsigned int numJoints;
	unsigned int frameRate;
	unsigned int numAnimatedComponents;
	std::vector<md5hierarchyjoint> joints;
	std::vector<md5bounds> bounds;
	std::vector<md5baseframejoint> baseframe;
	std::vector<md5frame> frames;
};

class MD5File : public DataFile {
protected:
	MD5File(const char* fileName) : DataFile(fileName) {}

	unsigned int getValue(char*);
	void seekListBegin(char*);
	void seekListEnd();
};

class MD5MeshFile : public MD5File {
public:
	MD5MeshFile(const char* fileName) : MD5File(fileName) {
		memset(buf, 0, sizeof buf);
	}

	void getMeshData(md5meshdata &);

private:
	char buf[MAX_LINE_LENGTH];

	void getJoint(md5joint &);
	void getVertex(md5vertex &);
	void getTri(md5triangle &);
	void getWeight(md5weight &);
};

class MD5AnimFile : public MD5File {
public:
	MD5AnimFile(const char* fileName) : MD5File(fileName) {}

	void getAnimData(md5animdata &);

private:
	void getJoint(md5hierarchyjoint &);
	void getBounds(md5bounds &);
	void getBaseFrameJoint(md5baseframejoint &);
};

#endif
