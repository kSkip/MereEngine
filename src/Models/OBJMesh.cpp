#include <vector>
#include <string>
#include <map>

#include "CommonTypes.h"
#include "Models/OBJMesh.h"
#include "Models/TextureReader.h"

struct FaceIndex {
	int v; // vertex
	int n; // normal
	int t; // tex coord

	FaceIndex(int v, int n, int t) : v(v), n(n), t(t) {}
};

bool operator<(const FaceIndex& a, const FaceIndex& b) {
	if (a.v != b.v) return (a.v < b.v);
	if (a.n != b.n) return (a.n < b.n);
	if (a.t != b.t) return (a.t < b.t);
	return false;
}

const PrefixLabel OBJMeshFile::tokens[] = {
	{"f", Prefix::FACE},
	{"mtllib", Prefix::MTL},
	{"o", Prefix::OBJECT},
	{"v", Prefix::VERTEX},
	{"vn", Prefix::NORMAL},
	{"vt", Prefix::TEXCOORD}
};

const int OBJMeshFile::numTokens = sizeof(OBJMeshFile::tokens) / sizeof(PrefixLabel);

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

void OBJMeshFile::getMesh(std::vector<Vertex>& vertexData,
	std::vector<unsigned int>& elements,
	const std::string& mtlPath,
	std::string& diffuse)
{
	char buffer[MAX_LINE_LENGTH];

	std::vector<vec3> vertices;
	std::vector<vec3> normals;
	std::vector<vec2> texcoords;
	std::vector<FaceIndex> faceIndices;

	vec3 val;
	vec2 tex;
	std::string objectName;

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
	int idx = 0;
	for (int i = 0; i < faceIndices.size(); ++i) {
		auto it = vertexIndices.find(faceIndices[i]);
		if (it != vertexIndices.end()) {
			elements.push_back(it->second);
		}
		else {
			Vertex vert;
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

OBJMesh* OBJMesh::createOBJMesh(const std::string& name,
	const std::string& objectsPath,
	const std::string& materialsPath,
	const std::string& texturesPath)
{
	OBJMeshFile file((objectsPath + name + ".obj").c_str());

	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;
	std::string diffuse;
	file.getMesh(vertices, indices, materialsPath, diffuse);

	return new OBJMesh(vertices, indices, (texturesPath + diffuse), (objectsPath + name + ".bounds"));
}

OBJMesh::OBJMesh(const std::vector<Vertex>& vertexData,
	const std::vector<unsigned int>& indices,
	const std::string& texturePath,
	const std::string& boundsFileName)
{
	glGenVertexArrays(1, &vertexArray);
	glBindVertexArray(vertexArray);

	glGenBuffers(1, &vertices);
	glBindBuffer(GL_ARRAY_BUFFER, vertices);
	glBufferData(GL_ARRAY_BUFFER, vertexData.size() * sizeof(Vertex), vertexData.data(), GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
		(void*)offsetof(Vertex, position));
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
		(void*)offsetof(Vertex, normal));
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex),
		(void*)offsetof(Vertex, texcoord));

	glGenBuffers(1, &elements);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elements);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), indices.data(), GL_STATIC_DRAW);

	glBindVertexArray(0);

	elementCount = indices.size();

	TextureReader reader(texturePath.c_str());
	texture = reader.createOGLTexture();

	bounds.LoadBoundaries(boundsFileName);
}

void OBJMesh::draw(Shader& shader, mat4& translation, mat4&rotation, bool skipMVP, bool skipLighting)
{
	glUniformMatrix4fv(shader.translation, 1, GL_FALSE, (GLfloat*)&translation);
	glUniformMatrix4fv(shader.rotation, 1, GL_FALSE, (GLfloat*)&rotation);

	glUniform1ui(shader.skipMVP, skipMVP);
	glUniform1ui(shader.skipLighting, skipLighting);

	glBindTexture(GL_TEXTURE_2D, texture);

	glBindVertexArray(vertexArray);

	glDrawElements(
		GL_TRIANGLES,
		elementCount,
		GL_UNSIGNED_INT,
		(void*)0
	);

	glBindVertexArray(0);
}
