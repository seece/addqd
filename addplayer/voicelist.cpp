#include <cstring>
#include "player/voice.h"
#include "voicelist.h"

CVoiceList::CVoiceList() {
	this->clear();
}

CVoiceList::~CVoiceList() {

}

void CVoiceList::addVoice(Voice* voicep) {
	voices[voicep->index] = voicep;
}

void CVoiceList::removeVoice(Voice* voicep) {
	voicep->channel = NULL;
	voices[voicep->index] = NULL;
}

bool CVoiceList::containsVoice(Voice* voicep) {
	return voices[voicep->index] != NULL;
}

void CVoiceList::clear() {
	for (int i=0;i<SYN_MAX_VOICES;i++) {
		voices[i] = NULL;
	}
}