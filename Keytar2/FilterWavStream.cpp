/*
 * FilterWavPlay.cpp
 *
 *  Created on: 3 Apr 2020
 *      Author: adam
 */

#include <string.h>
#include <stdio.h>
#include "FilterWavStream.h"

FilterWavStream::FilterWavStream()
	: _isPlaying(false)
{
}

FilterWavStream::~FilterWavStream()
{
}

bool FilterWavStream::load(const TCHAR *filename)
{
	return _f.open(filename);
}

void FilterWavStream::play()
{
	if(_f.rewind()) {
		_isPlaying = true;
	} else {
		printf("SEEK ERROR.\n");
	}
}

void FilterWavStream::stop()
{
	_isPlaying = false;
}

void FilterWavStream::fillFrame(Sample *frame)
{
	if(_isPlaying) {
		int nBytes = sizeof(Sample) * kAudioFrameSize;
		uint8_t *dest = (uint8_t *)frame;
		int cb = _f.readBlock(dest, nBytes);
		if(cb < 0) {
			_isPlaying = false;
			_f.rewind();
		}
		if(cb < nBytes) {
			// end of file.
			//memset(&dest[cb], 0, nBytes - cb);
			_isPlaying = false;
			_f.rewind();
		}
	} else {
		// Not currently playing, fill the buffer with silence.
		memset(frame, 0, sizeof(Sample) * kAudioFrameSize);
	}
}
