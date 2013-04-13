#include <stdlib.h>	// for rand function
#include <cmath>
#include "util.h"
#include "generators.h"

#define RAND_MAX_HALF (RAND_MAX/2)

// a linear congruential generator
int customrand(int seed)
{
  seed = (1103515245 * seed + 12345) % (INT_MAX);
  return seed;
}

// by Alexander Kritov
double Generators::sawsin(double x)
{
	double t = fmod(x/(2*PI),(double)1.0);

	if (t>0.5) {
		return -sin(x);
	} else { 
		return (double)2.0*t-1.0;
	}
}

double Generators::noise(double x)
{
   return (double)rand()/(double)RAND_MAX_HALF - 1.0;
}

double Generators::simplenoise(double x)
{
   return (double)customrand((int)(x*1000.0))/(double)INT_MAX - 1.0;
}

double Generators::triangle(double x)
{
	// period of 2 * PI
	double t = fmod(x/(2*PI), (double)1.0);
	if (t < 0.5) {
		return t-1.0;
	} else {
		return 1.0-(t);
	}
	
}

double Generators::square(double x)
{
	if (x < PI) {
		return 1.0;
	} else {
		return 0.0;
	}
	
}