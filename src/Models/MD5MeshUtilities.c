#include "Models/MD5MeshUtilities.h"
#include "Models/MD5CommonUtilities.h"

unsigned int getNumVerts(FILE* md5file);
void readVertices(FILE* md5file, struct md5vertex* vertices, unsigned int numVertices);

unsigned int getNumTris(FILE* md5file);
void readTriangles(FILE* md5file, struct md5triangle* tris, unsigned int numTris);

unsigned int getNumWeights(FILE* md5file);
void readWeights(FILE* md5file, struct md5weight* weights, unsigned int numWeights);

unsigned int getNumJoints(FILE* md5file);
void readJoints(FILE* md5file, struct md5joint* joints, unsigned int numJoints);

unsigned int getNumMeshes(FILE* md5file);
void readMesh(FILE* md5file, struct md5mesh* meshes, char* shaderName);

unsigned int getNumVerts(FILE* md5file){

char line[128], aux[128];
char* pStr;
unsigned int numVerts;

	while(strcmp(aux,"numverts") && !feof(md5file)){
		fgets(line,128,md5file);
		pStr = strtok(line," \"()\t\n/");
		if(pStr) sprintf(aux,"%s",pStr);
	}

	pStr = strtok(NULL," \"()\t\n/");
	if(pStr) sscanf(pStr,"%d",&numVerts);
	else{
			printf("ERROR: parsing md5mesh\n");
			exit(1);
	}

	return numVerts;
}

void readVertices(FILE* md5file, struct md5vertex* vertices, unsigned int numVertices){

char line[128], aux[128];
char* pStr;
unsigned int i = 0;
unsigned int vertIndex, startWeight, countWeight;
float  s, t;

	for(i=0;i<numVertices;i++){

		fgets(line,128,md5file);
		pStr = strtok(line," \"()\t\n/");
		if(pStr) sprintf(aux,"%s",pStr);
		else{
			printf("ERROR: parsing md5mesh\n");
			exit(1);
		}
		if(strcmp(aux,"vert")){
			printf("ERROR: parsing md5mesh\n");
			exit(1);
		}

		pStr = strtok(NULL," \"()\t\n/");
		if(pStr) sscanf(pStr,"%d",&vertIndex);
		else{
			printf("ERROR: parsing md5mesh\n");
			exit(1);
		}

		pStr = strtok(NULL," \"()\t\n/");
		if(pStr) sscanf(pStr,"%f",&s);
		else{
			printf("ERROR: parsing md5mesh\n");
			exit(1);
		}

		pStr = strtok(NULL," \"()\t\n/");
		if(pStr) sscanf(pStr,"%f",&t);
		else{
			printf("ERROR: parsing md5mesh\n");
			exit(1);
		}

		pStr = strtok(NULL," \"()\t\n/");
		if(pStr) sscanf(pStr,"%d",&startWeight);
		else{
			printf("ERROR: parsing md5mesh\n");
			exit(1);
		}

		pStr = strtok(NULL," \"()\t\n/");
		if(pStr) sscanf(pStr,"%d",&countWeight);
		else{
			printf("ERROR: parsing md5mesh\n");
			exit(1);
		}

		vertices[i].index = vertIndex;
		vertices[i].s = s;
		vertices[i].t = t;
		vertices[i].startWeight = startWeight;
		vertices[i].countWeight = countWeight;

	}
}

unsigned int getNumTris(FILE* md5file){

char line[128], aux[128];
char* pStr;
unsigned int numTris;

	while(strcmp(aux,"numtris") && !feof(md5file)){
		fgets(line,128,md5file);
		pStr = strtok(line," \"()\t\n/");
		if(pStr) sprintf(aux,"%s",pStr);
	}
	pStr = strtok(NULL," \"()\t\n/");
	if(pStr) sscanf(pStr,"%d",&numTris);
	else{
			printf("ERROR: parsing md5mesh\n");
			exit(1);
	}

	return numTris;
}

