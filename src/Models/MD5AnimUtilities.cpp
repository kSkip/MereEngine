#include "Models/MD5AnimUtilities.h"
#include "Models/MD5CommonUtilities.h"

unsigned int getNumFrames(FILE* md5file);

unsigned int getFrameRate(FILE* md5file);

unsigned int getNumAnimatedComponents(FILE* md5file);

void readHeirarchy(FILE* md5file, struct md5hierarchyjoint* heirarchyjoints, unsigned int numJoints);

void readBounds(FILE* md5file, struct md5bounds* bounds, unsigned int numFrames);

void readBaseframe(FILE* md5file, struct md5baseframejoint* baseframe, unsigned int numJoints);

void readFrames(FILE* md5file, struct md5frame* frames, unsigned int numFrames, unsigned int numAnimatedComponents);

unsigned int getNumFrames(FILE* md5file){

char line[128], aux[128];
char* pStr;
unsigned int numFrames;

	fseek(md5file,0,SEEK_SET);

	while(strcmp(aux,"numFrames") && !feof(md5file)){
		fgets(line,128,md5file);
		pStr = strtok(line," \"()\t\n/");
		if(pStr) sprintf(aux,"%s",pStr);
	}

	pStr = strtok(NULL," \"()\t\n/");
	if(pStr) sscanf(pStr,"%d",&numFrames);
	else{
			printf("ERROR: parsing md5anim\n");
			exit(1);
	}

	return numFrames;

}

unsigned int getFrameRate(FILE* md5file){

char line[128], aux[128];
char* pStr;
unsigned int frameRate;

	fseek(md5file,0,SEEK_SET);

	while(strcmp(aux,"frameRate") && !feof(md5file)){
		fgets(line,128,md5file);
		pStr = strtok(line," \"()\t\n/");
		if(pStr) sprintf(aux,"%s",pStr);
	}

	pStr = strtok(NULL," \"()\t\n/");
	if(pStr) sscanf(pStr,"%d",&frameRate);
	else{
			printf("ERROR: parsing md5anim\n");
			exit(1);
	}

	return frameRate;
}

unsigned int getNumAnimatedComponents(FILE* md5file){

char line[128], aux[128];
char* pStr;
unsigned int numAnimatedComponents;

	fseek(md5file,0,SEEK_SET);

	while(strcmp(aux,"numAnimatedComponents") && !feof(md5file)){
		fgets(line,128,md5file);
		pStr = strtok(line," \"()\t\n/");
		if(pStr) sprintf(aux,"%s",pStr);
	}

	pStr = strtok(NULL," \"()\t\n/");
	if(pStr) sscanf(pStr,"%d",&numAnimatedComponents);
	else{
			printf("ERROR: parsing md5anim\n");
			exit(1);
	}

	return numAnimatedComponents;

}

void readHeirarchy(FILE* md5file, struct md5hierarchyjoint* joints, unsigned int numJoints){

char line[128], aux[128];
char* pStr;
unsigned int i;
char jointName[128], parentName[128];
int parent, flags, startIndex;

	fseek(md5file,0,SEEK_SET);

	while(strcmp(line,"hierarchy") && !feof(md5file)){
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
			printf("ERROR: parsing md5anim\n");
			exit(1);
		}

		pStr = strtok(NULL," \"()\t\n/");
		if(pStr) sscanf(pStr,"%d",&parent);
		else{
			printf("ERROR: parsing md5anim\n");
			exit(1);
		}

		pStr = strtok(NULL," \"()\t\n/");
		if(pStr) sscanf(pStr,"%d",&flags);
		else{
			printf("ERROR: parsing md5anim\n");
			exit(1);
		}
		pStr = strtok(NULL," \"()\t\n/");
		if(pStr) sscanf(pStr,"%d",&startIndex);
		else{
			printf("ERROR: parsing md5anim\n");
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
		joints[i].parentId = parent;
		joints[i].flags = flags;
		joints[i].startIndex = startIndex;

	}

	while(strcmp(aux,"}") && !feof(md5file)){
		fgets(line,128,md5file);
		pStr = strtok(line," \"()\t\n/");
		if(pStr) sprintf(aux,"%s",pStr);
	}

}

