/*
 * FilterMixer.cpp
 *
 *  Created on: 3 Apr 2020
 *      Author: adam
 */

#include "FilterMixer.h"
#include <string.h>

FilterMixer::MixChannel::MixChannel()
	: _source(0)
	, _level(kDefaultLevel)
{
}

FilterMixer::FilterMixer(int nChannels)
	: _nChannels(nChannels)
{
	_channel = new MixChannel[_nChannels];
}

void FilterMixer::setChannelSource(unsigned channel, AudioFilter *filter, unsigned level)
{
	_channel[channel]._source = filter;
	_channel[channel]._level = level;
}

void FilterMixer::setLevel(int channel, int level)
{
	_channel[channel]._level = level;
}

void FilterMixer::fillFrame(StereoSample *frame)
{
	// Clear the frame.
	memset(frame, 0, sizeof(StereoSample) * kAudioFrameSize);

	// Find out the total of all the mixer levels.
	int total = kMaxLevel * _nChannels;
//	for(unsigned i = 0; i < _nChannels; i++) {
//		total += (int)_channel[i]._level;
//	}

	// Mix each sample from each channel.
	StereoSample src[kAudioFrameSize]; // Working buffer.
	for(unsigned chan = 0; chan < _nChannels; chan++) {
		_channel[chan]._source->fillFrame(src);
		int level = _channel[chan]._level;
		for(unsigned i = 0; i < kAudioFrameSize; i++) {
			frame[i].l += (src[i].l * level) / total;
			frame[i].r += (src[i].r * level) / total;
		}
	}
}
