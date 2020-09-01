// *****************************************************************************
// **
// ** Class to encapsulate a single MIDI message
// **
// **   by Adam Pierce <adam@siliconsparrow.com>
// **   created 4-Mar-2018
// **
// *****************************************************************************

//#include "MIDIMessage.h"
//#include "File.h"
//#include "MIDIFile.h"

#ifdef OLD

// Return the number of bytes in a packet of the given message type.
unsigned MIDIMessage::messageLength(uint8_t cmd)
{
	const int MIDI_PACKET_LEN[] = { 3, 3, 3, 3, 2, 2, 3 };
	const int MIDI_SYS_PACKET_LEN[] = { 1, 2, 3, 2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 };

	if(cmd >= SYSTEM)
		return MIDI_SYS_PACKET_LEN[cmd - (uint8_t)SYSTEM];

	return MIDI_PACKET_LEN[(cmd & 0x70) >> 4];
}

// Extract the command nibble from the packet.
MIDIMessage::MIDI_MESSAGE MIDIMessage::msgType() const
{
	if(_len == 0)
		return RESET;

	if(_data[0] >= SYSTEM)
		return (MIDI_MESSAGE)_data[0];

	return (MIDI_MESSAGE)(0xF0 & _data[0]);
}

// Decode an incoming byte.
// Return true if we now have a complete MIDI message.
bool MIDIMessage::decodeByte(uint8_t &lastMsg, uint8_t b)
{
	// Decode one byte of an incoming MIDI message.
	if(0 != (b & 0x80)) // Is this the start byte of a MIDI message?
	{
		_len = 0;
		lastMsg = b;
		_data[_len++] = b;
	}
	else if(_len < MIDI_MESSAGE_MAX_SIZE)
		_data[_len++] = b;

	else
	{
		// Handle "Running Status" messages.
		_len = 0;
		_data[_len++] = lastMsg;
		_data[_len++] = b;
	}

	return _len >= messageLength(_data[0]);
}

// This is an invalid message object.
MIDIMessage::MIDIMessage()
	: _ts(0)
	, _len(0)
	, _linkedEvent(0)
	, _lockTranspose(TRANSPOSE_NOLOCK)
{
}

// Create a message with only the command byte.
MIDIMessage::MIDIMessage(MIDITimestamp ts, MIDI_MESSAGE msg, uint8_t channel)
	: _ts(ts)
	, _len(0)
	, _linkedEvent(0)
	, _lockTranspose(TRANSPOSE_NOLOCK)
{
	_data[_len++] = (uint8_t)msg | channel;
}

// Create a message with one parameter.
MIDIMessage::MIDIMessage(MIDITimestamp ts, MIDI_MESSAGE msg, uint8_t channel, uint8_t param1)
	: _ts(ts)
	, _len(0)
	, _linkedEvent(0)
	, _lockTranspose(TRANSPOSE_NOLOCK)
{
	_data[_len++] = (uint8_t)msg | channel;
	_data[_len++] = param1 & 0x7F;
}

// Create a message with two parameters.
MIDIMessage::MIDIMessage(MIDITimestamp ts, MIDI_MESSAGE msg, uint8_t channel, uint8_t param1, uint8_t param2)
	: _ts(ts)
	, _len(0)
	, _linkedEvent(0)
	, _lockTranspose(TRANSPOSE_NOLOCK)
{
	_data[_len++] = (uint8_t)msg | channel;
	_data[_len++] = param1 & 0x7F;
	_data[_len++] = param2 & 0x7F;
}

void MIDIMessage::set(MIDI_MESSAGE msg)
{
	_len = 0;
	_data[_len++] = (uint8_t)msg;
}

void MIDIMessage::set(MIDI_MESSAGE msg, uint8_t param1)
{
	_len = 0;
	_data[_len++] = (uint8_t)msg;
	_data[_len++] = param1;
}
void MIDIMessage::set(MIDI_MESSAGE msg, uint8_t param1, uint8_t param2)
{
	_len = 0;
	_data[_len++] = (uint8_t)msg;
	_data[_len++] = param1 & 0x7F;
	_data[_len++] = param2;
}

const char *MIDIMessage::toHex() const
{
	static char result[16];
	static const char *HEXIT = "0123456789ABCDEF";
	
	unsigned p = 0;
	for(unsigned i = 0; i < _len; i++)
	{
		if(i > 0)
			result[p++] = ' ';
		result[p++] = HEXIT[_data[i] >> 4];
		result[p++] = HEXIT[_data[i] & 15];
	}
	result[p] = 0;
	return result;
}

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

// Read a MIDI timestamp from a file. Timestamps are encoded in a variable length
// manner kind of like UTF-8.
// Returns the number of bytes consumed or -1 on disk error.
int MIDIMessage::loadTimestamp(MIDITimestamp *result, File &f)
{
	uint32_t ts = 0;
	uint8_t b;
	int byteCount = 0;

	while(1)
	{
		if(!f.readByte(&b))
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

int MIDIMessage::load(File &f, MIDIFile &m)
{
	uint8_t lastMsg = 0;
	uint8_t b;
	int byteCount = 0;

	// First read a timestamp.
	byteCount = loadTimestamp(&_ts, f);
	if(byteCount < 0)
		return byteCount;
	
	// Then read the MIDI event.
	while(1)
	{
		if(!f.readByte(&b))
			return -1;
		
		byteCount++;
		
		// 0xFF has special meaning in a MID file. It indicates a meta-event
		if(b == 0xFF)
		{
			// Skip over meta-event.
			uint8_t buf[256];
			if(!f.read(buf, 2))
				return -1;
			
			byteCount += 2;
			
			int meta = buf[0];
			int len = buf[1];
			if(len > 0)
			{
				if(!f.read(buf, len))
					return -1;
				
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
			if(decodeByte(lastMsg, b))
				break;
		}
	}
	
	return byteCount;
}

// Returns a number from 0..11 (ie. strips off the octave) so you know
// what note this is.
uint8_t MIDIMessage::getBaseNote(uint8_t n)
{
	while(n >= 12)
		n -= 12;

	return n;
}


// Return a copy of the message data bytes with the given transposition applied to them.
void MIDIMessage::dataTransposed(uint8_t *result, int transpose, bool minor)
{
	for(int i = 0; i < MIDI_MESSAGE_MAX_SIZE; i++)
		result[i] = _data[i];

	MIDI_MESSAGE type = msgType();
	if(type == NOTE_ON || type == NOTE_OFF)
	{
		if(getChannel() != DRUMTRACK_CHANNEL) // Do not transpose drum track.
		{
			if(_lockTranspose != TRANSPOSE_NOLOCK)
			{
				result[1] += _lockTranspose;
			}
			else
			{
				uint8_t note = _data[1];
				if(minor && getBaseNote(note) == 4)
					transpose--; // Lower the 3rd if using minor scale.

				// TODO: No idea how I'm going to do 7th, 6th etc.

				if(_linkedEvent != 0)
					_linkedEvent->_lockTranspose = transpose;

				result[1] = note + transpose;
			}
		}
	}
}

// Copy the message bytes into the buffer provided.
void MIDIMessage::dataCopy(uint8_t *result)
{
	for(int i = 0; i < MIDI_MESSAGE_MAX_SIZE; i++)
		result[i] = _data[i];
}

#endif // OLD
