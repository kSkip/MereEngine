#include <vector>
#include <string>
#include <map>

#include "Models/OBJMesh.h"
#include "Models/MD5Model.h"
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

	std::vector<vec3> vertices;
	std::vector<vec3> normals;
	std::vector<vec2> texcoords;
	std::vector<FaceIndex> faceIndices;

	vec3 val;
	vec2 tex;
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
			if (3 == sscanf(line, "%f%f%f", &val.x, &val.y, &val.z)) {
				vertices.push_back(val);
			}
			break;
		case Prefix::NORMAL:
			if (3 == sscanf(line, "%f%f%f", &val.x, &val.y, &val.z)) {
				normals.push_back(val);
			}
			break;
		case Prefix::TEXCOORD:
			if (2 == sscanf(line, "%f%f", &tex.x, &tex.y)) {
				texcoords.push_back(tex);
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
			vert.position.x = vertices[faceIndices[i].v - 1].x;
			vert.position.y = vertices[faceIndices[i].v - 1].y;
			vert.position.z = vertices[faceIndices[i].v - 1].z;
			vert.normal.x = normals[faceIndices[i].n - 1].x;
			vert.normal.y = normals[faceIndices[i].n - 1].y;
			vert.normal.z = normals[faceIndices[i].n - 1].z;
			vert.texcoord.s = texcoords[faceIndices[i].t - 1].x;
			vert.texcoord.t = texcoords[faceIndices[i].t - 1].y;
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