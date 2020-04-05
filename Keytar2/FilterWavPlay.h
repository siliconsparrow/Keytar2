/*
 * FilterWavPlay.h
 *
 *  Created on: 3 Apr 2020
 *      Author: adam
 */

#ifndef FILTERWAVPLAY_H_
#define FILTERWAVPLAY_H_

#include "AudioFilter.h"
#include "FileSystem.h"

class FilterWavPlay
    : public AudioFilter
{
public:
	FilterWavPlay();
	virtual ~FilterWavPlay();

	bool load(const TCHAR *filename);
	void play();
	void stop();

	virtual void fillFrame(Sample *frame);

private:
	bool             _isPlaying;
	FileSystem::File _f;
	int              _channels;
	int              _bits;
	int              _rate;
};

#endif /* FILTERWAVPLAY_H_ */
