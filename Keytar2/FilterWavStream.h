/*
 * FilterWavPlay.h
 *
 *  Created on: 3 Apr 2020
 *      Author: adam
 */

#ifndef FILTERWAVSTREAM_H_
#define FILTERWAVSTREAM_H_

#include "AudioFilter.h"
#include "WavFile.h"

class FilterWavStream
    : public AudioFilter
{
public:
	FilterWavStream();
	virtual ~FilterWavStream();

	bool load(const TCHAR *filename);
	void play();
	void stop();

	virtual void fillFrame(Sample *frame);

private:
	bool    _isPlaying;
	WavFile _f;
};

#endif /* FILTERWAVSTREAM_H_ */
