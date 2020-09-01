// *****************************************************************************
// **
// ** Load, store and replay a single MIDI track.
// **
// **   by Adam Pierce <adam@siliconsparrow.com>
// **   created 1-Apr-2018
// **
// *****************************************************************************

#ifndef MIDITRACK_H_
#define MIDITRACK_H_

//#include "MIDITimer.h"
#include "MIDIMessage.h"
//#include "Accompaniment.h"
//#include "MIDIMessageArray.h"

#ifdef OLD

class File;
class MIDI;
class MIDIFile;
class MIDITrack
{
public:
	MIDITrack();
	~MIDITrack();

	bool         load(File &f, MIDIFile &m);
	unsigned     getCurrentPosition() const { return _currentTime; }
	unsigned     getTotalLength()     const { return _trackLength; }
	unsigned     getNumEvents()       const { return _numEvents;   }
	void         changeTimebase(unsigned mul, unsigned div);
	void         stop(MIDI *midiOut);
	void         rewind();

	// New items to fix some replay bugs.
	void         findSetupMessages();
	void         setupForPlayback(MIDI *midiOut);
	void         jumpTo(unsigned t);

	void         exec(MIDI *midiOut, AccompState &accomp, unsigned currentTime);

private:
	MIDIMessage      *_event;
	MIDIMessageArray  _setupMsg;

	unsigned _numEvents;
	unsigned _trackLength; // Length of track in ticks.
	unsigned _currentEvent;
	unsigned _currentTime;
	unsigned _channelMap; // Bit map of which channels are used by this track.

	static int loadTimestamp(MIDITimestamp &result, File &f);

	void linkNoteMessages();
	void allNotesOff(MIDI *midiOut);

	int decodeMetaEvent(File &f, MIDIFile &m);
};

#endif // OLD

#endif /* MIDITRACK_H_ */