void readBounds(FILE* md5file, struct md5bounds* bounds, unsigned int numFrames){

char line[128], aux[128];
char* pStr;
unsigned int i;
float min[3], max[3];

	fseek(md5file,0,SEEK_SET);

	while(strcmp(line,"bounds") && !feof(md5file)){
		fgets(line,128,md5file);
		pStr = strtok(line," \"()\t\n/");
		if(pStr) sprintf(aux,"%s",pStr);
	}
	while(strcmp(aux,"{") && !feof(md5file)){
		pStr = strtok(NULL," \"()\t\n/");
		if(pStr) sprintf(aux,"%s",pStr);
	}


	for(i=0;i<numFrames;i++){

		fgets(line,128,md5file);
		pStr = strtok(line," \"()\t\n/");
		if(pStr) sscanf(pStr,"%f",&min[0]);
		else{
			printf("ERROR: parsing md5anim\n");
			exit(1);
		}

		pStr = strtok(NULL," \"()\t\n/");
		if(pStr) sscanf(pStr,"%f",&min[1]);
		else{
			printf("ERROR: parsing md5anim\n");
			exit(1);
		}
		pStr = strtok(NULL," \"()\t\n/");
		if(pStr) sscanf(pStr,"%f",&min[2]);
		else{
			printf("ERROR: parsing md5anim\n");
			exit(1);
		}

		pStr = strtok(NULL," \"()\t\n/");
		if(pStr) sscanf(pStr,"%f",&max[0]);
		else{
			printf("ERROR: parsing md5anim\n");
			exit(1);
		}

		pStr = strtok(NULL," \"()\t\n/");
		if(pStr) sscanf(pStr,"%f",&max[1]);
		else{
			printf("ERROR: parsing md5anim\n");
			exit(1);
		}
		pStr = strtok(NULL," \"()\t\n/");
		if(pStr) sscanf(pStr,"%f",&max[2]);
		else{
			printf("ERROR: parsing md5anim\n");
			exit(1);
		}


		memcpy(bounds[i].min,min,3*sizeof(float));
		memcpy(bounds[i].max,max,3*sizeof(float));

	}

	while(strcmp(aux,"}") && !feof(md5file)){
		fgets(line,128,md5file);
		pStr = strtok(line," \"()\t\n/");
		if(pStr) sprintf(aux,"%s",pStr);
	}

}

void readBaseframe(FILE* md5file, struct md5baseframejoint* baseframe, unsigned int numJoints){

char line[128], aux[128];
char* pStr;
unsigned int i;
float pos[3], orient[3];

	fseek(md5file,0,SEEK_SET);

	while(strcmp(line,"baseframe") && !feof(md5file)){
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
		if(pStr) sscanf(pStr,"%f",&pos[0]);
		else{
			printf("ERROR: parsing md5anim\n");
			exit(1);
		}

		pStr = strtok(NULL," \"()\t\n/");
		if(pStr) sscanf(pStr,"%f",&pos[1]);
		else{
			printf("ERROR: parsing md5anim\n");
			exit(1);
		}
		pStr = strtok(NULL," \"()\t\n/");
		if(pStr) sscanf(pStr,"%f",&pos[2]);
		else{
			printf("ERROR: parsing md5anim\n");
			exit(1);
		}

		pStr = strtok(NULL," \"()\t\n/");
		if(pStr) sscanf(pStr,"%f",&orient[0]);
		else{
			printf("ERROR: parsing md5anim\n");
			exit(1);
		}

		pStr = strtok(NULL," \"()\t\n/");
		if(pStr) sscanf(pStr,"%f",&orient[1]);
		else{
			printf("ERROR: parsing md5anim\n");
			exit(1);
		}
		pStr = strtok(NULL," \"()\t\n/");
		if(pStr) sscanf(pStr,"%f",&orient[2]);
		else{
			printf("ERROR: parsing md5anim\n");
			exit(1);
		}


		memcpy(baseframe[i].position,pos,3*sizeof(float));
		memcpy(baseframe[i].orientation,orient,3*sizeof(float));


	}

	while(strcmp(aux,"}") && !feof(md5file)){
		fgets(line,128,md5file);
		pStr = strtok(line," \"()\t\n/");
		if(pStr) sprintf(aux,"%s",pStr);
	}
}

