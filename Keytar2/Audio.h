/*
 * Audio.h
 *
 *  Created on: 28 Mar 2020
 *      Author: adam
 */

#ifndef AUDIO_H_
#define AUDIO_H_

#include <stdint.h>

typedef int16_t SAMPLE;

#ifdef OLD
#define AUDIO_IN_PCM_BUFFER_SIZE                   4*2304 /* buffer size in half-word */
#define AUDIO_OUT_BUFFER_SIZE                      8192
#endif

class Audio
{
public:
	Audio();

	enum STATUS {
		kStatusOk,
		kStatusFail
	};

	STATUS init();
	STATUS start();
	STATUS stop();
	void process();
	unsigned getData(SAMPLE **data);

#ifdef OLD
	void record();
	void playback();

	unsigned getBuffer(int16_t **buf);
	void     fillBuffer(const int16_t *audioData, unsigned nSamples);
#endif // OLD

private:
};

#endif /* AUDIO_H_ */
