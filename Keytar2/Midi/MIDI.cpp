// *****************************************************************************
// **
// ** Receive and send MIDI messages
// **
// **   by Adam Pierce <adam@siliconsparrow.com>
// **   created 21-Jan-2018
// **
// *****************************************************************************

#include "MIDI.h"
#include "GeneralMIDI.h"


#define NOACCOMP 0x10000

AccompState::AccompState()
//	: _transpose(NOACCOMP)
//	, _minor(false)
//	, _channelMask(DEFAULT_ACCOMP_CHANNEL_MASK)
{
}

#ifdef OLD
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
#endif // OLD


#ifdef OLD

#define MIDI_BIT_RATE 31250

//void blinkLed(); // for testing.

MIDI::MIDI(UART::kPortNum portNum)
	: _uart(portNum, MIDI_BIT_RATE)
	, _msgPos(0)
	, _lastCmd(0)
	, _lastMsg(0)
{
}

MIDIMessage *MIDI::getMessage()
{
	uint8_t b;

	while(_uart.recvByte(&b))
	{
		if(_msgDecode.decodeByte(_lastMsg, b))
			return &_msgDecode;
	}

	return 0;
}
/*
// Called very frequently. Returns true if it sent a message.
bool MIDI::tick()
{
	static uint8_t g_lastMsg = 0;

	uint8_t b;

	while(_uart.recvByte(&b))
	{
		if(_msgDecode.decodeByte(g_lastMsg, b))
			return processMessage(_msgDecode);
	}

	return false;
}

// Called when a complete MIDI message has been received.
// Returns true if it sent a message.
bool MIDI::processMessage(MIDIMessage &m)
{
	// Force channel 1 because who knows what channel the A500 will send us.
	m.setChannel(0);

	// Change aftertouch message to modulation message.
	// Why does the aftertouch default to pitch-bend? Such a dumb idea.
	if(m.msgType() == MIDIMessage::PITCH_BEND)
	{
		m.data()[0] = MIDIMessage::CONTROL_CHANGE | m.getChannel();
		m.data()[2] = m.data()[1];
		m.data()[1] = GM_CC_MODWHEEL;
	}

	// We just echo for now.
	if(m.msgType() < MIDIMessage::SYSTEM)
	{
		if(m.msgType() == MIDIMessage::NOTE_OFF)
		{
			// Turn NOTE OFF into NOTE_ON with zero velocity. These are more
			// efficient to send.
			m.data()[0] |= 0x10;
			m.data()[2] = 0;
		}

		send(m);
		return true;
	}

	return false;
}
*/

// Sends a MIDI message.
void MIDI::send(const MIDIMessage &msg)
{
	const uint8_t *data = msg.data();

	if(_lastCmd == data[0])
		_uart.send(&data[1], msg.dataLength() - 1);
	else
		_uart.send(data, msg.dataLength());
}
#endif // OLD

void MIDISink::sendTransposed(MIDIMessage &msg, AccompState &accomp)
{
#ifdef OLD
	// Get the transposed data from the message.
	uint8_t data[MIDI_MESSAGE_MAX_SIZE];
	if(!accomp.filterMessage(data, msg))
		return;

	// Send it.
	if(_lastCmd == data[0])
		_uart.send(&data[1], msg.dataLength() - 1);
	else
		_uart.send(data, msg.dataLength());

#endif // OLD

	send(msg);
}

#ifdef OLD
/*
// Sends a MIDI clock pulse.
void MIDI::sendClock()
{
	MIDIMessage msg(0, MIDIMessage::SEQ_CLOCK, 0);
	send(msg);
}

// Send a note-on message.
void MIDI::noteOn(uint8_t channel, uint8_t note, uint8_t velocity)
{
	MIDIMessage m(0, MIDIMessage::NOTE_ON, channel, note, velocity);
	send(m);
}

// Send a note-off message.
void MIDI::noteOff(uint8_t channel, uint8_t note, uint8_t velocity)
{
	MIDIMessage m(0, MIDIMessage::NOTE_OFF, channel, note, velocity);
	send(m);
}
*/
void MIDI::programChange(uint8_t channel, uint8_t pgm)
{
	MIDIMessage m(0, MIDIMessage::PROGRAM_CHANGE, channel, pgm);
	send(m);
}
#endif // OLD

void MIDISink::controlChange(uint8_t channel, uint8_t cc, uint8_t value)
{
	MIDIMessage m(MIDIMessage::CONTROL_CHANGE, channel, cc, value);
	send(m);
}

void MIDISink::allNotesOff(uint8_t channel)
{
	controlChange(channel, GM_CC_ALL_NOTES_OFF, 0);
}

#ifdef OLD
void MIDI::resetControllers(uint8_t channel)
{
	controlChange(channel, GM_CC_ALL_CONTROLLERS_OFF, 0);
}

#endif // OLD
