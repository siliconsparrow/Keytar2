/*
 * FilterSample.h
 *
 *  Created on: 5 Apr 2020
 *      Author: adam
 */

#ifndef FILTERSAMPLE_H_
#define FILTERSAMPLE_H_

#include "AudioFilter.h"
#include "FileSystem.h"

class FilterSample
	: public AudioFilter
{
public:
	FilterSample();
	virtual ~FilterSample();

	bool load(const TCHAR *wavFileName);

	void play();
	void stop();

	virtual void fillFrame(Sample *frame);

private:
	Sample   *_buf;
	unsigned  _bufCount;
	unsigned  _pos;
	bool      _isPlaying;
};

#endif /* FILTERSAMPLE_H_ */