void readTriangles(FILE* md5file, struct md5triangle* tris, unsigned int numTris){

char line[128], aux[128];
char* pStr;
unsigned int i = 0;
unsigned int triIndex, vertIndices[3];

	for(i=0;i<numTris;i++){

		fgets(line,128,md5file);
		pStr = strtok(line," \"()\t\n/");
		if(pStr) sprintf(aux,"%s",pStr);
		else{
			printf("ERROR: parsing md5mesh\n");
			exit(1);
		}
		if(strcmp(aux,"tri")){
			printf("ERROR: parsing md5mesh\n");
			exit(1);
		}

		pStr = strtok(NULL," \"()\t\n/");
		if(pStr) sscanf(pStr,"%d",&triIndex);
		else{
			printf("ERROR: parsing md5mesh\n");
			exit(1);
		}

		pStr = strtok(NULL," \"()\t\n/");
		if(pStr) sscanf(pStr,"%d",&vertIndices[0]);
		else{
			printf("ERROR: parsing md5mesh\n");
			exit(1);
		}

		pStr = strtok(NULL," \"()\t\n/");
		if(pStr) sscanf(pStr,"%d",&vertIndices[1]);
		else{
			printf("ERROR: parsing md5mesh\n");
			exit(1);
		}

		pStr = strtok(NULL," \"()\t\n/");
		if(pStr) sscanf(pStr,"%d",&vertIndices[2]);
		else{
			printf("ERROR: parsing md5mesh\n");
			exit(1);
		}

		tris[i].index = triIndex;
		memcpy(tris[i].vertIndices,vertIndices,3*sizeof(unsigned int));

	}

}

unsigned int getNumWeights(FILE* md5file){

char line[128], aux[128];
char* pStr;
unsigned int numWeights;

	while(strcmp(aux,"numweights") && !feof(md5file)){
		fgets(line,128,md5file);
		pStr = strtok(line," \"()\t\n/");
		if(pStr) sprintf(aux,"%s",pStr);
	}
	pStr = strtok(NULL," \"()\t\n/");
	if(pStr) sscanf(pStr,"%d",&numWeights);
	else{
			printf("ERROR: parsing md5mesh\n");
			exit(1);
	}

	return numWeights;

}

void readWeights(FILE* md5file, struct md5weight* weights, unsigned int numWeights){

char line[128], aux[128];
char* pStr;
unsigned int i = 0;
unsigned int weightIndex, joint;
float bias;
float weightPos[3];

	for(i=0;i<numWeights;i++){

		fgets(line,128,md5file);
		pStr = strtok(line," \"()\t\n/");
		if(pStr) sprintf(aux,"%s",pStr);
		else{
			printf("ERROR: parsing md5mesh\n");
			exit(1);
		}
		if(strcmp(aux,"weight")){
			printf("ERROR: parsing md5mesh\n");
			exit(1);
		}

		pStr = strtok(NULL," \"()\t\n/");
		if(pStr) sscanf(pStr,"%d",&weightIndex);
		else{
			printf("ERROR: parsing md5mesh\n");
			exit(1);
		}

		pStr = strtok(NULL," \"()\t\n/");
		if(pStr) sscanf(pStr,"%d",&joint);
		else{
			printf("ERROR: parsing md5mesh\n");
			exit(1);
		}

		pStr = strtok(NULL," \"()\t\n/");
		if(pStr) sscanf(pStr,"%f",&bias);
		else{
			printf("ERROR: parsing md5mesh\n");
			exit(1);
		}

		pStr = strtok(NULL," \"()\t\n/");
		if(pStr) sscanf(pStr,"%f",&weightPos[0]);
		else{
			printf("ERROR: parsing md5mesh\n");
			exit(1);
		}

		pStr = strtok(NULL," \"()\t\n/");
		if(pStr) sscanf(pStr,"%f",&weightPos[1]);
		else{
			printf("ERROR: parsing md5mesh\n");
			exit(1);
		}

		pStr = strtok(NULL," \"()\t\n/");
		if(pStr) sscanf(pStr,"%f",&weightPos[2]);
		else{
			printf("ERROR: parsing md5mesh\n");
			exit(1);
		}

		weights[i].index = weightIndex;
		weights[i].joint = joint;
		weights[i].bias = bias;
		memcpy(weights[i].weightPos,weightPos,3*sizeof(float));

	}

}

