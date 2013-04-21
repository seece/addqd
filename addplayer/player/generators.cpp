#include <stdlib.h>	// for rand function
#include <cmath>
#include <mmintrin.h>
#include <xmmintrin.h>
#include "../misc.h"
#include "generators.h"

// returns the sign of a number
// http://stackoverflow.com/a/4609795
template <typename T> int sgn(T val) {
    return (T(0) < val) - (val < T(0));
}

#define RAND_MAX_HALF (RAND_MAX/2)

// a linear congruential generator
int customrand(int seed)
{
  seed = (1103515245 * seed + 12345) % (INT_MAX);
  return seed;
}

// by Alexander Kritov
double generators::sawsin(double x)
{
	double t = fmod(x/(2*PI),(double)1.0);

	if (t>0.5) {
		return -sin(x);
	} else { 
		return (double)2.0*t-1.0;
	}
}

double generators::noise(double x)
{
   return (double)rand()/(double)RAND_MAX_HALF - 1.0;
}

double generators::simplenoise(double x)
{
   return (double)customrand((int)(x*1000.0))/(double)INT_MAX - 1.0;
}

double generators::triangle(double x)
{
	// period of 2 * PI
	double t = fmod(x/(2*PI), (double)1.0);
	if (t > 0.5) {
		return t*4.0-3.0;
	} else {
		return 1.0-(t*4.0);
	}
	
}

double generators::square(double x)
{
	if (x < PI) {
		return 1.0;
	} else {
		return -1.0;
	}
	
}

double generators::sinsquare(double x)
{
	float val = (float)sin(x) * 10.0f;
	_mm_store_ss( &val, _mm_min_ss( _mm_max_ss(_mm_set_ss(val),_mm_set_ss(-1.0f)), _mm_set_ss(1.0f) ) );
	return (double)val;
}

double generators::sampler(double x, float * samplearray, int arraysize) {
	int pos = (int)(x*44100.0);
	pos = pos % arraysize;
	return (double)samplearray[pos];
}

// length in samples
void generate_noise_sample(Sample * sample, int length) {
	sample->length = length;
	sample->data = new float[length];

	for (int i=0;i<length;i++) {
		sample->data[i] = (float)generators::noise((double)i);
	}
}