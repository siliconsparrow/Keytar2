/*
 * FilterVocoder.cpp
 *
 *  Created on: 8 Apr 2020
 *      Author: adam
 */

#include "FilterVocoder.h"

FilterVocoder::FilterVocoder()
	: _src(0)
{
}

void FilterVocoder::fillFrame(Sample *frame)
{
	Sample buf[kAudioFrameSize];

	_src->fillFrame(buf);

	int j = 0;
	for(int i = 0; i < kAudioFrameSize / 2; i++) {
		frame[j++] = buf[i];
		frame[j++] = buf[i];
	}
}