unsigned int getNumMeshes(FILE* md5file){

char line[128], aux[128];
char* pStr;
unsigned int numMeshes;

	while(strcmp(aux,"numMeshes") && !feof(md5file)){
		fgets(line,128,md5file);
		pStr = strtok(line," \"()\t\n/");
		if(pStr) sprintf(aux,"%s",pStr);
	}
	pStr = strtok(NULL," \"()\t\n/");
	if(pStr) sscanf(pStr,"%d",&numMeshes);
	else{
			printf("ERROR: parsing md5mesh\n");
			exit(1);
	}

	return numMeshes;

}

void readMesh(FILE* md5file, struct md5mesh* meshes, char* shaderName){

char line[128], aux[128];
char* pStr;

	while(strcmp(line,"mesh") && !feof(md5file)){
		fgets(line,128,md5file);
		pStr = strtok(line," \"()\t\n/");
		if(pStr) sprintf(aux,"%s",pStr);
	}
	while(strcmp(aux,"{") && !feof(md5file)){
		pStr = strtok(NULL," \"()\t\n/");
		if(pStr) sprintf(aux,"%s",pStr);
	}

	fgets(line,128,md5file);
	pStr = strtok(line," \"()\t\n/");
	if(pStr) sprintf(aux,"%s",pStr);
	while(strcmp(aux,"shader")){
		fgets(line,128,md5file);
		pStr = strtok(NULL," \"()\t\n/");
		if(pStr) sprintf(aux,"%s",pStr);
	}
	pStr = strtok(NULL," \"()\t\n/");
	sprintf(shaderName,"%s",pStr);

	meshes->numVertices = getNumVerts(md5file);
	meshes->vertices = (struct md5vertex*)malloc(meshes->numVertices*sizeof(struct md5vertex));
	readVertices(md5file,meshes->vertices,meshes->numVertices);

	meshes->numTriangles = getNumTris(md5file);
	meshes->triangles = (struct md5triangle*)malloc(meshes->numTriangles*sizeof(struct md5triangle));
	readTriangles(md5file,meshes->triangles,meshes->numTriangles);

	meshes->numWeights = getNumWeights(md5file);
	meshes->weights = (struct md5weight*)malloc(meshes->numWeights*sizeof(struct md5weight));
	readWeights(md5file,meshes->weights,meshes->numWeights);

	while(strcmp(aux,"}") && !feof(md5file)){
		fgets(line,128,md5file);
		pStr = strtok(line," \"()\t\n/");
		if(pStr) sprintf(aux,"%s",pStr);
	}

}

