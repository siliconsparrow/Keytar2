/*
 * Synth.cpp
 *
 *  Created on: 30 Dec 2020
 *      Author: adam
 */

#include "Synth.h"
#include "UTimer.h"
#include "Button.h"
#include <stdio.h>

static Synth *g_synth = 0;

Synth::Synth()
	: _keyboard(0)
	, _btnStartStop(0)
{
    uTimerInit();
    _synth = new FilterFluidSynth();
    g_synth = this;

    _accomp.load("Psycho2.MID"); // "drumtest1.mid"); // "testMidi.mid");

    _synth->setProgram(MIDIMessage::CHANNEL1, 1);
}

Synth::~Synth()
{
	g_synth = 0;
}

void Synth::createControls(Gui::Gui *gui)
{
    // An on-screen keyboard might be useful.
	//_keyboard = new Gui::MusicKeyboard(Gui::Rect(0, 100, 480, 100), &Synth::kbNoteOn, &Synth::kbNoteOff);
	//gui->add(_keyboard);

    // Select patches within SF2 file.
    int x = 170;
    const char *NUM[] = { "1", "2", "3", "4", "5", "6", "7", "8", "9", "0" };
    for(int i = 0; i < 8; i++) {
    	char buf[2];
    	buf[0] = '1' + i;
    	buf[1] = 0;
    	gui->add(new Gui::Button(Gui::Rect(x, 10, 25, 30), NUM[i], 0, &fnPgmButton, i));
    	x += 28;
    }

    // Start/stop button
    _btnStartStop = new Gui::Button(Gui::Rect(10, 10, 100, 34), "START", 0, &fnStartStop);
    gui->add(_btnStartStop);
}

void Synth::fnPgmButton(unsigned tag)
{
	g_synth->_synth->setProgram(MIDIMessage::CHANNEL2, tag);
}

void Synth::fnStartStop(unsigned tag)
{
	g_synth->startStop();
}

void Synth::startStop()
{
	if(_accomp.isPlaying()) {
		_accomp.stop(_synth);
		_btnStartStop->setText("START");
	} else {
		_accomp.start(_synth);
		_btnStartStop->setText("STOP");
	}
}

void Synth::poll()
{
	_accomp.poll(_synth);
}

void Synth::midiMessage(MIDIMessage msg)
{
	_synth->send(msg);
}

// Called when the on-screen keyboard is pressed.
void Synth::kbNoteOn(uint8_t noteNum)
{
	g_synth->midiMessage(MIDIMessage(MIDIMessage::NOTE_ON, MIDIMessage::CHANNEL1, noteNum, 127));
}
void Synth::kbNoteOff(uint8_t noteNum)
{
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
