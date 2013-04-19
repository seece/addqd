#ifndef GENERATORS_H
#define GENERATORS_H

#include "addsynth_types.h"

// the oscillators assume a phase in the range of [0, 2*PI[
namespace Generators {
	double sawsin(double x);
	double simplenoise(double x);
	double noise(double x);
	double triangle(double x);
	double square(double x);
	double sinsquare(double x);
	double sampler(double x, float * samplearray, int arraysize);
};

void generate_noise_sample(Sample * sample, int length);

#endif