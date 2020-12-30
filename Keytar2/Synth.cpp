/*
 * Synth.cpp
 *
 *  Created on: 30 Dec 2020
 *      Author: adam
 */

#include "Synth.h"
#include <stdio.h>

Synth::Synth()
	: _keyboard(0)
{
}

Gui::MusicKeyboard *Synth::createKeyboard(Gui::Rect r)
{
	_keyboard = new Gui::MusicKeyboard(r/*, &Synth::kbNoteOn, &Synth::kbNoteOff*/);
	return _keyboard;
}

void Synth::usbMidiEvent(MIDIMessage msg)
{
	if(0 != _keyboard) {
		switch(msg.message())
		{
		case MIDIMessage::NOTE_ON:
			_keyboard->noteOn(msg.param1());
			break;

		case MIDIMessage::NOTE_OFF:
			_keyboard->noteOff(msg.param1());
			break;
		}
	} else {
		printf("MIDI: %x %x %02x %02x\n", (int)msg.channel(), (int)msg.message(), (int)msg.param1(), (int)msg.param2());
	}
}
