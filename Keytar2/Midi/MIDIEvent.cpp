// *****************************************************************************
// **
// ** A MIDI event is a MIDI message with added timestamp for playback at
// ** the right time.
// **
// **   by Adam Pierce <adam@siliconsparrow.com>
// **   created 1-Sep-2020
// **
// *****************************************************************************

#include "MIDIEvent.h"
#include "MIDIFile.h"

// Meta-events are command 0xFF. The next byte will be one of these types.
// the 3rd byte is the data length. This is followed by the data bytes.
enum MetaEventType
{
	META_SEQUENCE_NUMBER,       // META  0 - Sequence number
	META_TEXT,                  // META  1 - Text
	META_TEXT_COPYRIGHT,        // META  2 - Copyright text
	META_TEXT_TRACK_NAME,       // META  3 - Track name
	META_TEXT_INSTRUMENT_NAME,  // META  4 - Instrument name
	META_TEXT_LYRICS,           // META  5 - Lyrics
	META_TEXT_MARKER,           // META  6 - Marker text
	META_TEXT_CUE_POINT,        // META  7 - Cue point
	META_CHANNEL_PREFIX = 0x20, // META 32 - All following meta only apply to the given channel.
	META_TRACK_END = 0x2F,      // META 47 - Track end
	META_TEMPO = 0x51,          // META 81 - Tempo (24 bit, microseconds per quarter note)
	META_SMTPE_OFFSET = 0x54,   // META 84 - Track start time in SMTPE format.
	META_TIME_SIGNATURE = 0x58, // META 88 - Time signature
	META_KEY_SIGNATURE = 0x59,  // META 89 - Key signature

	META_SEQUENCER_EXCLUSIVE_DATA = 0x7F // META 7F - Proprietary data
};


int MIDIEvent::load(FileSystem::File &f, MIDIFile &m)
{
	//uint8_t     lastMsg = 0;
	uint8_t     b;
	int         byteCount = 0;
	MIDIDecoder decoder(this);

	// First read a timestamp.
	byteCount = loadTimestamp(&_ts, f);
	if(byteCount < 0)
		return byteCount; // Read error.

	// Then read the MIDI event.
	while(1)
	{
		if(!f.read(&b, 1))
			return -1; // Read error.

		byteCount++;

		// 0xFF has special meaning in a MID file. It indicates a meta-event
		if(b == 0xFF)
		{
			set(0,0,0,0);

			// Skip over meta-event.
			uint8_t buf[256];
			if(!f.read(buf, 2))
				return -1; // Read error.

			byteCount += 2;

			int meta = buf[0];
			int len = buf[1];
			if(len > 0)
			{
				if(!f.read(buf, len))
					return -1; // Read error.

				byteCount += len;
			}

			// Some meta events we recognise.
			switch(meta)
			{
			case META_TEMPO:
				m.setTempo(60000000 / ((buf[0] << 16) | (buf[1] << 8) | buf[2]));
				break;

			case META_TIME_SIGNATURE:
				m.setTimeSignature(buf[0], 1 << buf[1]);
				break;
			}
			break;
		}
		else
		{
			if(decoder.decodeByte(b))
				break;
		}
	}

	return byteCount;
}

// Read a MIDI timestamp from a file. Timestamps are encoded in a variable length
// manner kind of like UTF-8.
// Returns the number of bytes consumed or -1 on disk error.
int MIDIEvent::loadTimestamp(MIDITimestamp *result, FileSystem::File &f)
{
	uint32_t ts = 0;
	uint8_t b;
	int byteCount = 0;

	while(1)
	{
		if(!f.read(&b, 1))
			return -1;

		byteCount++;

		ts <<= 7;
		ts |= (b & 0x7F);

		if(0 == (b & 0x80))
			break;
	}
	*result = ts;
	return byteCount;
}

