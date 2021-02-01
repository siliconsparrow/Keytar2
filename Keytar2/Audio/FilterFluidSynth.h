/*
 * FluidSynth.h
 *
 *  Created on: 18 Apr 2020
 *      Author: adam
 */

#ifndef FILTERFLUIDSYNTH_H_
#define FILTERFLUIDSYNTH_H_

#include "AudioFilter.h"
#include "MIDI.h"
#include "platform.h"
#include "lfluidsynth.h"

class FilterFluidSynth
	: public AudioFilter
	, public MIDISink
{
public:
	FilterFluidSynth();
	virtual ~FilterFluidSynth() { }

	void replaceSF(const char *filename);
	bool setBank(unsigned channel, unsigned bank);
	bool setProgram(unsigned channel, unsigned program);

	// AudioFilter implementation.
	virtual void fillFrame(StereoSample *frame);

	// MIDISink implementation.
	virtual void send(const MIDIMessage &msg);

private:
	fluid_synth_t *_synth;
	int _sfId;
};

#endif /* FILTERFLUIDSYNTH_H_ */
