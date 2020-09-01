// *****************************************************************************
// **
// ** Load, store and replay a single MIDI track.
// **
// **   by Adam Pierce <adam@siliconsparrow.com>
// **   created 1-Apr-2018
// **
// *****************************************************************************

//#include "MIDIFile.h"
//#include "MIDI.h"
//#include "File.h"
//#include <string.h>
//#include <malloc.h>

#ifdef OLD

// 4CC code used to identify a MIDI track within a MID file.
#define MID_TRACK_MAGIC ((const uint8_t *)"MTrk")

// Structure of the MIDI track header.
struct __attribute__((packed)) MIDTrk
{
	char     magic[4];
	uint32_t length;
};

// Initialize a new, blank MIDI track.
MIDITrack::MIDITrack()
	: _event(0)
	, _numEvents(0)
	, _trackLength(0)
	, _currentEvent(0)
	, _currentTime(0)
	, _channelMap(0)
{
}

MIDITrack::~MIDITrack()
{
	delete[] _event;
}

// Load a MIDI track from the file.
bool MIDITrack::load(File &f, MIDIFile &m)
{
	MIDTrk hdrTrack;

	if(!f.read((uint8_t *)&hdrTrack, sizeof(hdrTrack)))
		return false; // Failed to read track header.

	if(0 != memcmp(hdrTrack.magic, MID_TRACK_MAGIC, sizeof(hdrTrack.magic)))
		return false; // Track header invalid.

	// Delete any old data.
	delete[] _event;
	_event = 0;
	_numEvents = 0;

	// This is the track length in bytes.
	uint32_t trackLength = ntohl(hdrTrack.length);

	// Allocate RAM to store MIDI events. Err on the side of allocating too much.
	//unsigned maxEvents = trackLength / 2;
	//_event = new MIDIMessage[maxEvents];

	// Read the MIDI events.
	uint32_t byteCount = 0;
	unsigned totalTime = 0;
	while(byteCount < trackLength)
	{
		MIDIMessage msg;
		int r = msg.load(f, m);
		if(r <= 0)
			return false; // File read failure.

		totalTime += msg.getDeltaTime();

		byteCount += r;

		if(msg.dataLength() > 0)
		{
			MIDIMessage *newArray = (MIDIMessage *)realloc(_event, sizeof(MIDIMessage) * (_numEvents + 1));
			if(newArray == 0)
				return false; // Out of memory.

			_event = newArray;
			_event[_numEvents++] = msg;


			_channelMap |= (1 << msg.getChannel());
		}
/*
		MIDIMessage &msg = _event[_numEvents];

		int r = msg.load(f, m);
		if(r <= 0)
			return false; // File read failure.

		totalTime += msg.getDeltaTime();

		byteCount += r;

		// Meta events will come back with a data length of zero but we don't
		// want to store them in the playback stream.
		if(_numEvents < maxEvents && msg.dataLength() > 0)
		{
			_channelMap |= (1 << msg.getChannel());
			_numEvents++;
		}
		*/
	}

	// Free any unused space.
	unsigned trackSize = sizeof(MIDIMessage) * _numEvents;
	_event = (MIDIMessage *)realloc(_event, trackSize);

	// Work out the total length of the track to the nearest bar.
	unsigned ticksPerBar = m.getTicksPerBar();
	unsigned bars = totalTime / ticksPerBar;
	if(0 != totalTime % ticksPerBar)
		bars++;

	_trackLength = bars * ticksPerBar;

	// Match up note-on and note-off messages to make life easy for the accompaniment system.
	linkNoteMessages();

	// Find initial program and control messages.
	findSetupMessages();

	// Done.
	rewind();
	return true;
}

// Links up note-on and corresponding note-off messages. This is needed
// to help with the transposition in the accompaniment system.
void MIDITrack::linkNoteMessages()
{
	for(unsigned i = 0; i < _numEvents; i++)
	{
		MIDIMessage &msg = _event[i];
		if(msg.msgType() == MIDIMessage::NOTE_ON && msg.data()[2] > 0)
		{
			uint8_t channel = msg.getChannel();
			uint8_t note = msg.data()[1];
			for(unsigned j = i + 1; j < _numEvents; j++)
			{
				MIDIMessage &m = _event[j];
				if(m.msgType() == MIDIMessage::NOTE_OFF
				   || (m.msgType() == MIDIMessage::NOTE_ON && m.data()[2] == 0))
				{
					if(m.getChannel() == channel && m.data()[1] == note)
					{
						msg.linkEvent(&m);
						break;
					}
				}
			}
		}
	}
}

