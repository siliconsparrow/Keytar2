/*
 * FilterWavPlay.cpp
 *
 *  Created on: 3 Apr 2020
 *      Author: adam
 */

#include "FilterWavPlay.h"
#include <string.h>

FilterWavPlay::FilterWavPlay(const TCHAR *filename)
{
}

FilterWavPlay::~FilterWavPlay()
{
}

void FilterWavPlay::fillFrame(Sample *frame)
{
	if(_isPlaying) {

	} else {
		// Not currently playing, fill the buffer with silence.
		memset(frame, 0, sizeof(Sample) * kAudioFrameSize);
	}
}
