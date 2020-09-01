// *****************************************************************************
// **
// ** Class to encapsulate a single MIDI message
// **
// **   by Adam Pierce <adam@siliconsparrow.com>
// **   created 19-Jul-2020
// **
// *****************************************************************************

#ifndef MIDIMESSAGE_H_
#define MIDIMESSAGE_H_

#include <stdint.h>

class __attribute__((packed)) MIDIMessage
{
public:
	enum
	{
		kMessageMaxSize = 3,   // Exception is SYS_EX which can be any length but I don't handle those ATM.
		kDrumtrackChannel = 9, // Traditional channel to use for drums.
		kDrumBank = 120,       // Bank number which usually contains drums.
		kChannels = 16         // Total number of MIDI channels.
	};

	enum MESSAGE
	{
		// Standard messages.
		NOTE_OFF         = 0x80, // Note off - params: note number, velocity
		NOTE_ON          = 0x90, // Note on  - params: note number, velocity
		POLY_PRESSURE    = 0xA0, // Poly pressure - params: note number, pressure
		CONTROL_CHANGE   = 0xB0, // Control change - params: controller number, value
		PROGRAM_CHANGE   = 0xC0, // Program change - params: program number
		CHANNEL_PRESSURE = 0xD0, // Aftertouch - params: pressure
		PITCH_BEND       = 0xE0, // Pitch bend - params: lsb, msb

		// System messages.
		SYSTEM       = 0xF0,
		SYS_EX_START = 0xF0, // Start sending proprietary data.
		QFRAME       = 0xF1, // MIDI Time Code quarter frame. Params: 0nnndddd (msg nnn = type, dddd = value)
		SONG_POS     = 0xF2, // Number of beats since start of song. Params: lsb, msb
		SONG_SELECT  = 0xF3, // Select sequence. Params: sequence number.
		TUNE_REQUEST = 0xF6, // Request instrument tune itself. No params.
		SYS_EX_END   = 0xF7, // End of proprietary data (see SYS_EX_START)
		SEQ_CLOCK    = 0xF8, // Sequence clock. Sent 24 times per quarter note. No params
		SEQ_START    = 0xFA, // Sequence start. No params.
		SEQ_CONTINUE = 0xFB, // Sequence continue. No params.
		SEQ_STOP     = 0xFC, // Sequence stop. No params.
		ACTIVE_SENSE = 0xFE, // Send 3x per second. Used as keepalive in some systems. No params.
		RESET        = 0xFF  // Reset all receivers to power up state.
	};

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

	MIDIMessage() : _msg(0), _chan(0), _param1(0), _param2(0) { }
	MIDIMessage(MESSAGE msg, uint8_t channel) : _msg(msg), _chan(channel), _param1(0), _param2(0) { }
	MIDIMessage(MESSAGE msg, uint8_t channel, uint8_t param1) : _msg(msg), _chan(channel), _param1(param1), _param2(0) { }
	MIDIMessage(MESSAGE msg, uint8_t channel, uint8_t param1, uint8_t param2) : _msg(msg), _chan(channel), _param1(param1), _param2(param2) { }

	MESSAGE message() const { return (MESSAGE)_msg; }
	uint8_t channel() const { return _chan; }
	uint8_t param1()  const { return _param1; }
	uint8_t param2()  const { return _param2; }

private:
	uint8_t _msg;
	uint8_t _chan;
	uint8_t _param1;
	uint8_t _param2;
};

#ifdef OLD
#include <stdint.h>

#define TRANSPOSE_NOLOCK 0x10000

#define MIDI_MESSAGE_MAX_SIZE 3 // Exception is SYS_EX which can be any length but I don't handle those ATM.

#define DRUMTRACK_CHANNEL   9
#define DRUM_BANK         120
#define MIDI_NUM_CHANNELS  16

typedef uint32_t MIDITimestamp;

class File;
class MIDIFile;
class __attribute__((packed)) MIDIMessage
{
public:
	enum MIDI_MESSAGE
	{
		// Standard messages.
		NOTE_OFF         = 0x80, // Note off - params: note number, velocity
		NOTE_ON          = 0x90, // Note on  - params: note number, velocity
		POLY_PRESSURE    = 0xA0, // Poly pressure - params: note number, pressure
		CONTROL_CHANGE   = 0xB0, // Control change - params: controller number, value
		PROGRAM_CHANGE   = 0xC0, // Program change - params: program number
		CHANNEL_PRESSURE = 0xD0, // Aftertouch - params: pressure
		PITCH_BEND       = 0xE0, // Pitch bend - params: lsb, msb

		// System messages.
		SYSTEM       = 0xF0,
		SYS_EX_START = 0xF0, // Start sending proprietary data.
		QFRAME       = 0xF1, // MIDI Time Code quarter frame. Params: 0nnndddd (msg nnn = type, dddd = value)
		SONG_POS     = 0xF2, // Number of beats since start of song. Params: lsb, msb
		SONG_SELECT  = 0xF3, // Select sequence. Params: sequence number.
		TUNE_REQUEST = 0xF6, // Request instrument tune itself. No params.
		SYS_EX_END   = 0xF7, // End of proprietary data (see SYS_EX_START)
		SEQ_CLOCK    = 0xF8, // Sequence clock. Sent 24 times per quarter note. No params
		SEQ_START    = 0xFA, // Sequence start. No params.
		SEQ_CONTINUE = 0xFB, // Sequence continue. No params.
		SEQ_STOP     = 0xFC, // Sequence stop. No params.
		ACTIVE_SENSE = 0xFE, // Send 3x per second. Used as keepalive in some systems. No params.
		RESET        = 0xFF  // Reset all receivers to power up state.
	};

	MIDIMessage();
	MIDIMessage(MIDITimestamp ts, MIDI_MESSAGE msg, uint8_t channel);
	MIDIMessage(MIDITimestamp ts, MIDI_MESSAGE msg, uint8_t channel, uint8_t param1);
	MIDIMessage(MIDITimestamp ts, MIDI_MESSAGE msg, uint8_t channel, uint8_t param1, uint8_t param2);

	int load(File &f, MIDIFile &m);

	void set(MIDI_MESSAGE msg);
	void set(MIDI_MESSAGE msg, uint8_t param1);
	void set(MIDI_MESSAGE msg, uint8_t param1, uint8_t param2);

	void setChannel(unsigned ch) { _data[0] = (_data[0] & 0xF0) | (ch & 0xF); }

	static unsigned messageLength(uint8_t cmd);

	MIDI_MESSAGE msgType() const;

	void decodeStart();
	bool decodeByte(uint8_t &g_lastMsg, uint8_t b);

	uint8_t       *data()               { return _data; }
	const uint8_t *data()         const { return _data; }
	unsigned       dataLength()   const { return _len; }
	uint8_t        getChannel()   const { return _data[0] & 0xF; }
	unsigned       getDeltaTime() const { return _ts; }

	void           dataTransposed(uint8_t *result, int transpose, bool minor);
	void           dataCopy(uint8_t *result);

	void           setDeltaTime(unsigned dt) { _ts = dt; }

	void           linkEvent(MIDIMessage *evt) { _linkedEvent = evt; }

private:
	MIDITimestamp _ts;
	uint8_t       _len;
	uint8_t       _data[MIDI_MESSAGE_MAX_SIZE];
	MIDIMessage  *_linkedEvent;
	int           _lockTranspose;

	static int loadTimestamp(MIDITimestamp *result, File &f);

	static uint8_t getBaseNote(uint8_t n);

	const char *toHex() const;
};

#endif // OLD

#endif /* MIDIMESSAGE_H_ */