// Scale the timestamps in this track to suit a different PPQ.
void MIDITrack::changeTimebase(unsigned mul, unsigned div)
{
	unsigned tOld = 0;
	unsigned tNew = 0;

	// Adjust timing of all MIDI events.
	for(unsigned i = 0; i < _numEvents; i++)
	{
		MIDIMessage &evt = _event[i];

		unsigned tNewBeforeEvent = tNew;
		tOld += evt.getDeltaTime();
		tNew = (tOld * mul) / div;
		unsigned newDelta = tNew - tNewBeforeEvent;
		evt.setDeltaTime(newDelta);
	}

	// Adjust track length.
	_trackLength *= mul;
	_trackLength /= div;
}

// Find all events that need to be actioned at the current time.
// If midiOut is NULL, it will still advance counters and whatever
// but will not sound any MIDI events.
// Returns true if the track has looped.
// TODO: There's a possibility that this could miss some events near the end if the timer
//       loops around before exec() is called. Maybe this class needs to have it's own
//       timing and not use the MIDITimer class or maybe there needs to be a "hasLooped" flag.
void MIDITrack::exec(MIDI *midiOut, AccompState &accomp, unsigned currentTime)
{
	// In mode-1 MIDI files, track zero often has no playable events.
	if(_numEvents == 0)
		return;

	// If the timer has looped, play any remaining events to the end of the track.
	if(currentTime < _currentTime)
	{
		while(_currentEvent < _numEvents)
		{
			if(midiOut != 0)
				midiOut->sendTransposed(_event[_currentEvent], accomp);

			_currentEvent++;
		}

		_currentEvent = 0;
		_currentTime = 0;
	}

	// Find and play all events that should be played at the current time.
	while(_currentEvent < _numEvents)
	{
		MIDIMessage &msg = _event[_currentEvent];
		unsigned t = _currentTime + msg.getDeltaTime();

		if(t > currentTime)
			break;

		if(midiOut != 0)
			midiOut->sendTransposed(msg, accomp);

		_currentEvent++;
		_currentTime = t;
	}
}

// Issue note-off events for all channels used by this track.
void MIDITrack::stop(MIDI *midiOut)
{
	for(unsigned channel = 0; channel < 16; channel++)
	{
		if(0 != ((1 << channel) & _channelMap))
			midiOut->allNotesOff(channel & 15);
	}
}

void MIDITrack::rewind()
{
	_currentEvent = 0;
	_currentTime = 0;
}


// Scan the track and find the first program change and control change
// message for each channel.
void MIDITrack::findSetupMessages()
{
	_setupMsg.clear();

	for(unsigned i = 0; i < _numEvents; i++)
	{
		MIDIMessage &msg = _event[i];
		if(msg.msgType() == MIDIMessage::PROGRAM_CHANGE)
		{
			if(!_setupMsg.findPgmChange(msg.getChannel()))
				_setupMsg.add(msg);
		}
		else if(msg.msgType() == MIDIMessage::CONTROL_CHANGE)
		{
			if(!_setupMsg.findControlChange(msg.getChannel(), msg.data()[1]))
				_setupMsg.add(msg);
		}
	}
}

// Send note-off commands for all channes used by this track.
void MIDITrack::allNotesOff(MIDI *midiOut)
{
	for(unsigned i = 0; i < MIDI_NUM_CHANNELS; i++)
	{
		if(0 != (_channelMap & (1 << i)))
			midiOut->allNotesOff(i);
	}
}

// Load the program and control channel settings for this track.
void MIDITrack::setupForPlayback(MIDI *midiOut)
{
	allNotesOff(midiOut);
	_setupMsg.replay(midiOut);
}

// Seek to the given position.
void MIDITrack::jumpTo(unsigned t)
{
	t = t % _trackLength; // In case t is outside the bounds.

	for(_currentEvent = 0, _currentTime = 0; _currentTime < t && _currentEvent < _numEvents; _currentEvent++)
	{
		unsigned tNext = _currentTime + _event[_currentEvent].getDeltaTime();
		if(tNext >= t)
			break;

		_currentTime = tNext;
	}
}

#endif // OLD
