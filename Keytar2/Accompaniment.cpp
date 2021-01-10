/*
 * Accompaniment.cpp
 *
 *  Created on: 9Apr.,2018
 *      Author: adam
 */

#include "Accompaniment.h"



Accompaniment::Accompaniment()
{
}

void Accompaniment::poll(MIDISink *midiOut)
{
	_midiFile.exec(midiOut, _state);
}

bool Accompaniment::load(const TCHAR *filename)
{
	return _midiFile.load(filename);
}

bool Accompaniment::isPlaying() const
{
	return _midiFile.isPlaying();
}

void Accompaniment::start(MIDISink *midiOut)
{
	_midiFile.rewind();
	_midiFile.start(midiOut);
}

void Accompaniment::stop(MIDISink *midiOut)
{
	_midiFile.stop(midiOut);
}

#ifdef OLD
/*

MID file
Start/stop button
Tempo knob
Fill button
2x variation buttons (possibly other variation signals)

One-shot timer, could be as fast as half a microsecond, could be as slow as 5 minutes.

Read MID file.
Put each track as a separate object
Work out tick times and program timebase (eg. 2.083ms per tick)

Convert to our timebase which is 240 ticks per quarter note.

Determine length of drum track for repeating.

Work out next event and set one-shot timer.

Send MIDI message

*/

void Accompaniment::setTempo(unsigned bpm) // Set from a knob or something.
{

}


void Accompaniment::setDrumPattern(MIDFile mid)
{

}

void Accompaniment::resetTracks()
{
	unsigned nextEvent = _sequenceLength;
	for(unsigned i = 0; i < _nTracks; i++)
	{
		MIDTrack &track = _tracks[i];
		track.rewind();
		unsigned nextTrackEvent = track.nextEventTime();
		if(nextTrackEvent < nextEvent)
			nextEvent = nextTrackEvent; // nextEvent should be member var? Don't set timer if in STOP mode.
	}
}

// TODO: How to handle fills?

// TODO: Set bassline and other accompaniments. These need to be transposable.

void Accompaniment::oneShotTrigger(unsigned timeNow)
{
	// Loop the track if we are at the end.
	if(timeNow == _sequenceLength)
	{
		resetTracks();
		timeNow = 0;
	}

	unsigned nextEvent = _sequenceLength;

	for(unsigned i = 0; i < _nTracks; i++)
	{
		MIDTrack &track = _tracks[i];

		unsigned nextTrackEvent = tracks.nextEventTime();

		if(nextTrackEvent == timeNow)
			track.triggerNextEvent();



		unsigned
		if(track.nextEvent() == timeNow)
			track.triggerEvent();

		if(track.nextEvent() < nextEvent)
			nextEvent = _track[i].nextEvent();
	}
}


unsigned Accompaniment::getTimeOfNextEvent()
{
	unsigned nextEvent = _sequenceLength;

	for(unsigned i = 0; i < _nTracks; i++)
	{
		MIDTrack &track = _tracks[i];

		if(track.nextEvent() == timeNow)
			track.triggerEvent();
		if(_track[i].nextEvent() < nextEvent;
			nextEvent = _track[i].nextEvent();
	}



	oneShotSetNext()

	// SOme way to deal with the end of track and repeating.
}

#endif // OLD
