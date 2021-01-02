/*
 * DrumKit.cpp
 *
 *  Created on: 19 Jul 2020
 *      Author: adam
 */

#include "FilterDrumKit.h"
#include <stdio.h>

const char *DrumKitFileName[] = {
	"kick",
	"snare",
	"hatClosed",
	"hatOpen",
	"crash",
	"tomHigh",
	"tomMed",
	"tomLow",
	"ride",
	"rim",
	"clap"
};

FilterDrumKit::FilterDrumKit()
	: FilterMixer(kNumDrums)
{
	// Set up the mixer for all the drums.
	for(int i = 0; i < kNumDrums; i++) {
		this->setChannelSource(i, &_drum[i], kMaxLevel / 2);
	}

	// TODO: Maybe by being smart, I won't need to have so many mixer channels.
	//       eg. Only mix drums that are actually sounding at any given moment.
}

FilterDrumKit::~FilterDrumKit() {
}

bool FilterDrumKit::load(const char *path)
{
	for(int i = 0; i < kNumDrums; i++) {
		char filename[256];
		sprintf(filename, "%s/%s.wav", path, DrumKitFileName[i]);
		if(!_drum[i].load(filename))
			return false;
	}

	return true;
}

void FilterDrumKit::midi(MidiMessage msg)
{
	// We only support the NOTE ON command.
	if(MidiMessage::NOTE_ON == msg.message()) {
		uint8_t note = msg.param1();
		if(note >= MidiMessage::C4) {
			note -= MidiMessage::C4;
			if(note < kNumDrums) {
				_drum[note].play();
			}
		}
	}
}
