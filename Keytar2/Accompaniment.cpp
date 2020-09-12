/*
 * Accompaniment.cpp
 *
 *  Created on: 9Apr.,2018
 *      Author: adam
 */

#ifdef OLD

#include "Accompaniment.h"

#define NOACCOMP 0x10000

AccompState::AccompState()
	: _transpose(NOACCOMP)
	, _minor(false)
	, _channelMask(DEFAULT_ACCOMP_CHANNEL_MASK)
{
}

void AccompState::stopAccomp()
{
	_transpose = NOACCOMP;
}

void AccompState::setTranspose(int transpose, bool minor)
{
	_transpose = transpose;
	_minor = minor;
}

void AccompState::setChannelMask(uint16_t mask)
{
	_channelMask = mask;
}

// Return false if the message is not to be played.
bool AccompState::filterMessage(uint8_t *result, MIDIMessage &msg)
{
	if(msg.msgType() == MIDIMessage::NOTE_ON || msg.msgType() == MIDIMessage::NOTE_OFF)
	{
		if(0 != (_channelMask & (1 << msg.getChannel())))
		{
			if(_transpose == NOACCOMP)
				return false; // Do not play accompaniment notes when no scale has been selected.

			// Make a transposed copy of the MIDI message.
			msg.dataTransposed(result, _transpose, _minor);
			return true;
		}
	}

	// Non-note messages and non-accomp channel messages do not get transposed.
	// Just make a straight-up copy of those messages.
	msg.dataCopy(result);
	return true;

/*
	if(msg.msgType() == MIDIMessage::NOTE_ON)
	{
		if(msg.getChannel() != DRUMTRACK_CHANNEL && _transpose == NOACCOMP)
			return false; // Do not play accompaniment notes when no scale has been selected.
	}

	msg.dataTransposed(result, _transpose, _minor);
	return true;
*/
}



#if 0


Accompaniment::Accompaniment()
{
	// TODO Auto-generated constructor stub

}

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

void Accompaniment::start()
{

}

void Accompaniment::stop()
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

#endif

#endif // OLD
