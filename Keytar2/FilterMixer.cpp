/*
 * FilterMixer.cpp
 *
 *  Created on: 3 Apr 2020
 *      Author: adam
 */

#include "FilterMixer.h"

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
	_channel[channel]._source = channel;
	_channel[channel]._level = level;
}

void FilterMixer::setLevel(int channel, int level)
{
	_channel[channel]._level = level;
}

void FilterMixer::fillFrame(Sample *frame)
{
	// Clear the frame.
	memset(frame, 0, sizeof(Sample) * kAudioFrameSize);

	// Find out the total of all the mixer levels.
	int total = 0;
	for(unsigned i = 0; i < _nChannels; i++) {
		total += (int)_channel[i]._level;
	}

	// Mix each sample from each channel.
	Sample src[kAudioFrameSize]; // Working buffer.
	for(unsigned chan = 0; chan < _nChannels; chan++) {
		_channel[chan]._source->fillFrame(src);
		int level = _channel[chan]._level;
		for(unsigned i = 0; i < kAudioFrameSize; i++) {
			frame[i] += (src[i] * level) / total;
		}
	}
}
