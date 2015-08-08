#ifndef MD5COMMONUTILITIES_H
#define MD5COMMONUTILITIES_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef __cplusplus
extern "C" {
#endif

unsigned int getMD5Version(FILE* md5file);

void getCommandline(FILE* md5file, char* commandline);

unsigned int getNumJoints(FILE* md5file);

#ifdef __cplusplus
}
#endif

#endif
