#include <cstring>
#include <cstddef>
#include "config.h"
#include "channel.h"

Channel::Channel() {
	this->pan = 1.0f;
	this->volume = 1.0f;
	this->chain.numberOfEffects = 0;
	this->instrument = NULL;

	this->buffer = new SAMPLE_TYPE[SYN_MAX_BUFFER_SIZE*2];
	memset(this->buffer, 0, SYN_MAX_BUFFER_SIZE*2*sizeof(float));
}

Channel::~Channel() {
	delete this->buffer;
}