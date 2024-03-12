#include "Models/MD5Model.h"
#include "Models/TextureReader.h"

MD5Model* MD5Model::createMD5Model(const std::string& name,
	const std::string& objectsPath,
	const std::string& texturesPath)
{
	std::string meshFileName = objectsPath + name + ".md5mesh";
	MD5MeshFile meshFile(meshFileName.c_str());

	md5meshdata md5data;
	meshFile.getMeshData(md5data);

	std::string animFileName = objectsPath + name + ".md5anim";
	MD5AnimFile animFile(animFileName.c_str());

	md5animdata md5anim;
	animFile.getAnimData(md5anim);

	return new MD5Model(md5data, md5anim, texturesPath, (objectsPath + name + ".bounds"));
}

MD5Model::MD5Model(md5meshdata& md5data,
	md5animdata& md5anim,
	const std::string& texturesPath,
	const std::string& boundsFileName)
{
	glGenVertexArrays(1, &vertexArray);
	glBindVertexArray(vertexArray);

	createVertexBuffer(md5data);

	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
		(void*)offsetof(Vertex, position));
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
		(void*)offsetof(Vertex, normal));
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex),
		(void*)offsetof(Vertex, texcoord));

	createElementBuffer(md5data);

	glBindVertexArray(0);

	std::string path = texturesPath + std::string(md5data.meshes[0].shaderName);
	TextureReader reader(path.c_str());
	diffuseTex = reader.createOGLTexture();

	animation.buildAnimation(md5anim);

	bounds.LoadBoundaries(boundsFileName);
}

