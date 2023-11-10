#include "Models/MD5GLBufferUtilities.h"
#include <string>
#include "VectorMath/quaternion_math.h"
#include <math.h>
#include "Models/TextureReader.h"

/*Function declarations for functions used only in this source file*/

void crossProduct(float inPlaneVec0[3], float inPlaneVec1[3], float product[3]);

void MD5CalcNormals(float* vertices, struct md5meshdata* data, float* normals);

void MD5CalcNormals(struct vertex* vertices, struct md5meshdata* data, float* normals);

void MD5CalcWeightNormals(struct UnskinnedVertex* vertices, unsigned int numVertices, struct md5joint* joints, float* normals);

/*MD5CreateVertexBuffer takes the data structure obtained directly from the md5mesh file and puts the data
into a format that is suitable for passing to the vertex shader for model skinning*/
/*DO NOT USE! GPU SKINNING HAS NOT BEEN COMPLETED!*/
GLuint MD5CreateVertexBuffer(struct md5meshdata* md5data, unsigned int* numVertices){

unsigned int i, j;
struct UnskinnedVertex* vertices;
float* bindPoseVertices;
float* normals;
GLuint buffer;

	*numVertices = md5data->meshes[0].numVertices;

	vertices = (struct UnskinnedVertex*)malloc((*numVertices)*sizeof(struct UnskinnedVertex));
	bindPoseVertices = (float*)malloc(3*(*numVertices)*sizeof(float));

	for(i=0;i<(*numVertices);i++){

		memset(&(vertices[i]),0,sizeof(UnskinnedVertex));

		vertices[i].texcoord[0] = md5data->meshes[0].vertices[i].s;
		vertices[i].texcoord[1] = md5data->meshes[0].vertices[i].t;

		bindPoseVertices[3*i] = 0.0f;
		bindPoseVertices[3*i+1] = 0.0f;
		bindPoseVertices[3*i+2] = 0.0f;

		for(j=0;j<md5data->meshes[0].vertices[i].countWeight;j++){

			unsigned int startWeight = md5data->meshes[0].vertices[i].startWeight;

			vertices[i].weightPosition[j][0] = md5data->meshes[0].weights[startWeight+j].weightPos[0];
			vertices[i].weightPosition[j][1] = md5data->meshes[0].weights[startWeight+j].weightPos[1];
			vertices[i].weightPosition[j][2] = md5data->meshes[0].weights[startWeight+j].weightPos[2];

			vertices[i].weightBias[j] = md5data->meshes[0].weights[startWeight+j].bias;

			vertices[i].jointId[j] = md5data->meshes[0].weights[startWeight+j].joint;

			struct md5joint* joint = &(md5data->joints[vertices[i].jointId[j]]);

			float rotatedWeight[3];
			float orientation[4];

			memcpy(orientation,joint->orientation,3*sizeof(float));
			quaternion_w(orientation);

			rotate_position(orientation,vertices[i].weightPosition[j],rotatedWeight);

			bindPoseVertices[3*i]   += (joint->position[0]+rotatedWeight[0])*vertices[i].weightBias[j];
			bindPoseVertices[3*i+1] += (joint->position[1]+rotatedWeight[1])*vertices[i].weightBias[j];
			bindPoseVertices[3*i+2] += (joint->position[2]+rotatedWeight[2])*vertices[i].weightBias[j];

		}

	}

	normals = (float*)malloc(3*(*numVertices)*sizeof(float));
	MD5CalcNormals(bindPoseVertices,md5data,normals);

	MD5CalcWeightNormals(vertices,*numVertices,md5data->joints.data(),normals);

	glGenBuffers(1, &buffer);
	glBindBuffer(GL_ARRAY_BUFFER, buffer);
	glBufferData(GL_ARRAY_BUFFER,(*numVertices)*sizeof(UnskinnedVertex), vertices, GL_STATIC_DRAW);

	free(vertices);
	free(normals);
	free(bindPoseVertices);

	return buffer;
}

