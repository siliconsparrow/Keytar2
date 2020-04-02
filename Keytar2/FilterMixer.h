/*
 * FilterMixer.h
 *
 *  Created on: 3 Apr 2020
 *      Author: adam
 */

#ifndef FILTERMIXER_H_
#define FILTERMIXER_H_

#include "AudioFilter.h"

class FilterMixer
{
public:
	FilterMixer(int nChannels);

	enum {
		kMaxLevel = 32767,
		kDefaultLevel = kMaxLevel
	};

	void setChannelSource(unsigned channel, AudioFilter *filter, unsigned level = kDefaultLevel);
	void setLevel(int channel, int level);

	virtual void fillFrame(Sample *frame);

private:
	class MixChannel
	{
	public:
		MixChannel();

		AudioFilter *_source;
		unsigned     _level;
	};

	MixChannel *_channel;
	unsigned    _nChannels;
};

#endif /* FILTERMIXER_H_ */
