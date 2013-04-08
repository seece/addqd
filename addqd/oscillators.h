#ifndef OSCILLATORS_H
#define OSCILLATORS_H

// a pointer to a function that generates a waveform when given the phase p
typedef double (*WaveformFunc_t)(double p);

// the oscillators assume a phase in the range of [0, 2*PI[
namespace Oscillators {
	double sawsin(double x);
	double noise(double x);
	double triangle(double x);
	double square(double x);
};

#endif