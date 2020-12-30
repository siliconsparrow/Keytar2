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

class Synth
	: public USBMidi::Delegate
{
public:
	Synth();
	virtual ~Synth() { }

	Gui::MusicKeyboard *createKeyboard(Gui::Rect r);

protected:
	virtual void usbMidiEvent(MIDIMessage msg);

private:
	Gui::MusicKeyboard *_keyboard;
};

#endif // SYNTH_H_
