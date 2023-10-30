#ifndef ARMATURE_H
#define ARMATURE_H

#define MAXCHILDREN 20
#define MAXNODES 20

#include "MD5GLBufferUtilities.h"
#include "MD5AnimUtilities.h"

class Armature{
	public:
		Armature();
		Armature(const Armature & rhs);
		~Armature();

		void buildArmature(struct md5animdata* md5data); 

		void buildFrame(float animationTime, float* jointPositions);

		void setVertices(struct vertex* vertices, struct UnskinnedVertex* unskinned, unsigned int numVertices);

		double getTotalTime() { return double(numFrames) / double(frameRate); }

	private:
		class Node{
			friend class Armature;

			Node* children[MAXCHILDREN];
			unsigned int numChildren;
			int id;
			int parentId;
			int flags;
			int startIndex;

			float currentPosition[3];
			float currentOrientation[4];
		};

		Node* rootNodes[MAXCHILDREN];
		unsigned int numRoots;

		Node* allNodes[MAXNODES];
		unsigned int numNodes;

		unsigned int numFrames;
		unsigned int numJoints;
		unsigned int frameRate;
		unsigned int numAnimatedComponents;
		struct md5bounds* bounds;
		struct md5baseframejoint* baseframe;
		struct md5frame* frames;

		bool pushDown(struct md5hierarchyjoint* joint);
		bool push(Node* pNode, struct md5hierarchyjoint* joint);
		void deleteNode(Node* pNode);

		void getFrameValues(Node* pNode, float position[3], float orientation[4], struct md5frame* frame);
		void setJoint(Node* pNode, Node* pParent, unsigned int firstFrame, unsigned int secondFrame, float interpol);
};

#endif
