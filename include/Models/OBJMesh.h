#ifndef OBJMESH_H
#define OBJMESH_H

#include "Platform.h"
#include "DataFile.h"
#include "Boundary.h"
#include "Shader.h"
#include "CommonTypes.h"

enum class Prefix {
	UNKNOWN = -1,
	FACE,
	MTL,
	OBJECT,
	VERTEX,
	NORMAL,
	TEXCOORD
};

struct PrefixLabel {
	const char *token;
	Prefix value;
};

class OBJMeshFile : public DataFile {
public:
	OBJMeshFile(const char* fileName) : DataFile(fileName) {}

	void getMesh(std::vector<Vertex>&,
		std::vector<unsigned int>&,
		const std::string&,
		std::string&);

protected:
	static const PrefixLabel tokens[];
	static const int numTokens;

	static int prefixComp(const void* a, const void* b)
	{
		const char* lhs = static_cast<const char*>(a);
		const PrefixLabel* rhs = static_cast<const PrefixLabel*>(b);
		return strcmp(lhs, rhs->token);
	}

	Prefix getToken(const char *token)
	{
		void* element = bsearch(token, tokens, numTokens, sizeof(PrefixLabel), prefixComp);
		if (element) {
			return static_cast<PrefixLabel*>(element)->value;
		}
		else {
			return Prefix::UNKNOWN;
		}
	}

};

class MTLFile : public DataFile {
public:
	MTLFile(const char* fileName) : DataFile(fileName) {}

	std::string getTextureName();
};

struct OBJMesh {
	GLuint vertexArray;
	GLuint vertices;
	GLuint elements;
	GLuint elementCount;
	GLuint texture;

	boundary bounds;

	static OBJMesh* createOBJMesh(const std::string&,
		const std::string&,
		const std::string&,
		const std::string&);

	OBJMesh(const std::vector<Vertex>&, const std::vector<unsigned int>&, const std::string&, const std::string&);

	void draw(Shader&, mat4&, mat4&, bool, bool);
};

#endif