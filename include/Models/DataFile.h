#ifndef DATAFILE_H
#define DATAFILE_H

#include <stdio.h>
#include <string.h>

#define MAX_LINE_LENGTH 128

class DataFile {
protected:
	DataFile(const char* fileName) {
		fp = fopen(fileName, "r");
		memset(line, 0, sizeof line);
	}
	~DataFile() {
		fclose(fp);
	}
	DataFile(DataFile const &) = delete;
	DataFile& operator= (DataFile const &) = delete;

	char* DataFile::getLineOrNull()
	{
		return fgets(line, MAX_LINE_LENGTH, fp);
	}

	char* DataFile::getLine()
	{
		char *l = getLineOrNull();
		if (l == NULL || feof(fp)) {
			throw;
		}
		return l;
	}

private:
	FILE *fp;
	char line[MAX_LINE_LENGTH];
};

#endif