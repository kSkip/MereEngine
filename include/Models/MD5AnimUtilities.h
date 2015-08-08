#ifndef MD5ANIMUTILITIES_H
#define MD5ANIMUTILITIES_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct md5hierarchyjoint{
	char name[128];
	unsigned int id;
	int parentId;
	int flags;
	int startIndex;
};

struct md5bounds{
	float min[3];
	float max[3];
};

struct md5baseframejoint{
	float position[3];
	float orientation[3];
};

struct md5frame{
	unsigned int index;
	float* animatedComponents;
};

struct md5animdata{
	unsigned int version;
	char commandline[128];
	unsigned int numFrames;
	unsigned int numJoints;
	unsigned int frameRate;
	unsigned int numAnimatedComponents;
	struct md5hierarchyjoint* joints;
	struct md5bounds* bounds;
	struct md5baseframejoint* baseframe;
	struct md5frame* frames;
};

#ifdef __cplusplus
extern "C" {
#endif

struct md5animdata* getMD5AnimData(const char* filename);

void freeMD5AnimData(struct md5animdata* md5data);

#ifdef __cplusplus
}
#endif

#endif
