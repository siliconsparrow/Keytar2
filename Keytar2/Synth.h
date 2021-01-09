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
#include "Gui.h"

class Synth
	: public USBMidi::Delegate
{
public:
	Synth();
	virtual ~Synth();

	void createControls(Gui::Gui *gui);

	AudioFilter *getFilter() { return _synth; }

	void midiMessage(MIDIMessage msg);

protected:
	virtual void usbMidiEvent(MIDIMessage msg);

private:
	Gui::MusicKeyboard *_keyboard;
    FilterFluidSynth   *_synth;

	static void kbNoteOn(uint8_t noteNum);
	static void kbNoteOff(uint8_t noteNum);
	static void fnPgmButton(unsigned tag);
};

#endif // SYNTH_H_
