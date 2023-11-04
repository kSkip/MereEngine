#ifndef MD5MESHUTILITIES_H
#define MD5MESHUTILITIES_H

#include <stdio.h>
#include <string.h>

#include <vector>
#include <exception>

struct md5vertex {
	unsigned int index;
	float s,t;
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
	float weightPos[3];
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
	float position[3];
	float orientation[3];
};

struct md5meshdata {
	unsigned int numJoints;
	unsigned int numMeshes;

	std::vector<md5mesh> meshes;
	std::vector<md5joint> joints;
};

#define MAX_LINE_LENGTH 128

class MD5File {
protected:
	MD5File(const char* fileName) {
		fp = fopen(fileName, "r");
		memset(line, 0, sizeof line);
	}
	~MD5File() {
		fclose(fp);
	}
	MD5File(MD5File const &) = delete;
	MD5File& operator= (MD5File const &) = delete;

	char* getLine();
	unsigned int getValue(char*);
	void seekListBegin(char*);
	void seekListEnd();

private:
	FILE *fp;
	char line[MAX_LINE_LENGTH];
};

class MD5MeshFile : public MD5File {
public:
	MD5MeshFile(const char* fileName) : MD5File(fileName) {
		memset(buf, 0, sizeof buf);
	}
	~MD5MeshFile() {}
	MD5MeshFile(MD5MeshFile const &) = delete;
	MD5MeshFile& operator= (MD5MeshFile const &) = delete;

	void getMeshData(md5meshdata &);

private:
	char buf[MAX_LINE_LENGTH];

	void getJoint(md5joint &);
	void getVertex(md5vertex &);
	void getTri(md5triangle &);
	void getWeight(md5weight &);
};

#endif
