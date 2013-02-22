#ifndef UTIL_H
#define UTIL_H

#define NULL 0
#define MINUS_INFINITY INT_MIN

#define dprint(expr) printf(#expr " = %d\n", expr);
#define dprint1f(expr) printf(#expr " = %f\n", expr);
#define dprints(_fmt, expr) printf(#expr " = " #_fmt "\n", expr);
#define WAITKEY() printf("Press ENTER to continue.\n"); getchar();


// from http://www.iquilezles.org/www/articles/functions/functions.htm
float impulse(float k, float x);
float dspClamp(float input);
float fmin_func(float a, float b);
float fmax_func(float a, float b);
// if dist == 1.0, lerp = b
float lerp_func(float a, float b, float dist);
#define saturate(a) min(1.0, max(0.0, a))

#endif