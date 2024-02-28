#include "Models/Armature.h"
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

void getFrameValues(md5hierarchyjoint &joint, vec3& position, quat& orientation, md5frame& frame){

	size_t i = joint.startIndex;

	if(joint.flags & 1){
		position.x = frame.animatedComponents[i];
		i++;
	}
	if(joint.flags & 2){
		position.y = frame.animatedComponents[i];
		i++;
	}
	if(joint.flags & 4){
		position.z = frame.animatedComponents[i];
		i++;
	}
	if(joint.flags & 8){
		orientation.x = frame.animatedComponents[i];
		i++;
	}
	if(joint.flags & 16){
		orientation.y = frame.animatedComponents[i];
		i++;
	}
	if(joint.flags & 32){
		orientation.z = frame.animatedComponents[i];
		i++;
	}

	orientation.calcW();

}

void Armature::setJoint(unsigned int i, unsigned int firstFrame, unsigned int secondFrame, float interpol)
{
	vec3 floorPosition;
	quat floorOrientation;
	vec3 ceilPosition;
	quat ceilOrientation;

	md5hierarchyjoint & joint = joints[i];
	md5baseframejoint & thisJointBase = baseframe[i];
	md5frame & first = frames[firstFrame];
	md5frame & second = frames[secondFrame];

	floorPosition = thisJointBase.position;
	floorOrientation = thisJointBase.orientation;

	ceilPosition = thisJointBase.position;
	ceilOrientation = thisJointBase.orientation;

	getFrameValues(joints[i], floorPosition, floorOrientation, first);

	getFrameValues(joints[i], ceilPosition, ceilOrientation, second);

	quat orientation = nlerp(floorOrientation, ceilOrientation, interpol);

	vec3 position = lerp(floorPosition, ceilPosition, interpol);

	if (joint.parentId >= 0) {
		vec3 rotatedPosition = jointOrientation[joint.parentId].rotate(position);
		jointPosition[i] = rotatedPosition + jointPosition[joint.parentId];
		jointOrientation[i] = jointOrientation[joint.parentId] * orientation;
	}
	else {
		jointPosition[i] = position;
		jointOrientation[i] = orientation;
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

void Armature::setVertices(struct vertex* vertices, MD5Vertex* unskinned, unsigned int numVertices){

	unsigned int i, j;

	for (i = 0; i < numVertices; i++) {

		vec3 position;
		vec3 normal;

		for (j = 0; j < unskinned[i].countWeight; j++) {

			vec3 jointPos = jointPosition[unskinned[i].jointId[j]];
			quat jointOrient = jointOrientation[unskinned[i].jointId[j]];

			vec3 weightPosition = jointOrient.rotate(unskinned[i].weightPosition[j]);
			vec3 weightNormal = jointOrient.rotate(unskinned[i].weightNormal[j]);

			position += (jointPos + weightPosition) * unskinned[i].weightBias[j];
			normal += weightNormal * unskinned[i].weightBias[j];

		}
		vertices[i].position = position;
		vertices[i].normal = normal;

	}

}
