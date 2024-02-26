#include "Models/MD5GLBufferUtilities.h"
#include <string>
#include "VectorMath/QuaternionMath.h"
#include <math.h>
#include "Models/TextureReader.h"

/*Version of MD5CreateVertexBuffer that takes the data structure obtained directly from the md5mesh file and puts the data
into a format that is suitable for CPU skinning*/
GLuint MD5CreateVertexBuffer(struct md5meshdata* md5data, std::vector<vertex> &bindPoseVertices, std::vector<MD5Vertex> &unskinned, size_t numVertices)
{
	size_t i;

	unskinned.resize(numVertices);
	bindPoseVertices.resize(numVertices);
	md5mesh & mesh = md5data->meshes[0];
	std::vector<md5joint>& joints = md5data->joints;

	for (i = 0; i < numVertices; i++) {

		unskinned[i].texcoord = mesh.vertices[i].texcoord;
		unskinned[i].countWeight = mesh.vertices[i].countWeight;

		for (size_t j = 0; j < mesh.vertices[i].countWeight; j++) {

			size_t weightIdx = mesh.vertices[i].startWeight + j;

			unskinned[i].weightPosition[j] = mesh.weights[weightIdx].weightPos;
			unskinned[i].weightBias[j] = mesh.weights[weightIdx].bias;
			unskinned[i].jointId[j] = mesh.weights[weightIdx].joint;

			struct md5joint* joint = &(md5data->joints[unskinned[i].jointId[j]]);

			vec3 rotatedWeight;
			quat orientation;

			memcpy(&orientation,joint->orientation,3*sizeof(float));
			quaternion_w((float*)&orientation);

			rotate_position((float*)&orientation, (float*)&(unskinned[i].weightPosition[j]), (float*)&rotatedWeight);

			bindPoseVertices[i].position += (joint->position + rotatedWeight) * unskinned[i].weightBias[j];

		}

	}
	
	quat orientation;
	vec3 normal;

	for (i = 0; i < mesh.numTriangles; i++) {
		unsigned int indices[3];
		memcpy(indices, mesh.triangles[i].vertIndices, 3 * sizeof(unsigned int));

		normal = cross(bindPoseVertices[indices[1]].position - bindPoseVertices[indices[0]].position,
			           bindPoseVertices[indices[2]].position - bindPoseVertices[indices[0]].position);

		bindPoseVertices[indices[0]].normal += normal;
		bindPoseVertices[indices[1]].normal += normal;
		bindPoseVertices[indices[2]].normal += normal;
	}

	for (i = 0; i < mesh.numVertices; i++) {
		bindPoseVertices[i].normal = normalize(bindPoseVertices[i].normal);
	}

	for (i = 0; i < unskinned.size(); i++) {
		for (size_t j = 0; j < 4; j++) {
			unsigned int jointId = unskinned[i].jointId[j];
			memcpy(&orientation, joints[jointId].orientation, 3 * sizeof(float));
			normal = bindPoseVertices[i].normal;

			orientation.x *= -1;
			orientation.y *= -1;
			orientation.z *= -1;

			quaternion_w((float*)&orientation);

			rotate_position((float*)&orientation, (float*)&normal, (float*)&unskinned[i].weightNormal[j]);

		}
	}

	GLuint buffer;
	glGenBuffers(1, &buffer);
	glBindBuffer(GL_ARRAY_BUFFER, buffer);
	glBufferData(GL_ARRAY_BUFFER,numVertices*sizeof(vertex), bindPoseVertices.data(), GL_STREAM_DRAW);

	return buffer;
}

/*MD5CreatElementBuffer takes the data obtained directly from the md5mesh file
and creates the element buffer for glDrawElements*/
GLuint MD5CreateElementBuffer(struct md5meshdata* md5data, unsigned int* numElements){

unsigned int i;
unsigned int* elements;
GLuint buffer;

	*numElements = md5data->meshes[0].numTriangles;

	elements = (unsigned int*)malloc(3*(*numElements)*sizeof(unsigned int));
	for(i=0;i<(*numElements);i++){
		elements[3*i]   = md5data->meshes[0].triangles[i].vertIndices[0];
		elements[3*i+1] = md5data->meshes[0].triangles[i].vertIndices[1];
		elements[3*i+2] = md5data->meshes[0].triangles[i].vertIndices[2];
	}

	glGenBuffers(1, &buffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER,(3*(*numElements))*sizeof(GLuint), elements, GL_STATIC_DRAW);

	free(elements);

	return buffer;

}

GLuint MD5CreateTextureBuffer(struct md5meshdata* md5data, const char* shaderDirectory){

	GLuint texture;

	std::string path = std::string(shaderDirectory) + std::string(md5data->meshes[0].shaderName);

	TextureReader reader(path.c_str());
	texture = reader.createOGLTexture();

	return texture;

}
