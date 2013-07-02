#ifndef GENERATORS_H
#define GENERATORS_H

#include "sample.h" 

// a pointer to a function that generates a waveform when given the phase p
typedef double (*WaveformFunc_t)(double p);
//typedef double (*FMFunc_1op_t)(double p, double a);
//typedef double (*FMFunc_2op_t)(double p, double a, double b);
typedef double (*OscFunc_2op_t)(double p, double a, double b);
typedef double (*SamplerFunc_t)(double time, float * samplearray, int arraysize);

// the oscillators assume a phase in the range of [0, 2*PI[
namespace generators {
	const int NUM_OSCILLATOR_TYPES = 3;
	/// Different oscillator types. Used in CToneBlock.
	enum osc_type {OSC_SINE = 0, OSC_SQUARE = 1, OSC_SAW = 2};
	/// Maps an oscillator type enum to a rendering function
	extern WaveformFunc_t osc_functions[NUM_OSCILLATOR_TYPES];

	double sine(double x);
	double sawsin(double x);
	double simplenoise(double x);
	double noise(double x);
	double triangle(double x);
	double square(double x);
	double sinsquare(double x);
	double sampler(double x, float * samplearray, int arraysize);
	double resonant_fm(double x, double a, double b);
};

void generate_noise_sample(Sample * sample, int length);

#endif