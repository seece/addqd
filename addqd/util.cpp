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

