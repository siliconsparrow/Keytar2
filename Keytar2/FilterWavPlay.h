/*
 * FilterWavPlay.h
 *
 *  Created on: 3 Apr 2020
 *      Author: adam
 */

#ifndef FILTERWAVPLAY_H_
#define FILTERWAVPLAY_H_

#include "AudioFilter.h"
#include "ff.h"

class FilterWavPlay
    : public AudioFilter
{
public:
	FilterWavPlay(const TCHAR *filename);
	virtual ~FilterWavPlay();

	virtual void fillFrame(Sample *frame);

private:
};

#endif /* FILTERWAVPLAY_H_ */
