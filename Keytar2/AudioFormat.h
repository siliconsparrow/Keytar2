/*
 * AudioFormat.h
 *
 *  Created on: 5 Apr 2020
 *      Author: adam
 */

#ifndef AUDIOFORMAT_H_
#define AUDIOFORMAT_H_

#include "AudioFilter.h"

class AudioFormat
{
public:
	AudioFormat();
	AudioFormat(int rate, int bits, int channels);

	int getRate()     const { return _rate; }
	int getBits()     const { return _bits; }
	int getChannels() const { return _channels; }

	void convertTo(Sample *dest, const uint8_t *src, unsigned nSamples);

private:
	int _rate;
	int _bits;
	int _channels;
};

#endif /* AUDIOFORMAT_H_ */
