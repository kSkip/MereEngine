#ifndef OBJMESH_H
#define OBJMESH_H

#include "Platform.h"
#include "DataFile.h"

namespace OBJ {

struct Mesh {
	GLuint vertices;
	GLuint elements;
	GLuint texture;
};

enum class Prefix {
	UNKNOWN = -1,
	FACE,
	MTL,
	OBJECT,
	VERTEX,
	NORMAL,
	TEXCOORD
};

struct FaceIndex {
	int v; // vertex
	int n; // normal
	int t; // tex coord

	FaceIndex(int v, int n, int t) : v(v), n(n), t(t) {}
};

struct PrefixLabel {
	const char *token;
	Prefix value;
};

class MeshFile : public DataFile {
public:
	MeshFile(const char* fileName) : DataFile(fileName) {}

	void getMesh(Mesh&, const char*, unsigned int*);

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

}

#endif