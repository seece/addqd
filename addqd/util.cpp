#include <cmath>
#include <limits>
#include "util.h"

// from http://www.iquilezles.org/www/articles/functions/functions.htm
float impulse(float k, float x) {
	const float h = k*x;
	return h*expf(1.0f-h);
}

float dspClamp(float input) {
	if (input > 1.0f) {
		return 1.0f;
	} else if (input < -1.0f) {
		return -1.0f;
	}

	return input;
}


float fmin_func(float a, float b) {
	return a > b ? b : a;
}

float fmax_func(float a, float b) {
	return a < b ? b : a;
}

// if dist == 1.0, lerp = b
float lerp_func(float a, float b, float dist) {
	return b * dist + a * (1.0f-dist);
}

// from http://stackoverflow.com/questions/1024389/print-an-int-in-binary-representation-using-c
uint8_t *intToBin(int a, uint8_t *buffer, int buf_size) {
	int i;
    buffer += (buf_size - 1);

    for (i = 31; i >= 0; i--) {
        *buffer-- = (a & 1) + '0';

        a >>= 1;
    }

    return buffer;
}

uint8_t *intToBin2(int a, uint8_t *buffer, int buf_size) {
	int i;
    buffer += (buf_size - 1);

    for (i = buf_size - 1; i >= 0; i--) {
        *buffer-- = (a & 1) + '0';

        a >>= 1;
    }

    return buffer;
}

//writes three bytes to the output
uint8_t *charToHex(uint8_t a, uint8_t *output) {
	unsigned char modulo = a % 16;
	unsigned char tens = (a / 16);
	char characters[] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'};
	output[0] = characters[tens];
	output[1] = characters[modulo];
	output[2] = '\0';

	return output;
}

//writes three bytes to the output, replaces 00 with --
uint8_t *charToNiceHex(uint8_t a, uint8_t *output) {
	unsigned char modulo = a % 16;
	unsigned char tens = (a / 16);
	char characters[] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'};
	output[0] = characters[tens];
	output[1] = characters[modulo];
	output[2] = '\0';

	if ((output[0] == output[1])) {
			if (output[0] == '0') {
				output[0] = output[1] = '-';
			}
	}

	return output;
}

short swapBytes(short in) {
	short a,b;	//
	a = (in & 0xFF00) >> 8;
	b = (in & 0x00FF) << 8;
	return a + b;
}

uint64_t dumpArrayToDisk(char *data, int length, const char *output_path) {
    FILE *fp;
    fp = fopen(output_path, "wb");

	if (fp == NULL) {
		printf("Couldn't open file %s for writing!", output_path);
		fclose(fp);
		return 0;	// wrote 0 bytes
    }

	fwrite(data, sizeof(uint8_t), length,  fp);

	uint64_t written = (uint64_t) ftell(fp);
	fclose(fp);

	return written;
}

long getFilesize(FILE *fp) {
    fseek(fp, 0L, SEEK_END);
	long sz = ftell(fp);
	fseek(fp, 0L, SEEK_SET);

	return sz;
}

// converts a 16-bit sample to a 32-bit float representation
// a pointer to a sample struct, pointer to 16-bit integer sample data, length of data in bytes
int convert_sample_to_float(Sample * sample, char * sampledata, int length) {
	int samplecount = length/2;
	//int fullsize = samplecount * sizeof(float);
	sample->data = new float[samplecount];
	int samplenum = 0;

	for (int i=0;i<length;i+=2) {
		//int s = sampledata[i];
		char a = sampledata[i];
		char b = sampledata[i+1];
		//unsigned short * shortp = (unsigned short *) &sampledata[i];
		short x = (b & 0xFF) << 8 | (a & 0xFF);
		//short x = *shortp;
		float f = (float)x/((float)SHRT_MAX);
		samplenum++;
		sample->data[samplenum] = f;
	}

	sample->length = samplecount;
	return samplecount;
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