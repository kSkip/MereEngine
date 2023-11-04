#include "Models/MD5MeshUtilities.h"
#include "Models/MD5CommonUtilities.h"

char* MD5File::getLine()
{
	char *l = fgets(line, MAX_LINE_LENGTH, fp);
	if (l == NULL || feof(fp)) {
		throw;
	}
	return l;
}

unsigned int MD5File::getValue(char* name)
{
	char tag[MAX_LINE_LENGTH] = {};
	unsigned int value;
	int count = 0;

	do {
		char *l = getLine();
		count = sscanf(l, "%s%d", tag, &value);
	} while (count < 2 && strcmp(tag, name));

	return value;
}

void MD5File::seekListBegin(char* name)
{
	char tag[MAX_LINE_LENGTH] = {};
	char bracket[2] = {};
	int count = 0;

	do {
		char *l = getLine();
		count = sscanf(l, "%s%s", tag, bracket);
	} while (count < 2 && strcmp(tag, name) && strcmp(bracket, "{"));
}

void MD5File::seekListEnd()
{
	char bracket[2] = {};
	int count = 0;

	do {
		char *l = getLine();
		count = sscanf(l, "%1s", bracket);
	} while (count < 1 && strcmp(bracket, "}"));
}

void MD5MeshFile::getMeshData(md5meshdata &data)
{
	unsigned int version = getValue("MD5Version");
	if (version != 10) {
		throw;
	}

	int i, j;

	data.numJoints = getValue("numJoints");

	data.numMeshes = getValue("numMeshes");

	data.joints.resize(data.numJoints);
	seekListBegin("joints");
	for (i = 0; i < data.numJoints; i++) {
		getJoint(data.joints[i]);
		data.joints[i].id = i;
	}
	seekListEnd();

	data.meshes.resize(data.numMeshes);
	for (i = 0; i < data.numMeshes; i++) {
		md5mesh & mesh = data.meshes[i];
		seekListBegin("mesh");

		{
			char tag[MAX_LINE_LENGTH] = {};
			int count = 0;

			do {
				char *l = getLine();
				count = sscanf(l, "%s \"%s\"", tag, mesh.shaderName);
			} while (count < 2 && strcmp(tag, "shader"));
		}
		mesh.shaderName[strlen(mesh.shaderName) - 1] = 0;

		mesh.numVertices = getValue("numverts");
		mesh.vertices.resize(mesh.numVertices);
		for (j = 0; j < mesh.numVertices; j++) {
			getVertex(mesh.vertices[j]);
		}

		mesh.numTriangles = getValue("numtris");
		mesh.triangles.resize(mesh.numTriangles);
		for (j = 0; j < mesh.numTriangles; j++) {
			getTri(mesh.triangles[j]);
		}

		mesh.numWeights = getValue("numweights");
		mesh.weights.resize(mesh.numWeights);
		for (j = 0; j < mesh.numVertices; j++) {
			getWeight(mesh.weights[j]);
		}
		seekListEnd();
	}
}

void MD5MeshFile::getJoint(md5joint &j)
{
	char jointName[MAX_LINE_LENGTH];
	char parentName[MAX_LINE_LENGTH];
	char left[2], right[2];

	char *l = getLine();
	int count = sscanf(l,
		"%s%d%s%f%f%f%s%s%f%f%f%s",
		jointName, &j.parentId,
		left, j.position, j.position + 1, j.position + 2, right,
		left, j.orientation, j.orientation + 1, j.orientation + 2, right);
	if (count != 12) {
		throw;
	}
}

void MD5MeshFile::getVertex(md5vertex & v)
{
	char left[2], right[2];
	char *l = getLine();
	int count = sscanf(l, "%s%d%s%f%f%s%d%d", buf, &v.index,
		left, &v.s, &v.t, right, &v.startWeight, &v.countWeight);
	if (count != 8) {
		throw;
	}
}

void MD5MeshFile::getTri(md5triangle & t)
{
	char *l = getLine();
	int count = sscanf(l, "%s%d%d%d%d", buf, &t.index,
		&t.vertIndices[0], &t.vertIndices[1], &t.vertIndices[2]);
	if (count != 5) {
		throw;
	}
}

void MD5MeshFile::getWeight(md5weight &w)
{
	char left[2], right[2];
	char *l = getLine();
	int count = sscanf(l, "%s%d%d%f%s%f%f%f%s", buf, &w.index, &w.joint, &w.bias,
		left, &w.weightPos[0], &w.weightPos[1], &w.weightPos[2], right);
	if (count != 9) {
		throw;
	}
}