void readFrames(FILE* md5file, struct md5frame* frames, unsigned int numFrames, unsigned int numAnimatedComponents){

char line[128], aux[128];
char* pStr;
unsigned int i, j, frameIndex;
float value;

	for(i=0;i<numFrames;i++){

	while(strcmp(aux,"frame") && !feof(md5file)){
		fgets(line,128,md5file);
		pStr = strtok(line," \"()\t\n/");
		if(pStr) sprintf(aux,"%s",pStr);
	}
	pStr = strtok(NULL," \"()\t\n/");
	if(pStr){
		sprintf(aux,"%s",pStr);
		sscanf(aux,"%d",&frameIndex);
		if(frameIndex != i){
			printf("ERROR: Frame index is %d and i is %d\n",frameIndex,i);
			printf("ERROR: parsing md5anim\n");
			exit(1);
		}
		frames[i].index = frameIndex;
	}else{
		printf("ERROR: parsing md5anim\n");
		exit(1);
	}

	while(strcmp(aux,"{") && !feof(md5file)){
		pStr = strtok(NULL," \"()\t\n/");
		if(pStr) sprintf(aux,"%s",pStr);
	}

	frames[i].animatedComponents = (float*)malloc(numAnimatedComponents*sizeof(float));

	fgets(line,128,md5file);
	pStr = strtok(line," \"()\t\n/");
	for(j=0;j<numAnimatedComponents;j++){

		if(pStr) sscanf(pStr,"%f",&value);
		else{
			fgets(line,128,md5file);
			pStr = strtok(line," \"()\t\n/");
			if(pStr) sscanf(pStr,"%f",&value);
			else{
				printf("ERROR: parsing md5anim\n");
				exit(1);
			}
		}

		pStr = strtok(NULL," \"()\t\n/");

		frames[i].animatedComponents[j] = value;

	}

	while(strcmp(aux,"}") && !feof(md5file)){
		fgets(line,128,md5file);
		pStr = strtok(line," \"()\t\n/");
		if(pStr) sprintf(aux,"%s",pStr);
	}

	}

}

struct md5animdata* getMD5AnimData(const char* filename){

	FILE* md5file;
	char commandline[128];
	struct md5animdata* md5data;

	md5file = fopen(filename,"r");
	if(!md5file){
		printf("Error opening file\n");
		return NULL;
	}

	md5data = (struct md5animdata*)malloc(sizeof(struct md5animdata));

	md5data->version = getMD5Version(md5file);

	getCommandline(md5file,commandline);
	memcpy(md5data->commandline,commandline,128*sizeof(char));

	md5data->numFrames = getNumFrames(md5file);

	md5data->numJoints = getNumJoints(md5file);

	md5data->frameRate = getFrameRate(md5file);

	md5data->numAnimatedComponents = getNumAnimatedComponents(md5file);

	md5data->joints = (struct md5hierarchyjoint*)malloc(md5data->numJoints*sizeof(struct md5hierarchyjoint));
	readHeirarchy(md5file,md5data->joints,md5data->numJoints);

	md5data->bounds = (struct md5bounds*)malloc(md5data->numFrames*sizeof(struct md5bounds));
	readBounds(md5file,md5data->bounds,md5data->numFrames);

	md5data->baseframe = (struct md5baseframejoint*)malloc(md5data->numJoints*sizeof(struct md5baseframejoint));
	readBaseframe(md5file,md5data->baseframe,md5data->numJoints);

	md5data->frames = (struct md5frame*)malloc(md5data->numFrames*sizeof(struct md5frame));
	readFrames(md5file,md5data->frames,md5data->numFrames,md5data->numAnimatedComponents);

	fclose(md5file);

	return md5data;

}

void freeMD5AnimData(struct md5animdata* md5data){

	free(md5data->joints);

	free(md5data->bounds);

	free(md5data->baseframe);

	free(md5data->frames);

	free(md5data);

}
