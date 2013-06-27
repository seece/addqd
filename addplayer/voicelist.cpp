#include <cstring>
#include "player/voice.h"
#include "voicelist.h"

VoiceList::VoiceList() {
	this->clear();
}

VoiceList::~VoiceList() {

}

void VoiceList::addVoice(Voice* voicep) {
	voices[voicep->index] = voicep;
}

void VoiceList::removeVoice(Voice* voicep) {
	voices[voicep->index] = NULL;
}

bool VoiceList::containsVoice(Voice* voicep) {
	return voices[voicep->index] != NULL;
}

void VoiceList::clear() {
	for (int i=0;i<SYN_MAX_VOICES;i++) {
		voices[i] = NULL;
	}
}