#include <cmath>
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


