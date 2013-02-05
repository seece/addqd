#ifndef CONFIG_H
#define CONFIG_H

#define SYN_NUMCHANNELS 2
#define SYN_BUFFERSIZE 4096
#define SYN_RATE        44100

#define FLOAT_32BIT
#ifdef FLOAT_32BIT
	#define SAMPLE_TYPE float
#else
	#define SAMPLE_TYPE short
#endif

#endif