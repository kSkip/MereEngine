#include "Models/MD5CommonUtilities.h"

unsigned int getMD5Version(FILE* md5file){

char line[128], aux[128];
unsigned int version;	

	while(strcmp(aux,"MD5Version") && !feof(md5file)){
		fgets(line,128,md5file);
		sscanf(line,"%s%d",aux,&version);
	}

	return version;

}

void getCommandline(FILE* md5file,char* commandline){

char line[128], aux[128];

	while(strcmp(aux,"commandline") && !feof(md5file)){
		fgets(line,128,md5file);
		sscanf(line,"%s %[^\n]s",aux,commandline);
	}

}

unsigned int getNumJoints(FILE* md5file){

char line[128], aux[128];
char* pStr;
unsigned int numJoints;

	while(strcmp(aux,"numJoints") && !feof(md5file)){
		fgets(line,128,md5file);
		pStr = strtok(line," \"()\t\n/");
		if(pStr) sprintf(aux,"%s",pStr);
	}
	pStr = strtok(NULL," \"()\t\n/");
	if(pStr) sscanf(pStr,"%d",&numJoints);
	else{
			printf("ERROR: parsing md5mesh\n");
			exit(1);
	}

	return numJoints;

}
