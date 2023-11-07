#ifndef ARMATURE_H
#define ARMATURE_H

#include "MD5GLBufferUtilities.h"
#include "MD5MeshUtilities.h"

typedef struct vec3 { float v[3]; } vec3;
typedef struct quat4 { float v[4]; } quat4;

class Armature{
	public:
		void buildArmature(struct md5animdata* md5data); 

		void buildFrame(float animationTime);

		void setVertices(struct vertex* vertices, struct UnskinnedVertex* unskinned, unsigned int numVertices);

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
		std::vector<quat4> jointOrientation;

		void setJoint(unsigned int i, unsigned int firstFrame, unsigned int secondFrame, float interpol);
};

#endif