void MD5Model::createVertexBuffer(md5meshdata& md5data)
{
	size_t i;

	unsigned numVertices = md5data.meshes[0].numVertices;
	unskinned.resize(numVertices);
	vertices.resize(numVertices);
	md5mesh & mesh = md5data.meshes[0];
	std::vector<md5joint>& joints = md5data.joints;

	for (i = 0; i < numVertices; i++) {

		unskinned[i].texcoord = mesh.vertices[i].texcoord;
		unskinned[i].countWeight = mesh.vertices[i].countWeight;

		for (size_t j = 0; j < mesh.vertices[i].countWeight; j++) {

			size_t weightIdx = mesh.vertices[i].startWeight + j;

			unskinned[i].weightPosition[j] = mesh.weights[weightIdx].weightPos;
			unskinned[i].weightBias[j] = mesh.weights[weightIdx].bias;
			unskinned[i].jointId[j] = mesh.weights[weightIdx].joint;

			md5joint& joint = md5data.joints[unskinned[i].jointId[j]];

			vec3 rotatedWeight = joint.orientation.rotate(unskinned[i].weightPosition[j]);

			vertices[i].position += (joint.position + rotatedWeight) * unskinned[i].weightBias[j];

		}

	}
	
	vec3 normal;

	for (i = 0; i < mesh.numTriangles; i++) {
		unsigned int indices[3];
		memcpy(indices, mesh.triangles[i].vertIndices, 3 * sizeof(unsigned int));

		normal = cross(vertices[indices[1]].position - vertices[indices[0]].position,
			vertices[indices[2]].position - vertices[indices[0]].position);

		vertices[indices[0]].normal += normal;
		vertices[indices[1]].normal += normal;
		vertices[indices[2]].normal += normal;
	}

	for (i = 0; i < mesh.numVertices; i++) {
		vertices[i].normal = normalize(vertices[i].normal);
	}

	for (i = 0; i < unskinned.size(); i++) {
		for (size_t j = 0; j < 4; j++) {
			unsigned int jointId = unskinned[i].jointId[j];
			quat orientation = joints[jointId].orientation;
			normal = vertices[i].normal;

			orientation.x = -orientation.x;
			orientation.y = -orientation.y;
			orientation.z = -orientation.z;

			unskinned[i].weightNormal[j] = orientation.rotate(normal);
		}
	}

	glGenBuffers(1, &vertexBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
	glBufferData(GL_ARRAY_BUFFER, numVertices * sizeof(Vertex), vertices.data(), GL_DYNAMIC_DRAW);
}

void MD5Model::createElementBuffer(md5meshdata& md5data)
{
	unsigned int i;
	unsigned int numElements = md5data.meshes[0].triangles.size();

	std::vector<unsigned> elements(3 * numElements);
	for(i=0;i< numElements;i++){
		elements[3*i]   = md5data.meshes[0].triangles[i].vertIndices[0];
		elements[3*i+1] = md5data.meshes[0].triangles[i].vertIndices[1];
		elements[3*i+2] = md5data.meshes[0].triangles[i].vertIndices[2];
	}

	glGenBuffers(1, &elementBuffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementBuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, (3 * numElements) * sizeof(GLuint), elements.data(), GL_STATIC_DRAW);

	elementCount = 3 * numElements;

}

void MD5Model::setAnimation(double animTime)
{
	animation.buildFrame(animTime);
	animation.setVertices(vertices.data(), unskinned.data(), vertices.size());

	glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
	glBufferSubData(GL_ARRAY_BUFFER, 0, vertices.size() * sizeof(Vertex), vertices.data());
}

void MD5Model::draw(Shader& shader, mat4& translation, mat4&rotation, bool skipMVP, bool skipLighting)
{
	glUniformMatrix4fv(shader.translation, 1, GL_FALSE, (GLfloat*)&translation);
	glUniformMatrix4fv(shader.rotation, 1, GL_FALSE, (GLfloat*)&rotation);

	glUniform1ui(shader.skipMVP, skipMVP);
	glUniform1ui(shader.skipLighting, skipLighting);

	glBindTexture(GL_TEXTURE_2D, diffuseTex);

	glBindVertexArray(vertexArray);

	glDrawElements(
		GL_TRIANGLES,
		3 * elementCount,
		GL_UNSIGNED_INT,
		(void*)0
	);

	glBindVertexArray(0);
}


void Animation::buildAnimation(md5animdata& md5data) {

	unsigned int i;

	numJoints = md5data.numJoints;
	numFrames = md5data.numFrames;
	frameRate = md5data.frameRate;
	numAnimatedComponents = md5data.numAnimatedComponents;

	std::swap(joints, md5data.joints);
	std::swap(bounds, md5data.bounds);
	std::swap(baseframe, md5data.baseframe);
	std::swap(frames, md5data.frames);

	jointPosition.resize(joints.size());
	jointOrientation.resize(joints.size());
}

void getFrameValues(md5hierarchyjoint &joint, vec3& position, quat& orientation, md5frame& frame) {

	size_t i = joint.startIndex;

	if (joint.flags & 1) {
		position.x = frame.animatedComponents[i];
		i++;
	}
	if (joint.flags & 2) {
		position.y = frame.animatedComponents[i];
		i++;
	}
	if (joint.flags & 4) {
		position.z = frame.animatedComponents[i];
		i++;
	}
	if (joint.flags & 8) {
		orientation.x = frame.animatedComponents[i];
		i++;
	}
	if (joint.flags & 16) {
		orientation.y = frame.animatedComponents[i];
		i++;
	}
	if (joint.flags & 32) {
		orientation.z = frame.animatedComponents[i];
		i++;
	}

	orientation.calcW();

}

void Animation::setJoint(unsigned int i, unsigned int firstFrame, unsigned int secondFrame, float interpol)
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

void Animation::buildFrame(float animationTime)
{
	unsigned int firstFrame, secondFrame;
	float interpol;

	float animationFrames = animationTime * float(frameRate);
	animationFrames = fmod(animationFrames, float(numFrames));

	firstFrame = (int)floor(animationFrames) % numFrames;
	secondFrame = (int)ceil(animationFrames) % numFrames;

	interpol = animationFrames - float(firstFrame);

	for (size_t i = 0; i < numJoints; i++) {
		setJoint(i, firstFrame, secondFrame, interpol);
	}

}

void Animation::setVertices(Vertex* vertices, MD5Vertex* unskinned, unsigned int numVertices)
{
	for (size_t i = 0; i < numVertices; i++) {
		vec3 position;
		vec3 normal;
		for (size_t j = 0; j < unskinned[i].countWeight; j++) {

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
