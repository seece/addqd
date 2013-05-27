#ifndef ENVELOPE_H
#define ENVELOPE_H

struct Envelope {
	float attack;	// attack time in seconds
	float decay;
	float hold;
	float sustain;
	float release;
};

#endif