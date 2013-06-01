#ifndef EKS_MATH_H
#define EKS_MATH_H

#define NOTEMAGIC 1.059460646483
//#define PI 3.14159265
#define TAU (2*PI)
#define NOTEFREQ(pitch) (pow(NOTEMAGIC, pitch) * 440.0)

#ifndef PI
	#define PI 3.14159265358979323846
#endif

#define saturate(a) min(1.0, max(0.0, a))

#endif