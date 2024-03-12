#ifndef MD5MODEL_H
#define MD5MODEL_H

#include "Platform.h"
#include "MD5Mesh.h"
#include "VectorMath.h"
#include "CommonTypes.h"
#include "Boundary.h"
#include "Shader.h"

struct MD5Vertex {
	mat4x3 normal;
	vec2 texcoord;
	unsigned int countWeight;
	float weightBias[4];
	mat4x3 weightPosition;
	mat4x3 weightNormal;
	unsigned int jointId[4];
};

class Animation {
public:
	void buildAnimation(md5animdata& md5data);

	void buildFrame(float animationTime);

	void setVertices(Vertex* vertices, MD5Vertex* unskinned, unsigned int numVertices);

	double getTotalTime() { return double(numFrames) / double(frameRate); }

private:
	unsigned int numFrames;
	unsigned int numJoints;
	unsigned int frameRate;
	unsigned int numAnimatedComponents;
	std::vector<md5hierarchyjoint> joints;
	std::vector<md5bounds> bounds;
	std::vector<md5baseframejoint> baseframe;
	std::vector<md5frame> frames;

	std::vector<vec3> jointPosition;
	std::vector<quat> jointOrientation;

	void setJoint(unsigned int i, unsigned int firstFrame, unsigned int secondFrame, float interpol);
};

struct MD5Model {

	static MD5Model* createMD5Model(const std::string&, const std::string&, const std::string&);
	MD5Model(md5meshdata&, md5animdata&, const std::string&, const std::string&);

	void createVertexBuffer(md5meshdata&);
	void createElementBuffer(md5meshdata&);
	void setAnimation(double);
	void draw(Shader&, mat4&, mat4&, bool, bool);

	std::vector<MD5Vertex> unskinned;
	std::vector<Vertex> vertices;

	GLuint vertexArray;
	GLuint vertexBuffer;
	GLuint elementBuffer;
	GLsizei elementCount;
	GLuint diffuseTex;

	Animation animation;

	boundary bounds;
};

#endif