/*Version of MD5CreateVertexBuffer that takes the data structure obtained directly from the md5mesh file and puts the data
into a format that is suitable for CPU skinning*/
GLuint MD5CreateVertexBuffer(struct md5meshdata* md5data, std::vector<vertex> &bindPoseVertices, std::vector<UnskinnedVertex> &unskinned, unsigned int numVertices)
{
	unsigned int i, j;
	GLuint buffer;

	unskinned.resize(numVertices);
	bindPoseVertices.resize(numVertices);
	md5mesh & mesh = md5data->meshes[0];

	for (i = 0; i < numVertices; i++) {

		unskinned[i].texcoord[0] = mesh.vertices[i].s;
		unskinned[i].texcoord[1] = mesh.vertices[i].t;

		bindPoseVertices[i].position[0] = 0.0f;
		bindPoseVertices[i].position[1] = 0.0f;
		bindPoseVertices[i].position[2] = 0.0f;

		unskinned[i].countWeight = mesh.vertices[i].countWeight;

		for (j = 0; j < mesh.vertices[i].countWeight; j++) {

			size_t weightIdx = mesh.vertices[i].startWeight + (size_t)j;

			unskinned[i].weightPosition[j][0] = mesh.weights[weightIdx].weightPos[0];
			unskinned[i].weightPosition[j][1] = mesh.weights[weightIdx].weightPos[1];
			unskinned[i].weightPosition[j][2] = mesh.weights[weightIdx].weightPos[2];

			unskinned[i].weightBias[j] = mesh.weights[weightIdx].bias;

			unskinned[i].jointId[j] = mesh.weights[weightIdx].joint;

			struct md5joint* joint = &(md5data->joints[unskinned[i].jointId[j]]);

			float rotatedWeight[3];
			float orientation[4];

			memcpy(orientation,joint->orientation,3*sizeof(float));
			quaternion_w(orientation);

			rotate_position(orientation,unskinned[i].weightPosition[j],rotatedWeight);

			bindPoseVertices[i].position[0] += (joint->position[0]+rotatedWeight[0])*unskinned[i].weightBias[j];
			bindPoseVertices[i].position[1] += (joint->position[1]+rotatedWeight[1])*unskinned[i].weightBias[j];
			bindPoseVertices[i].position[2] += (joint->position[2]+rotatedWeight[2])*unskinned[i].weightBias[j];

		}

	}

	std::vector<float> normals(3ULL * numVertices);
	MD5CalcNormals(bindPoseVertices.data(),md5data,normals.data());

	MD5CalcWeightNormals(unskinned.data(),numVertices,md5data->joints.data(),normals.data());

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

/*crossProduct produces, well,.. a vector cross product!*/
void crossProduct(float vec0[3], float vec1[3], float product[3]){

	product[0] = vec0[1]*vec1[2] - vec1[1]*vec0[2];
	product[1] = vec1[0]*vec0[2] - vec0[0]*vec1[2];
	product[2] = vec0[0]*vec1[1] - vec1[0]*vec0[1];

}

/*MD5CalcNormals calculates the normals of the bind pose model*/
void MD5CalcNormals(float* vertices, struct md5meshdata* data, float* normals){

unsigned int i;
float p0[3], p1[3], p2[3], inPlaneVec0[3], inPlaneVec1[3], product[3];

	for(i=0;i<data->meshes[0].numVertices;i++){
		normals[i] = 0.0f;
	}

	for(i=0;i<data->meshes[0].numTriangles;i++){
		unsigned int indices[3];
		memcpy(indices,data->meshes[0].triangles[i].vertIndices,3*sizeof(unsigned int));
		memcpy(p0, &(vertices[3*indices[0]]), 3*sizeof(float));
		memcpy(p1, &(vertices[3*indices[1]]), 3*sizeof(float));
		memcpy(p2, &(vertices[3*indices[2]]), 3*sizeof(float));

		inPlaneVec0[0] = p1[0] - p0[0];
		inPlaneVec0[1] = p1[1] - p0[1];
		inPlaneVec0[2] = p1[2] - p0[2];

		inPlaneVec1[0] = p2[0] - p0[0];
		inPlaneVec1[1] = p2[1] - p0[1];
		inPlaneVec1[2] = p2[2] - p0[2];

		crossProduct(inPlaneVec0,inPlaneVec1,product);

		normals[3*indices[0]] += product[0];
		normals[3*indices[0]+1] += product[1];
		normals[3*indices[0]+2] += product[2];

		normals[3*indices[1]] += product[0];
		normals[3*indices[1]+1] += product[1];
		normals[3*indices[1]+2] += product[2];

		normals[3*indices[2]] += product[0];
		normals[3*indices[2]+1] += product[1];
		normals[3*indices[2]+2] += product[2];
	}

	for(i=0;i<data->meshes[0].numVertices;i++){
		float factor;
		factor = normals[3*i]*normals[3*i] + normals[3*i+1]*normals[3*i+1] + normals[3*i+2]*normals[3*i+2];
		factor = sqrt(factor);

		normals[3*i] /= factor;
		normals[3*i+1] /= factor;
		normals[3*i+2] /= factor;
	}
}

/*MD5CalcNormals calculates the normals of the bind pose model*/
void MD5CalcNormals(struct vertex* vertices, struct md5meshdata* data, float* normals){

unsigned int i;
float p0[3], p1[3], p2[3], inPlaneVec0[3], inPlaneVec1[3], product[3];

	for(i=0;i<3*data->meshes[0].numVertices;i++){
		normals[i] = 0.0f;
	}

	for(i=0;i<data->meshes[0].numTriangles;i++){
		unsigned int indices[3];
		memcpy(indices,data->meshes[0].triangles[i].vertIndices,3*sizeof(unsigned int));
		memcpy(p0, &(vertices[indices[0]].position[0]), 3*sizeof(float));
		memcpy(p1, &(vertices[indices[1]].position[0]), 3*sizeof(float));
		memcpy(p2, &(vertices[indices[2]].position[0]), 3*sizeof(float));

		inPlaneVec0[0] = p1[0] - p0[0];
		inPlaneVec0[1] = p1[1] - p0[1];
		inPlaneVec0[2] = p1[2] - p0[2];

		inPlaneVec1[0] = p2[0] - p0[0];
		inPlaneVec1[1] = p2[1] - p0[1];
		inPlaneVec1[2] = p2[2] - p0[2];

		crossProduct(inPlaneVec0,inPlaneVec1,product);

		normals[3*indices[0]] += product[0];
		normals[3*indices[0]+1] += product[1];
		normals[3*indices[0]+2] += product[2];

		normals[3*indices[1]] += product[0];
		normals[3*indices[1]+1] += product[1];
		normals[3*indices[1]+2] += product[2];

		normals[3*indices[2]] += product[0];
		normals[3*indices[2]+1] += product[1];
		normals[3*indices[2]+2] += product[2];
	}

	for(i=0;i<data->meshes[0].numVertices;i++){
		float factor;
		factor = normals[3*i]*normals[3*i] + normals[3*i+1]*normals[3*i+1] + normals[3*i+2]*normals[3*i+2];
		factor = sqrt(factor);

		normals[3*i] /= factor;
		normals[3*i+1] /= factor;
		normals[3*i+2] /= factor;

		vertices[i].normal[0] = normals[3*i];
		vertices[i].normal[1] = normals[3*i+1];
		vertices[i].normal[2] = normals[3*i+2];
	}
}

/*MD5CalcWeightNormals calculates the inverse bindpose orientation of the normals calculated from
bind pose*/
void MD5CalcWeightNormals(struct UnskinnedVertex* vertices, unsigned int numVertices, struct md5joint* joints, float* normals){

unsigned int i, j;
float orientation[4], normal[3];

	for(i=0;i<numVertices;i++){
		for(j=0;j<4;j++){
			unsigned int jointId = vertices[i].jointId[j];
			memcpy(orientation,joints[jointId].orientation,3*sizeof(float));
			memcpy(normal,&(normals[3*i]),3*sizeof(float));

			orientation[0] *= -1;
			orientation[1] *= -1;
			orientation[2] *= -1;

			quaternion_w(orientation);

			rotate_position(orientation,normal,vertices[i].weightNormal[j]);

		}
	}
}
