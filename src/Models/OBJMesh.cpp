#include <vector>
#include <string>
#include <map>

#include "Models/OBJMesh.h"
#include "Models/MD5GLBufferUtilities.h"
#include "Models/TextureReader.h"

namespace OBJ {

bool operator<(const FaceIndex& a, const FaceIndex& b) {
	if (a.v != b.v) return (a.v < b.v);
	if (a.n != b.n) return (a.n < b.n);
	if (a.t != b.t) return (a.t < b.t);
	return false;
}

const PrefixLabel MeshFile::tokens[] = {
	{"f", Prefix::FACE},
	{"mtllib", Prefix::MTL},
	{"o", Prefix::OBJECT},
	{"v", Prefix::VERTEX},
	{"vn", Prefix::NORMAL},
	{"vt", Prefix::TEXCOORD}
};

const int MeshFile::numTokens = sizeof(MeshFile::tokens) / sizeof(PrefixLabel);

struct OBJVec3 {
	float x;
	float y;
	float z;
};

struct OBJVec2 {
	float x;
	float y;
};

void parseFace(const char* values, std::vector<FaceIndex>& faceIndices)
{
	int read, offset = 0;
	int v, n, t;
	while (3 == sscanf(values + offset, "%d/%d/%d%n", &v, &t, &n, &read)) {
		FaceIndex index(v, n, t);
		faceIndices.push_back(index);
		offset += read;
	}
}

void MeshFile::getMesh(Mesh& data, const char* mtlPath, unsigned int* numElements)
{
	char buffer[MAX_LINE_LENGTH];

	std::vector<OBJVec3> vertices;
	std::vector<OBJVec3> normals;
	std::vector<OBJVec2> texcoords;
	std::vector<FaceIndex> faceIndices;

	float x, y, z;
	std::string objectName;
	std::string diffuse;

	while (char* line = getLineOrNull()) {
		if (line[0] == '#') continue;

		int read;
		int count = sscanf(line, "%s%n", buffer, &read);
		if (count < 0) {
			throw;
		}
		line += read;

		switch (getToken(buffer)) {
		case Prefix::OBJECT:
			{
				line += strspn(line, " \t\r\n");
				char *end = line + strcspn(line, " \t\r\n");
				objectName = std::string(line, end);
			}
			break;
		case Prefix::VERTEX:
			if (3 == sscanf(line, "%f%f%f", &x, &y, &z)) {
				vertices.push_back({ x, y, z });
			}
			break;
		case Prefix::NORMAL:
			if (3 == sscanf(line, "%f%f%f", &x, &y, &z)) {
				normals.push_back({ x, y, z });
			}
			break;
		case Prefix::TEXCOORD:
			if (2 == sscanf(line, "%f%f", &x, &y)) {
				texcoords.push_back({ x, y });
			}
			break;
		case Prefix::FACE:
			parseFace(line, faceIndices);
			break;
		case Prefix::MTL:
			{
				line += strspn(line, " \t\r\n");
				char *end = line + strcspn(line, " \t\r\n");
				std::string mtlFilename(line, end);
				MTLFile mtl((mtlPath + mtlFilename).c_str());
				diffuse = mtl.getTextureName();
			}
			break;
		default:
			break;
		}
	}

	std::map<FaceIndex, int> vertexIndices;
	std::vector<vertex> vertexData;
	std::vector<unsigned int> elements;
	int idx = 0;
	for (int i = 0; i < faceIndices.size(); ++i) {
		auto it = vertexIndices.find(faceIndices[i]);
		if (it != vertexIndices.end()) {
			elements.push_back(it->second);
		}
		else {
			vertex vert;
			vert.position[0] = vertices[faceIndices[i].v - 1].x;
			vert.position[1] = vertices[faceIndices[i].v - 1].y;
			vert.position[2] = vertices[faceIndices[i].v - 1].z;
			vert.normal[0] = normals[faceIndices[i].n - 1].x;
			vert.normal[1] = normals[faceIndices[i].n - 1].y;
			vert.normal[2] = normals[faceIndices[i].n - 1].z;
			vert.texcoord[0] = texcoords[faceIndices[i].t - 1].x;
			vert.texcoord[1] = texcoords[faceIndices[i].t - 1].y;
			vertexData.push_back(vert);
			elements.push_back(idx);
			vertexIndices[faceIndices[i]] = idx;
			++idx;
		}
	}

	glGenBuffers(1, &data.vertices);
	glBindBuffer(GL_ARRAY_BUFFER, data.vertices);
	glBufferData(GL_ARRAY_BUFFER, vertexData.size() * sizeof(vertex), vertexData.data(), GL_STATIC_DRAW);

	glGenBuffers(1, &data.elements);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, data.elements);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, elements.size() * sizeof(GLuint), elements.data(), GL_STATIC_DRAW);

	*numElements = elements.size() / 3;

	std::string path = std::string(mtlPath) + diffuse;

	TextureReader reader(path.c_str());
	data.texture = reader.createOGLTexture();
}

std::string MTLFile::getTextureName()
{
	char buffer[MAX_LINE_LENGTH] = {};
	int read;
	while (char* line = getLineOrNull()) {
		int count = sscanf(line, "%s%n", buffer, &read);
		if (count > 0) {
			if (!strcmp(buffer, "map_Kd")) {
				if (sscanf(line + read, "%s", buffer) > 0) {
					return buffer;
				}
			}
		}
	}
	return buffer;
}

}