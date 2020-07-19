/*
 * FluidSynth.h
 *
 *  Created on: 18 Apr 2020
 *      Author: adam
 */

#ifndef FILTERFLUIDSYNTH_H_
#define FILTERFLUIDSYNTH_H_

#include "AudioFilter.h"
#include "lfluidsynth.h"

class FilterFluidSynth: public AudioFilter {
public:
	FilterFluidSynth();
	virtual ~FilterFluidSynth() { }

	void replaceSF(const char *filename);

	virtual void fillFrame(StereoSample *frame);

	void midiRecv(uint8_t *msg, uint32_t len);
	void noteOn(unsigned noteNum, unsigned velocity);
	void noteOff(unsigned noteNum);
	void cc(unsigned channel, unsigned value);

private:
	fluid_synth_t *_synth;
	int _sfId;
};

#endif /* FILTERFLUIDSYNTH_H_ */
