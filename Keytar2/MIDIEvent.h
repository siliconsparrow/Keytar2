// *****************************************************************************
// **
// ** A MIDI event is a MIDI message with added timestamp for playback at
// ** the right time.
// **
// **   by Adam Pierce <adam@siliconsparrow.com>
// **   created 1-Sep-2020
// **
// *****************************************************************************

#ifndef MIDIEVENT_H_
#define MIDIEVENT_H_

#include "FileSystem.h"
#include "MIDIMessage.h"
#include <stdint.h>

typedef uint32_t MIDITimestamp;

class MIDIFile;
class MIDIEvent : public MIDIMessage
{
public:
	MIDIEvent() : _ts(0) { }

	int load(FileSystem::File &f, MIDIFile &m);

	unsigned getDeltaTime() const { return (unsigned)_ts; }

private:
	MIDITimestamp _ts;

	static int loadTimestamp(MIDITimestamp *result, FileSystem::File &f);
};

#endif /* MIDIEVENT_H_ */
