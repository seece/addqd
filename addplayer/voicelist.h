// The active voices on each channel are added to this map. It is used 
// when the channel wants to know which voices are currently playing on 
// it. 

#ifndef VOICELIST_H
#define VOICELIST_H

#include "player/voice.h"

class CVoiceList {
	public:
	CVoiceList();
	~CVoiceList();
	void addVoice(Voice* voicep);
	void removeVoice(Voice* voicep);
	bool containsVoice(Voice* voicep);
	void clear();

	Voice* voices[SYN_MAX_VOICES];

	private:
};

#endif VOICELIST_H