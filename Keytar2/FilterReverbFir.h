/*
 * FilterReverbFir.h
 *
 *  Created on: 11 Apr 2020
 *      Author: adam
 */

#ifndef FILTERREVERBFIR_H_
#define FILTERREVERBFIR_H_

#include "AudioFilter.h"

class FilterReverbFir
		: public AudioFilter
{
public:
	FilterReverbFir();
	virtual ~FilterReverbFir() { }

	void setSource(AudioFilter *source);

	virtual void fillFrame(Sample *frame);

private:
	enum {
		kBufSize = kSampleRate * kSampleChannels,
		kMaxTaps = 16
	};

	struct Tap
	{
		unsigned offset; // Offset of tap in samples.
		int      amp;    // Amplitude from 0 to 16383.
	};

	AudioFilter *_source;
	Sample       _buffer[kBufSize];
	int          _bufHead;
	int          _bufTail;
	unsigned     _nTaps;
	Tap          _tap[kMaxTaps];

	unsigned getOffset(unsigned offset) const;
	bool     addTap(unsigned offset, int amp);
};

#endif /* FILTERREVERBFIR_H_ */
