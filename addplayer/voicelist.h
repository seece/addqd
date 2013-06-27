#ifndef VOICELIST_H
#define VOICELIST_H

#include "player/voice.h"

class VoiceList {
	public:
	VoiceList();
	~VoiceList();
	void addVoice(Voice* voicep);
	void removeVoice(Voice* voicep);
	bool containsVoice(Voice* voicep);
	void clear();

	Voice* voices[SYN_MAX_VOICES];

	private:
};

#endif VOICELIST_H