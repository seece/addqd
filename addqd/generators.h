#ifndef GENERATORS_H
#define GENERATORS_H

// a pointer to a function that generates a waveform when given the phase p
typedef double (*WaveformFunc_t)(double p);

typedef double (*SamplerFunc_t)(double time, float * samplearray, int arraysize);

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

#endif