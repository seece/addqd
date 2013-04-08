#include <stdlib.h>	// for rand function
#include <cmath>
#include "util.h"
#include "oscillators.h"

#define RAND_MAX_HALF (RAND_MAX/2)

// by Alexander Kritov
double Oscillators::sawsin(double x)
{
   double t = fmod(x/(2*PI),(double)1.0);
   if (t>0.5)
	   return -sin(x);
   if (t<=0.5)
	   return (double)2.0*t-1.0;
}

double Oscillators::noise(double x)
{
   return (double)rand()/(double)RAND_MAX_HALF - 1.0;
}


double Oscillators::triangle(double x)
{
	// period of 2 * PI
	if (x < PI) {
		return x-1.0;
	} else {
		return 1.0-(x-PI);
	}
	
}

double Oscillators::square(double x)
{
	if (x < PI) {
		return 1.0;
	} else {
		return 0.0;
	}
	
}