/*
 * Synth.h
 *
 *  Created on: 30 Dec 2020
 *      Author: adam
 */

#ifndef SYNTH_H_
#define SYNTH_H_

#include "USBMidi.h"
#include "MusicKeyboard.h"
#include "FilterFluidSynth.h"

class Synth
	: public USBMidi::Delegate
{
public:
	Synth();
	virtual ~Synth();

	Gui::MusicKeyboard *createKeyboard(Gui::Rect r);

	AudioFilter *getFilter() { return _synth; }

	void midiMessage(MIDIMessage msg);

protected:
	virtual void usbMidiEvent(MIDIMessage msg);

private:
	Gui::MusicKeyboard *_keyboard;
    FilterFluidSynth   *_synth;

	static void kbNoteOn(uint8_t noteNum);
	static void kbNoteOff(uint8_t noteNum);
};

#endif // SYNTH_H_
