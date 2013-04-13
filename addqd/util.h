#ifndef UTIL_H
#define UTIL_H

#include <stdio.h>
#include <stdint.h>
#include "addsynth.h"

#ifndef NULL
	#define NULL 0
#endif

#define MINUS_INFINITY INT_MIN

#ifndef PI
	#define PI 3.14159265358979323846
#endif

#define dprint(expr) printf(#expr " = %d\n", expr);
#define dprint1f(expr) printf(#expr " = %f\n", expr);
#define dprints(_fmt, expr) printf(#expr " = " #_fmt "\n", expr);
#define WAITKEY() printf("Press ENTER to continue.\n"); getchar();

#define MIN(a,b) (((a)<(b))?(a):(b))
#define MAX(a,b) (((a)>(b))?(a):(b))

// from http://www.iquilezles.org/www/articles/functions/functions.htm
float impulse(float k, float x);
float dspClamp(float input);
float fmin_func(float a, float b);
float fmax_func(float a, float b);
// if dist == 1.0, lerp = b
float lerp_func(float a, float b, float dist);
#define saturate(a) min(1.0, max(0.0, a))

char * load16bitWAV(const char * path, int * length);
uint64_t dumpArrayToDisk(char *data, int length, const char *output_path);
int convert_sample_to_float(Sample * sample, char * sampledata, int length);
long getFilesize(FILE *fp);

uint8_t *intToBin(int a, uint8_t *buffer, int buf_size);
uint8_t *intToBin2(int a, uint8_t *buffer, int buf_size);
// these write three bytes to output
uint8_t *charToHex(uint8_t a, uint8_t *output);
uint8_t *charToNiceHex(uint8_t a, uint8_t *output);
// AMIGAA
short swapBytes(short in);

#endif