#ifndef WAV_H
#define WAV_H

#include "player/addsynth_types.h"

char * load16bitWAV(const char * path, int * length);
int convert_sample_to_float(Sample * sample, char * sampledata, int length);

#endif