#include <cstdio>
#include <limits>
#include "player/addsynth_types.h"

// converts a 16-bit sample to a 32-bit float representation
// a pointer to a sample struct, pointer to 16-bit integer sample data, length of data in bytes
// returns the number of allocated bytes
int convert_sample_to_float(Sample * sample, char * sampledata, int length) {
	int samplecount = length/2;
	sample->data = new float[samplecount];
	int samplenum = 0;

	for (int i=0;i<length;i+=2) {
		char a = sampledata[i];
		char b = sampledata[i+1];
		short x = (b & 0xFF) << 8 | (a & 0xFF);
		float f = (float)x/((float)SHRT_MAX);
		sample->data[samplenum] = f;
		samplenum++;
	}

	sample->length = samplecount;
	return samplecount*sizeof(float);
}

char * load16bitWAV(const char * path, int * length) {
	size_t result;
	char * data=NULL;
	size_t size=0;
	size_t datasize = 0;

	FILE * fp;
	fp = fopen(path, "rb");

	if (fp == NULL) {
		fprintf(stderr, "Cannot open file %s!\n", path);
		return NULL;
	}

	fseek(fp, 0L, SEEK_END);
	size = ftell(fp);
	rewind(fp);


	fseek(fp, 40, SEEK_SET);
	fread(&datasize, 4, 1, fp);	// Subchunk2 Size

	data = new char[datasize];
	result = fread(data,1,datasize,fp);

	if (result == datasize) {
		printf("Read %d bytes from %s\n", result, path);
	} else {
		fprintf(stderr, "Read error with file %s.\n", path);
	}

	*length = datasize;

	return data;
}