#ifndef UTIL_H
#define UTIL_H

#define NULL 0
#define MINUS_INFINITY INT_MIN

#define dprint(expr) printf(#expr " = %d\n", expr);
#define dprint1f(expr) printf(#expr " = %f\n", expr);
#define dprints(_fmt, expr) printf(#expr " = " #_fmt "\n", expr);
#define WAITKEY() printf("Press ENTER to continue.\n"); getchar();

#endif