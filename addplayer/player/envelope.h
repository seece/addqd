#ifndef ENVELOPE_H
#define ENVELOPE_H

struct EnvState {
	bool hold;				// if a key is held down
	long beginTime;			// the moment in samples when the key was repressed
	long endTime;	
	bool released;
	double volume;			// volume set by note-on command
	double target_volume;	// if target_volume differs from volume, volume will be interpolated smoothly to it
};


struct Envelope {
	float attack;	// attack time in seconds
	float decay;
	float hold;
	float sustain;
	float release;
};

void init_envelope(Envelope* env);
EnvState init_envstate(long sampleTime);
#endif