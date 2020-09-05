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
#include "MIDIEvent.h"
#include "FileSystem.h"
//#include "Accompaniment.h"
//#include "MIDIMessageArray.h"

// ARM byte swap functions.
#define ntohl __builtin_bswap32
#define ntohs __builtin_bswap16

//class MIDI;
class MIDIFile;
class MIDITrack
{
public:
	MIDITrack();
	~MIDITrack();

	bool         load(FileSystem::File &f, MIDIFile &m);

	bool         isEmpty() const { return _trackLength == 0; }
#ifdef OLD
	unsigned     getCurrentPosition() const { return _currentTime; }
#endif // OLD
	unsigned     getTotalLength()     const { return _trackLength; }
#ifdef OLD
	unsigned     getNumEvents()       const { return _numEvents;   }
	void         changeTimebase(unsigned mul, unsigned div);
	void         stop(MIDI *midiOut);
#endif // OLD
	void         rewind();

#ifdef OLD
	// New items to fix some replay bugs.
	void         findSetupMessages();
	void         setupForPlayback(MIDI *midiOut);
	void         jumpTo(unsigned t);

	void         exec(MIDI *midiOut, AccompState &accomp, unsigned currentTime);
#endif // OLD

private:
	MIDIEvent *_event;
#ifdef OLD
	MIDIEventArray  _setupMsg;
#endif // OLD
	unsigned  _numEvents;
	unsigned  _trackLength; // Length of track in ticks.
#ifdef OLD
	unsigned _currentEvent;
	unsigned _currentTime;
#endif // OLD
	unsigned _channelMap; // Bit map of which channels are used by this track.

#ifdef OLD
	static int loadTimestamp(MIDITimestamp &result, File &f);

	void linkNoteMessages();
	void allNotesOff(MIDI *midiOut);

	int decodeMetaEvent(File &f, MIDIFile &m);
#endif // OLD
};

#endif /* MIDITRACK_H_ */
