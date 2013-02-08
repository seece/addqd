#ifndef CONFIG_H
#define CONFIG_H

#define AUDIO_NUMCHANNELS 2
#define AUDIO_RATE        44100
#define AUDIO_BUFFERSIZE 4096*2
#define AUDIO_BANKS 2

#define FLOAT_32BIT
#ifdef FLOAT_32BIT
	#define SAMPLE_TYPE float
#else
	#define SAMPLE_TYPE short
#endif

#endif