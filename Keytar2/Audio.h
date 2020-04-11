/*
 * Audio.h
 *
 *  Created on: 28 Mar 2020
 *      Author: adam
 */

#ifndef AUDIO_H_
#define AUDIO_H_

#include "AudioFilter.h"

// Line-in filter. Pulls audio from a microphone or other external source.
class FilterLineIn : public AudioFilter
{
public:
	FilterLineIn() { }
	virtual ~FilterLineIn() { }

	virtual void fillFrame(Sample *frame);
};

// Audio class controls the Codec and drives the audio processing.
class Audio
{
public:
	static Audio *instance();

	enum STATUS {
		kStatusOk,
		kStatusFail
	};

	STATUS init();
	STATUS start();
	STATUS stop();

	void setFilterChain(AudioFilter *f);

	//	void process();
	//unsigned getData(SAMPLE **data);

	void pullBuffer(Sample *dest);

private:
	AudioFilter *_filterChain;

	Audio();

	int      initCodec(uint16_t InputDevice, uint16_t OutputDevice, uint32_t AudioFreq, uint32_t BitRes, uint32_t ChnlNbr);

	void     initClock();
	void     codecInit(uint16_t audiodevs);
	void     codecReset();
	uint32_t codecPlay();
	void     codecStop();
};

#endif /* AUDIO_H_ */
