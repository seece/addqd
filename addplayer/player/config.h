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
#define SYN_DEBUG_EVENT
#define DEBUG_CHANNEL_SANITY_CHECKS
#define DEBUG_AUDIO_SANITY_CHECKS
#define DEBUG_PLAYER_SANITY_CHECKS
#define DEBUG_MODULATION_SANITY_CHECKS
#define DEBUG_INSTRUMENTS
#define DEBUG_VOICE
//#define DEBUG_PLAYER
//#define DEBUG_PLAYER_VERBOSE
#define DEBUG_INSTRUMENT_SANITY_CHECKS

// IL Harmor has 516 partials
#define SYN_PARTIAL_AMOUNT 20
#define SYN_ENVELOPE_JITTER 441	// envelope processing delay in frames 
#define SYN_MAX_EFFECTS 10
#define SYN_MAX_PARAMETERS 10
#define SYN_MAX_INSTRUMENTS 16
#define SYN_PARTIAL_HIGH_CUT 17500.0
#define SYN_SINE_TABLE_SIZE 2205
#define SYN_MAX_VOICES 32
#define SYN_CHN_ENV_AMOUNT 2
#define SYN_CHN_LFO_AMOUNT 2
#define SYN_CHN_MOD_AMOUNT 8	// how many modulation matrix entries are allowed

#endif