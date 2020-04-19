/*
 * FilterReverbFir.cpp
 *
 *  Created on: 11 Apr 2020
 *      Author: adam
 */

#include "FilterReverbFir.h"
#include <string.h>

FilterReverbFir::FilterReverbFir()
	: _source(0)
	, _bufHead(0)
	, _bufTail(70000)
	, _nTaps(0)
{
	memset(_buffer, 0, sizeof(_buffer));
	addTap(8000, 4096);
	addTap(11025, 4096);
	addTap(22050, 2048);
	addTap(32000, 1024);
}

void FilterReverbFir::setSource(AudioFilter *source)
{
	_source = source;
}

bool FilterReverbFir::addTap(unsigned offset, int amp)
{
	if(_nTaps >= kMaxTaps)
		return false;

	_tap[_nTaps].amp = amp;
	_tap[_nTaps].offset = offset;
	return true;
}

unsigned FilterReverbFir::getOffset(unsigned offset) const
{
	if(offset > _bufHead)
		offset += kBufSize;

	return _bufHead - offset;
}

void FilterReverbFir::fillFrame(Sample *frame)
{
	// Get a frame from our source.
	Sample src[kAudioFrameSize];
	_source->fillFrame(src);

	memcpy(&_buffer[_bufHead], src, sizeof(src));
	_bufHead += kAudioFrameSize;
	if(_bufHead >= 80000)
		_bufHead = 0;
	_bufTail += kAudioFrameSize;
	if(_bufTail >= 80000)
		_bufTail = 0;

	for(unsigned i = 0; i < kAudioFrameSize; i++) {
		frame[i] = ((16000 * (int)src[i]) + (16000 * (int)_buffer[_bufTail + i])) / 32768;
	}

	/*
	// Get a frame from our source.
	Sample src[kAudioFrameSize];
	_source->fillFrame(src);

	// Create echoes into the buffer using taps.
	int decay = 1; // 32768 / kBufSize;
	unsigned offset = 0;
	for(unsigned tap = 0; tap < _nTaps; tap++) {
		unsigned offset = getOffset(_tap[tap].offset);
		int amp = ((32768 - decay) * _tap[tap].amp) / 32768;
		for(unsigned i = 0; i < kAudioFrameSize; i++) {
			Sample *b = &_buffer[offset++];
			if(offset >= kBufSize)
				offset = 0;
			*b = ((amp * (int)*b) + (32768 * (int)src[i])) / 32768;
		}
	}

	// Mix the original sound with the echos.
	int wet = 16384;
	int dry = 32768 - wet;
	for(unsigned i = 0; i < kAudioFrameSize; i++) {
		frame[i] = ((dry * (int)src[i]) + (wet * (int)_buffer[_bufHead++])) / 32768;
		if(_bufHead >= kBufSize)
			_bufHead = 0;
	}
	*/
}