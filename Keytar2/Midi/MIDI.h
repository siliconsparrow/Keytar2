// *****************************************************************************
// **
// ** Receive and send MIDI messages
// **
// **   by Adam Pierce <adam@siliconsparrow.com>
// **   created 21-Jan-2018
// **
// *****************************************************************************

#ifndef MIDI_H_
#define MIDI_H_

//#include "UART.h"
//#include "Accompaniment.h"
#include "Midi/MIDIMessage.h"

// Class to handle transposition to different keys during MIDI playback.
#ifdef OLD
#include "MIDIMessage.h"
#include <stdint.h>

#define DEFAULT_ACCOMP_CHANNEL_MASK ((uint16_t)0xFBFE) // Default accompaniment channels are all except for 10 (drums) and 1 (keyboard).
#endif // OLD

class AccompState
{
public:
	AccompState();

#ifdef OLD
	void stopAccomp();
	void setChannelMask(uint16_t mask);
	void setTranspose(int transpose, bool minor);

	bool filterMessage(uint8_t *result, MIDIMessage &msg);

private:
	int      _transpose;
	bool     _minor;
	uint16_t _channelMask;
#endif // OLD
};


// Base class that could send MIDI messages internally or via UART.

class MIDISource
{
public:
	virtual ~MIDISource() { }

	virtual MIDIMessage *getMessage() = 0;
};

class MIDISink
{
public:
	virtual ~MIDISink() { }

	virtual void send(const MIDIMessage &msg) = 0;

	void         sendTransposed(MIDIMessage &msg, AccompState &accomp);
#ifdef OLD
	void         programChange(uint8_t channel, uint8_t pgm);
#endif // OLD
	void         controlChange(uint8_t channel, uint8_t cc, uint8_t value);
	void         allNotesOff(uint8_t channel);
#ifdef OLD
	void         resetControllers(uint8_t channel);
#endif // OLD
};

#ifdef OLD

class MIDI
{
public:
	MIDI(UART::kPortNum portNum);

	MIDIMessage *getMessage();
	void         send(const MIDIMessage &msg);
	void         sendTransposed(MIDIMessage &msg, AccompState &accomp);
	void         programChange(uint8_t channel, uint8_t pgm);
	void         controlChange(uint8_t channel, uint8_t cc, uint8_t value);
	void         allNotesOff(uint8_t channel);
	void         resetControllers(uint8_t channel);

	//bool tick();
	//void sendClock();
	//void noteOn(uint8_t channel, uint8_t note, uint8_t velocity);
	//void noteOff(uint8_t channel, uint8_t note, uint8_t velocity);
	//void allOff();
	//void allNotesOff();

	enum MIDINote
	{
		C0,  Cs0,  D0,  Ds0,  E0,  F0,  Fs0,  G0,  Gs0,  A0,  As0,  B0,
		C1,  Cs1,  D1,  Ds1,  E1,  F1,  Fs1,  G1,  Gs1,  A1,  As1,  B1,
		C2,  Cs2,  D2,  Ds2,  E2,  F2,  Fs2,  G2,  Gs2,  A2,  As2,  B2,
		C3,  Cs3,  D3,  Ds3,  E3,  F3,  Fs3,  G3,  Gs3,  A3,  As3,  B3,
		C4,  Cs4,  D4,  Ds4,  E4,  F4,  Fs4,  G4,  Gs4,  A4,  As4,  B4,
		C5,  Cs5,  D5,  Ds5,  E5,  F5,  Fs5,  G5,  Gs5,  A5,  As5,  B5,
		C6,  Cs6,  D6,  Ds6,  E6,  F6,  Fs6,  G6,  Gs6,  A6,  As6,  B6,
		C7,  Cs7,  D7,  Ds7,  E7,  F7,  Fs7,  G7,  Gs7,  A7,  As7,  B7,
		C8,  Cs8,  D8,  Ds8,  E8,  F8,  Fs8,  G8,  Gs8,  A8,  As8,  B8,
		C9,  Cs9,  D9,  Ds9,  E9,  F9,  Fs9,  G9,  Gs9,  A9,  As9,  B9,
		C10, Cs10, D10, Ds10, E10, F10, Fs10, G10, Gs10, A10, As10, B10,
	};

private:
	UART         _uart;
	MIDIMessage  _msgDecode;
	int          _msgPos;
	uint8_t      _lastCmd;
	uint8_t      _lastMsg;

	//bool processMessage(MIDIMessage &m);
};
#endif // OLD

#endif /* MIDI_H_ */
