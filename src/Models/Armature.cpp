#include "Models/Armature.h"
#include "VectorMath/QuaternionMath.h"
#include <iostream>
#include <cmath>

void Armature::buildArmature(struct md5animdata* md5data){

	unsigned int i;

	numJoints = md5data->numJoints;
	numFrames = md5data->numFrames;
	frameRate = md5data->frameRate;
	numAnimatedComponents = md5data->numAnimatedComponents;

	std::swap(joints, md5data->joints);
	std::swap(bounds, md5data->bounds);
	std::swap(baseframe, md5data->baseframe);
	std::swap(frames, md5data->frames);

	jointPosition.resize(joints.size());
	jointOrientation.resize(joints.size());
}

void getFrameValues(md5hierarchyjoint &joint, float position[3], float orientation[4], md5frame &frame){

	size_t i = joint.startIndex;

	if(joint.flags & 1){
		position[0] = frame.animatedComponents[i];
		i++;
	}
	if(joint.flags & 2){
		position[1] = frame.animatedComponents[i];
		i++;
	}
	if(joint.flags & 4){
		position[2] = frame.animatedComponents[i];
		i++;
	}
	if(joint.flags & 8){
		orientation[0] = frame.animatedComponents[i];
		i++;
	}
	if(joint.flags & 16){
		orientation[1] = frame.animatedComponents[i];
		i++;
	}
	if(joint.flags & 32){
		orientation[2] = frame.animatedComponents[i];
		i++;
	}

	quaternion_w(orientation);

}

void Armature::setJoint(unsigned int i, unsigned int firstFrame, unsigned int secondFrame, float interpol)
{
	float floorPosition[3];
	float floorOrientation[4];
	float ceilPosition[3];
	float ceilOrientation[4];

	float position[3];
	float orientation[4];

	md5hierarchyjoint & joint = joints[i];
	md5baseframejoint & thisJointBase = baseframe[i];
	md5frame & first = frames[firstFrame];
	md5frame & second = frames[secondFrame];

	memcpy(floorPosition, thisJointBase.position, 3 * sizeof(float));
	memcpy(floorOrientation, thisJointBase.orientation, 3 * sizeof(float));

	memcpy(ceilPosition, thisJointBase.position, 3 * sizeof(float));
	memcpy(ceilOrientation, thisJointBase.orientation, 3 * sizeof(float));

	getFrameValues(joints[i], floorPosition, floorOrientation, first);

	getFrameValues(joints[i], ceilPosition, ceilOrientation, second);

	interpolate_quaternion(floorOrientation, ceilOrientation, interpol, orientation);

	interpolate_position(floorPosition, ceilPosition, interpol, position);

	if(joint.parentId >= 0) {
		float rotatedPosition[3];
		rotate_position(jointOrientation[joint.parentId].v, position, rotatedPosition);

		jointPosition[i].v[0] = rotatedPosition[0] + jointPosition[joint.parentId].v[0];
		jointPosition[i].v[1] = rotatedPosition[1] + jointPosition[joint.parentId].v[1];
		jointPosition[i].v[2] = rotatedPosition[2] + jointPosition[joint.parentId].v[2];

		float rotatedQuat[4];
		quaternion_product(jointOrientation[joint.parentId].v, orientation, rotatedQuat);

		memcpy(jointOrientation[i].v, rotatedQuat, 4 * sizeof(float));

	}else{

		memcpy(jointPosition[i].v, position, 3 * sizeof(float));
		memcpy(jointOrientation[i].v, orientation, 4 * sizeof(float));

	}

}

void Armature::buildFrame(float animationTime)
{
	unsigned int firstFrame, secondFrame;
	float interpol;

	float animationFrames = animationTime * float(frameRate);
	animationFrames = fmod(animationFrames,float(numFrames));

	firstFrame = (int)floor(animationFrames) % numFrames;
	secondFrame = (int)ceil(animationFrames) % numFrames;

	interpol = animationFrames - float(firstFrame);

	for (size_t i = 0; i < numJoints; i++) {
		setJoint(i, firstFrame, secondFrame, interpol);
	}

}

void Armature::setVertices(struct vertex* vertices, struct UnskinnedVertex* unskinned, unsigned int numVertices){

	unsigned int i, j;

	for (i = 0; i < numVertices; i++) {

		memset(vertices[i].position,0,3*sizeof(float));
		memset(vertices[i].normal,0,3*sizeof(float));

		for (j = 0; j < unskinned[i].countWeight; j++) {

			float jointPos[3];
			float jointOrient[4];
			memcpy(jointPos, jointPosition[unskinned[i].jointId[j]].v, 3 * sizeof(float));
			memcpy(jointOrient, jointOrientation[unskinned[i].jointId[j]].v, 4 * sizeof(float));

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
