/*
 * Accompaniment.h
 *
 *  Created on: 9Apr.,2018
 *      Author: adam
 */

// Controls MIDI drums, bass, arpeggios etc.

#ifndef ACCOMPANIMENT_H_
#define ACCOMPANIMENT_H_

#include "MIDIFile.h"

class Accompaniment
{
public:
	Accompaniment();

	void poll(MIDISink *midiOut);

	bool load(const TCHAR *filename);

	bool isPlaying() const;

	void start(MIDISink *midiOut);
	void stop(MIDISink *midiOut);

#ifdef OLD
	void setDrumPattern(MIDIFile mid);
	void setTempo(unsigned bpm);
#endif // OLD

private:
	MIDIFile    _midiFile;
	AccompState _state;


#ifdef OLD
	unsigned _sequenceLength; // Length of drum pattern in ticks.


	void oneShotTrigger(unsigned timeNow);


#endif // OLD
};

#endif /* ACCOMPANIMENT_H_ */
