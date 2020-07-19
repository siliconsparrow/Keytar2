/*
 * FilterSample.cpp
 *
 *  Created on: 5 Apr 2020
 *      Author: adam
 */

#include "FilterSample.h"
#include "WavFile.h"
#include "chipAlloc.h"
#include <malloc.h>
#include <string.h>

FilterSample::FilterSample()
	: _buf(0)
	, _bufCount(0)
	, _pos(0)
	, _isPlaying(false)
{
}

FilterSample::~FilterSample()
{
	free(_buf);
	//freeSDRam(_buf);
}

bool FilterSample::load(const TCHAR *wavFileName)
{
	WavFile wf;

	if(!wf.open(wavFileName)) {
		return false;
	}

	_bufCount = wf.getByteSize();
	//_buf = (StereoSample *)allocSDRam(_bufCount); //malloc(_bufCount); // Must be in SDRAM because internal RAM is not big enough.
	_buf = (StereoSample *)malloc(_bufCount);
	if(_buf == 0) {
		return false;
	}

	if(_bufCount != wf.readBlock((uint8_t *)_buf, _bufCount)) {
		return false;
	}

	_bufCount /= sizeof(StereoSample);
	return true;
}

void FilterSample::play()
{
	_pos = 0;
	_isPlaying = true;
}

void FilterSample::stop()
{
	_isPlaying = false;
}

void FilterSample::fillFrame(StereoSample *frame)
{
	if(_isPlaying) {
		if(_pos + kAudioFrameSize < _bufCount) {
			memcpy((uint8_t *)frame, (uint8_t *)&_buf[_pos], sizeof(StereoSample) * kAudioFrameSize);
			_pos += kAudioFrameSize;
		} else {
			unsigned nBytes = (_bufCount - _pos) * sizeof(StereoSample);
			memcpy((uint8_t *)frame, (uint8_t *)&_buf[_pos], nBytes);
			memset(&((uint8_t *)frame)[nBytes], 0, (sizeof(StereoSample) * kAudioFrameSize) - nBytes);
			_isPlaying = false;
		}
	} else {
		memset((uint8_t *)frame, 0, sizeof(StereoSample) * kAudioFrameSize);
	}
}
