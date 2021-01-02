/*
 * Synth.cpp
 *
 *  Created on: 30 Dec 2020
 *      Author: adam
 */

#include "Synth.h"
#include "UTimer.h"
#include <stdio.h>

static Synth *g_synth = 0;

Synth::Synth()
	: _keyboard(0)
{
    uTimerInit();
    _synth = new FilterFluidSynth();
    g_synth = this;

    _synth->setProgram(MIDIMessage::CHANNEL1, 1);
}

Synth::~Synth()
{
	g_synth = 0;
}

Gui::MusicKeyboard *Synth::createKeyboard(Gui::Rect r)
{
	_keyboard = new Gui::MusicKeyboard(r, &Synth::kbNoteOn, &Synth::kbNoteOff);
	return _keyboard;
}

void Synth::midiMessage(MIDIMessage msg)
{
	_synth->send(msg);
}

// Called when the on-screen keyboard is pressed.
void Synth::kbNoteOn(uint8_t noteNum)
{
	if(g_synth != 0)
		g_synth->midiMessage(MIDIMessage(MIDIMessage::NOTE_ON, MIDIMessage::CHANNEL1, noteNum, 127));
}
void Synth::kbNoteOff(uint8_t noteNum)
{
	if(g_synth != 0)
		g_synth->midiMessage(MIDIMessage(MIDIMessage::NOTE_OFF, MIDIMessage::CHANNEL1, noteNum));
}

void Synth::usbMidiEvent(MIDIMessage msg)
{
	_synth->send(msg);

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
//	} else {
//		printf("MIDI: %x %x %02x %02x\n", (int)msg->channel(), (int)msg->message(), (int)msg->param1(), (int)msg->param2());
	}
}
