#ifndef UTIL_H
#define UTIL_H

#define dprint(expr) printf(#expr " = %d\n", expr);
#define dprint1f(expr) printf(#expr " = %f\n", expr);
#define dprints(_fmt, expr) printf(#expr " = " #_fmt "\n", expr);
#define WAITKEY() printf("Press ENTER to continue.\n"); getchar();

#endif