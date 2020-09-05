// *****************************************************************************
// **
// ** Class to encapsulate a single MIDI message
// **
// **   by Adam Pierce <adam@siliconsparrow.com>
// **   created 4-Mar-2018
// **
// *****************************************************************************

#include "MIDIMessage.h"


MIDIMessage::MIDIMessage()
	: _msg(0)
	, _chan(0)
	, _param1(0)
	, _param2(0)
{
}

MIDIMessage::MIDIMessage(MESSAGE msg, uint8_t channel, uint8_t param1, uint8_t param2)
	: _msg((uint8_t)msg)
	, _chan(channel)
	, _param1(param1)
	, _param2(param2)
{
}

void MIDIMessage::set(uint8_t msg, uint8_t channel, uint8_t param1, uint8_t param2)
{
	_msg    = msg;
	_chan   = channel;
	_param1 = param1;
	_param2 = param2;
}

// Return the number of bytes in a packet of the given message type.
unsigned MIDIMessage::messageLength(uint8_t cmd)
{
	const int MIDI_PACKET_LEN[] = { 3, 3, 3, 3, 2, 2, 3 };
	const int MIDI_SYS_PACKET_LEN[] = { 1, 2, 3, 2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 };

	if(cmd < 0x80)
		return 0;

	if(cmd >= MIDIMessage::SYSTEM)
		return MIDI_SYS_PACKET_LEN[cmd - (uint8_t)MIDIMessage::SYSTEM];

	return MIDI_PACKET_LEN[(cmd & 0x70) >> 4];
}



// Take bytes from some MIDI source and decode into MIDI messages.
MIDIDecoder::MIDIDecoder(MIDIMessage *msg)
	: _msg(msg)
	, _len(0)
	, _lastMsg(0)
{
	_data[1] = 0;
	_data[2] = 0;
}

// Decode one byte of an incoming MIDI message.
// If it has a complete message, it will return
// a pointer to it. Otherwise it returns NULL.
bool MIDIDecoder::decodeByte(uint8_t b)
{
	if(0 != (b & 0x80)) // Is this the start byte of a MIDI message?
	{
		_len = 0;
		_lastMsg = b;
		_data[_len++] = b;
	}
	else if(_len < kMessageMaxSize)
	{
		_data[_len++] = b;
	}
	else
	{
		// Handle "Running Status" messages.
		_len = 0;
		_data[_len++] = _lastMsg;
		_data[_len++] = b;
	}

	if(_len >= MIDIMessage::messageLength(_data[0]))
	{
		if(_data[0] >= MIDIMessage::SYSTEM)
			_msg->set(_data[0], 0, _data[1], _data[2]);
		else
			_msg->set(_data[0] & 0xF0, _data[0] & 0x0F, _data[1], _data[2]);

		_len = 0;
		return true;
	}

	return false;
}


#ifdef OLD

// Extract the command nibble from the packet.
MIDIMessage::MIDI_MESSAGE MIDIMessage::msgType() const
{
	if(_len == 0)
		return RESET;

	if(_data[0] >= SYSTEM)
		return (MIDI_MESSAGE)_data[0];

	return (MIDI_MESSAGE)(0xF0 & _data[0]);
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
