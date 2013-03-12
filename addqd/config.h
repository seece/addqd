#ifndef CONFIG_H
#define CONFIG_H

#define AUDIO_NUMCHANNELS (2)
#define AUDIO_RATE        (44100)
#define AUDIO_BUFFERSIZE  (2048)
#define AUDIO_BUFFERS (3)

#define FLOAT_32BIT
#ifdef FLOAT_32BIT
	#define SAMPLE_TYPE float
#else
	#define SAMPLE_TYPE short
#endif

//#define DEBUG_PRINT_SPEED
#define DEBUG_EVENT_SANITY_CHECKS
#define DEBUG_EVENT
#define DEBUG_INSTRUMENTS

#endif