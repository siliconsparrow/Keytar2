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
#include "Button.h"
#include "Accompaniment.h"

class Synth
	: public USBMidi::Delegate
{
public:
	Synth();
	virtual ~Synth();

	void poll();

	void createControls(Gui::Gui *gui);

	AudioFilter *getFilter() { return _synth; }

	void midiMessage(MIDIMessage msg);

protected:
	virtual void usbMidiEvent(MIDIMessage msg);

private:
	Gui::MusicKeyboard *_keyboard;
    FilterFluidSynth   *_synth;
    Gui::Button        *_btnStartStop;
    Accompaniment       _accomp;

	void startStop();

	static void kbNoteOn(uint8_t noteNum);
	static void kbNoteOff(uint8_t noteNum);
	static void fnPgmButton(unsigned tag);
	static void fnStartStop(unsigned tag);
};

#endif // SYNTH_H_
