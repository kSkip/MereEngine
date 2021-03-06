#include "Models/Armature.hpp"
#include "VectorMath/quaternion_math.h"
#include <iostream>
#include <cmath>

Armature::Armature(){

	numRoots = 0;
	numNodes = 0;

}

Armature::Armature(const Armature & rhs){

}

Armature::~Armature(){

	unsigned int i;

	for(i=0;i<numRoots;i++){

		deleteNode(rootNodes[i]);

	}

}

void Armature::deleteNode(Node* pNode){

	unsigned int i;

	for(i=0;i<pNode->numChildren;i++){

		deleteNode(pNode->children[i]);

	}

	delete pNode;

}

bool Armature::push(Node* pNode, struct md5hierarchyjoint* joint){

	unsigned int i;

	if(pNode->id == joint->parentId){

		Node* pNew = new Node;
		pNode->children[(pNode->numChildren)++] = pNew;
		pNew->numChildren = 0;
		pNew->id = joint->id;
		allNodes[pNew->id] = pNew;
		numNodes++;
		pNew->parentId = pNode->id;
		pNew->flags = joint->flags;
		pNew->startIndex = joint->startIndex;

		return true;

	}

	for(i=0;i<pNode->numChildren;i++){

		if(push(pNode->children[i],joint)) return true;

	}

	return false;
}

bool Armature::pushDown(struct md5hierarchyjoint* joint){

	unsigned int i;

	if(joint->parentId < 0){

		Node* pNew = new Node;
		rootNodes[numRoots++] = pNew;
		pNew->numChildren = 0;
		pNew->id = joint->id;
		allNodes[pNew->id] = pNew;
		numNodes++;
		pNew->parentId = -1;
		pNew->flags = joint->flags;
		pNew->startIndex = joint->startIndex;

		return true;

	}

	for(i=0;i<numRoots;i++){

		if(push(rootNodes[i],joint)) return true;

	}

	return false;

}

void Armature::buildArmature(struct md5animdata* md5data){

	unsigned int i;

	numJoints = md5data->numJoints;
	numFrames = md5data->numFrames;
	frameRate = md5data->frameRate;
	numAnimatedComponents = md5data->numAnimatedComponents;

	bounds = (struct md5bounds*)malloc(numFrames*sizeof(struct md5bounds));
	memcpy(bounds,md5data->bounds,numFrames*sizeof(struct md5bounds));

	baseframe = (struct md5baseframejoint*)malloc(numJoints*sizeof(struct md5baseframejoint));
	memcpy(baseframe,md5data->baseframe,numJoints*sizeof(struct md5baseframejoint));

	frames = (struct md5frame*)malloc(numFrames*sizeof(struct md5frame));
	memcpy(frames,md5data->frames,numFrames*sizeof(struct md5frame));

	for(i=0;i<numFrames;i++){

		frames[i].animatedComponents = (float*)malloc(numAnimatedComponents*sizeof(float));
		memcpy(frames[i].animatedComponents,md5data->frames[i].animatedComponents,numAnimatedComponents*sizeof(float));

	}

	for(i=0;i<md5data->numJoints;i++){

		pushDown(&(md5data->joints[i]));

	}

}

void Armature::getFrameValues(Node* pNode, float position[3], float orientation[4], struct md5frame* frame){

	unsigned int i = 0;

	if(pNode->flags & 1){
		position[0]    = frame->animatedComponents[pNode->startIndex+i];
		i++;
	}
	if(pNode->flags & 2){
		position[1]    = frame->animatedComponents[pNode->startIndex+i];
		i++;
	}
	if(pNode->flags & 4){
		position[2]    = frame->animatedComponents[pNode->startIndex+i];
		i++;
	}
	if(pNode->flags & 8){
		orientation[0] = frame->animatedComponents[pNode->startIndex+i];
		i++;
	}
	if(pNode->flags & 16){
		orientation[1] = frame->animatedComponents[pNode->startIndex+i];
		i++;
	}
	if(pNode->flags & 32){
		orientation[2] = frame->animatedComponents[pNode->startIndex+i];
		i++;
	}

	quaternion_w(orientation);

}

