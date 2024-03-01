#ifndef ARMATURE_H
#define ARMATURE_H

#include "MD5Model.h"
#include "MD5Mesh.h"


class Armature{
	public:
		void buildArmature(struct md5animdata* md5data); 

		void buildFrame(float animationTime);

		void setVertices(struct vertex* vertices, MD5Vertex* unskinned, unsigned int numVertices);

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

#endif
