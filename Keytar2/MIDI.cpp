// *****************************************************************************
// **
// ** Receive and send MIDI messages
// **
// **   by Adam Pierce <adam@siliconsparrow.com>
// **   created 21-Jan-2018
// **
// *****************************************************************************

#ifdef OLD
// TODO: Put this code on the M0 core
// TODO: Some way (callback) for packets to trigger DSP sound output.

#include "MIDI.h"
#include "GeneralMIDI.h"

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

void MIDI::sendTransposed(MIDIMessage &msg, AccompState &accomp)
{
	// Get the transposed data from the message.
	uint8_t data[MIDI_MESSAGE_MAX_SIZE];
	if(!accomp.filterMessage(data, msg))
		return;

	// Send it.
	if(_lastCmd == data[0])
		_uart.send(&data[1], msg.dataLength() - 1);
	else
		_uart.send(data, msg.dataLength());
}

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

void MIDI::controlChange(uint8_t channel, uint8_t cc, uint8_t value)
{
	MIDIMessage m(0, MIDIMessage::CONTROL_CHANGE, channel, cc, value);
	send(m);
}
/*
void MIDI::allOff()
{
	controlChange(0, GM_CC_ALL_NOTES_OFF, 0);
	controlChange(0, GM_CC_ALL_CONTROLLERS_OFF, 0);
}

void MIDI::allNotesOff()
{
	controlChange(0, GM_CC_ALL_NOTES_OFF, 0);
}
*/

void MIDI::allNotesOff(uint8_t channel)
{
	controlChange(channel, GM_CC_ALL_NOTES_OFF, 0);
}

void MIDI::resetControllers(uint8_t channel)
{
	controlChange(channel, GM_CC_ALL_CONTROLLERS_OFF, 0);
}

#endif // OLD
