/*
 * AudioFormat.cpp
 *
 *  Created on: 5 Apr 2020
 *      Author: adam
 */

#include "AudioFormat.h"

AudioFormat::AudioFormat()
	: _rate(0)
	, _bits(0)
	, _channels(0)
{
}

AudioFormat::AudioFormat(int rate, int bits, int channels)
	: _rate(rate)
	, _bits(bits)
	, _channels(channels)
{
}

void AudioFormat::convertTo(Sample *dest, const uint8_t *src, unsigned nSamples)
{
 // TODO: Convert sample format !!
}
