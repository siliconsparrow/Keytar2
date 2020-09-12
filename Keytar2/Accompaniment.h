/*
 * Accompaniment.h
 *
 *  Created on: 9Apr.,2018
 *      Author: adam
 */

// Controls MIDI drums, bass, arpeggios etc.

#ifndef ACCOMPANIMENT_H_
#define ACCOMPANIMENT_H_

#ifdef OLD
#include "MIDIMessage.h"
#include <stdint.h>

#define DEFAULT_ACCOMP_CHANNEL_MASK ((uint16_t)0xFBFE) // Default accompaniment channels are all except for 10 (drums) and 1 (keyboard).
#endif // OLD

class AccompState
{
#ifdef OLD
public:
	AccompState();

	void stopAccomp();
	void setChannelMask(uint16_t mask);
	void setTranspose(int transpose, bool minor);

	bool filterMessage(uint8_t *result, MIDIMessage &msg);

private:
	int      _transpose;
	bool     _minor;
	uint16_t _channelMask;
#endif // OLD
};

#ifdef OLD
#if 0

#include "MIDIFile.h"

class Accompaniment
{
public:
	Accompaniment();

	void setDrumPattern(MIDIFile mid);
	void setTempo(unsigned bpm);
	void start();
	void stop();

private:
	unsigned _sequenceLength; // Length of drum pattern in ticks.


	void oneShotTrigger(unsigned timeNow);


};
#endif // 0

#endif // OLD

#endif /* ACCOMPANIMENT_H_ */