void readJoints(FILE* md5file, struct md5joint* joints, unsigned int numJoints){

char line[128], aux[128];
char* pStr;
unsigned int i;
char jointName[128], parentName[128];
float p[3], o[3];
int parent;

	while(strcmp(line,"joints") && !feof(md5file)){
		fgets(line,128,md5file);
		pStr = strtok(line," \"()\t\n/");
		if(pStr) sprintf(aux,"%s",pStr);
	}
	while(strcmp(aux,"{") && !feof(md5file)){
		pStr = strtok(NULL," \"()\t\n/");
		if(pStr) sprintf(aux,"%s",pStr);
	}


	for(i=0;i<numJoints;i++){

		fgets(line,128,md5file);
		pStr = strtok(line," \"()\t\n/");
		if(pStr) sprintf(jointName,"%s",pStr);
		else{
			printf("ERROR: parsing md5mesh\n");
			exit(1);
		}

		pStr = strtok(NULL," \"()\t\n/");
		if(pStr) sscanf(pStr,"%d",&parent);
		else{
			printf("ERROR: parsing md5mesh\n");
			exit(1);
		}

		pStr = strtok(NULL," \"()\t\n/");
		if(pStr) sscanf(pStr,"%f",&p[0]);
		else{
			printf("ERROR: parsing md5mesh\n");
			exit(1);
		}
		pStr = strtok(NULL," \"()\t\n/");
		if(pStr) sscanf(pStr,"%f",&p[1]);
		else{
			printf("ERROR: parsing md5mesh\n");
			exit(1);
		}
		pStr = strtok(NULL," \"()\t\n/");
		if(pStr) sscanf(pStr,"%f",&p[2]);
		else{
			printf("ERROR: parsing md5mesh\n");
			exit(1);
		}


		pStr = strtok(NULL," \"()\t\n/");
		if(pStr) sscanf(pStr,"%f",&o[0]);
		else{
			printf("ERROR: parsing md5mesh\n");
			exit(1);
		}
		pStr = strtok(NULL," \"()\t\n/");
		if(pStr) sscanf(pStr,"%f",&o[1]);
		else{
			printf("ERROR: parsing md5mesh\n");
			exit(1);
		}
		pStr = strtok(NULL," \"()\t\n/");
		if(pStr) sscanf(pStr,"%f",&o[2]);
		else{
			printf("ERROR: parsing md5mesh\n");
			exit(1);
		}

		pStr = strtok(NULL," \"()\t\n/");
		if(pStr){
			sprintf(parentName,"%s",pStr);
		}else{
			sprintf(parentName,"root");
		}

		strcpy(joints[i].name,jointName);
		joints[i].id = i;
		strcpy(joints[i].parentName,parentName);
		joints[i].parentId = parent;

		memcpy(joints[i].position,p,3*sizeof(float));
		memcpy(joints[i].orientation,o,3*sizeof(float));

	}

	while(strcmp(aux,"}") && !feof(md5file)){
		fgets(line,128,md5file);
		pStr = strtok(line," \"()\t\n/");
		if(pStr) sprintf(aux,"%s",pStr);
	}

}

struct md5meshdata* getMD5MeshData(const char* filename){

	FILE* md5file;
	unsigned int i;
	char commandline[128];
	struct md5meshdata* md5data;

	md5file = fopen(filename,"r");
	if(!md5file){
		printf("Error opening file\n");
	}

	md5data = (struct md5meshdata*)malloc(sizeof(struct md5meshdata));

	md5data->version = getMD5Version(md5file);

	getCommandline(md5file,commandline);
	memcpy(md5data->commandline,commandline,128*sizeof(char));

	md5data->numJoints = getNumJoints(md5file);

	md5data->numMeshes = getNumMeshes(md5file);

	md5data->joints = (struct md5joint*)malloc(md5data->numJoints*sizeof(struct md5joint));
	readJoints(md5file,md5data->joints,md5data->numJoints);

	md5data->meshes = (struct md5mesh*)malloc(md5data->numMeshes*sizeof(struct md5mesh));
	for(i=0;i<md5data->numMeshes;i++){
		readMesh(md5file,&(md5data->meshes[i]),md5data->meshes[i].shaderName);
	}

	fclose(md5file);


	return md5data;
}

void freeMD5MeshData(struct md5meshdata* md5data){

unsigned int i;

	free(md5data->joints);

	for(i=0;i<md5data->numMeshes;i++){
		free(md5data->meshes[i].vertices);
		free(md5data->meshes[i].triangles);
		free(md5data->meshes[i].weights);
	}

	free(md5data->meshes);

	free(md5data);

}