void Armature::setJoint(Node* pNode, Node* pParent, unsigned int firstFrame, unsigned int secondFrame, float interpol){

	unsigned int i;

	float floorPosition[3];
	float floorOrientation[4];
	float ceilPosition[3];
	float ceilOrientation[4];

	float position[3];
	float orientation[4];

	struct md5baseframejoint* thisJointBase = &(baseframe[pNode->id]);
	struct md5frame* first = &(frames[firstFrame]);
	struct md5frame* second = &(frames[secondFrame]);

	memcpy(floorPosition,thisJointBase->position,3*sizeof(float));
	memcpy(floorOrientation,thisJointBase->orientation,3*sizeof(float));

	memcpy(ceilPosition,thisJointBase->position,3*sizeof(float));
	memcpy(ceilOrientation,thisJointBase->orientation,3*sizeof(float));

	getFrameValues(pNode,floorPosition,floorOrientation,first);

	getFrameValues(pNode,ceilPosition,ceilOrientation,second);

	interpolate_quaternion(floorOrientation,ceilOrientation,interpol,orientation);

	interpolate_position(floorPosition,ceilPosition,interpol,position);

	if(pParent){

		float rotatedPosition[3];
		rotate_position(pParent->currentOrientation,position,rotatedPosition);

		pNode->currentPosition[0] = rotatedPosition[0] + pParent->currentPosition[0];
		pNode->currentPosition[1] = rotatedPosition[1] + pParent->currentPosition[1];
		pNode->currentPosition[2] = rotatedPosition[2] + pParent->currentPosition[2];

		float rotatedQuat[4];
		quaternion_product(pParent->currentOrientation,orientation,rotatedQuat);

		memcpy(pNode->currentOrientation,rotatedQuat,4*sizeof(float));

	}else{

		memcpy(pNode->currentPosition,position,3*sizeof(float));
		memcpy(pNode->currentOrientation,orientation,4*sizeof(float));

	}

	for(i=0;i<pNode->numChildren;i++){

		setJoint(pNode->children[i],pNode,firstFrame,secondFrame,interpol);

	}

}

void Armature::buildFrame(float animationTime, float* jointPositions){

	unsigned int i;
	unsigned int firstFrame, secondFrame;
	float interpol;

	float animationFrames = animationTime * float(frameRate);
	animationFrames = fmod(animationFrames,float(numFrames));

	firstFrame = (int)floor(animationFrames) % numFrames;
	secondFrame = (int)ceil(animationFrames) % numFrames;

	interpol = animationFrames - float(firstFrame);

	for(i=0;i<numRoots;i++){

		setJoint(rootNodes[i], NULL, firstFrame, secondFrame, interpol);

	}

	if(jointPositions){

		for(i=0;i<numNodes;i++){

			memcpy(&(jointPositions[3*i]),allNodes[i]->currentPosition,3*sizeof(float));

		}

	}

}

void Armature::setVertices(struct vertex* vertices, struct UnskinnedVertex* unskinned, unsigned int numVertices){

	unsigned int i, j;

	for(i=0;i<numVertices;i++){

		memset(vertices[i].position,0,3*sizeof(float));
		memset(vertices[i].normal,0,3*sizeof(float));

		for(j=0;j<unskinned[i].countWeight;j++){

			float jointPos[3];
			float jointOrient[4];
			memcpy(jointPos   ,allNodes[unskinned[i].jointId[j]]->currentPosition   ,3*sizeof(float));
			memcpy(jointOrient,allNodes[unskinned[i].jointId[j]]->currentOrientation,4*sizeof(float));

			float weightPosition[3];
			float weightNormal[3];

			rotate_position(jointOrient,unskinned[i].weightPosition[j],weightPosition);
			rotate_position(jointOrient,unskinned[i].weightNormal[j],weightNormal);

			vertices[i].position[0] += (jointPos[0] + weightPosition[0])*unskinned[i].weightBias[j];
			vertices[i].position[1] += (jointPos[1] + weightPosition[1])*unskinned[i].weightBias[j];
			vertices[i].position[2] += (jointPos[2] + weightPosition[2])*unskinned[i].weightBias[j];

			vertices[i].normal[0] += weightNormal[0]*unskinned[i].weightBias[j];
			vertices[i].normal[1] += weightNormal[1]*unskinned[i].weightBias[j];
			vertices[i].normal[2] += weightNormal[2]*unskinned[i].weightBias[j];

		}

	}

}
