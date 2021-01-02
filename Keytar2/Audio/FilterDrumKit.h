/*
 * DrumKit.h
 *
 *  Created on: 19 Jul 2020
 *      Author: adam
 */

#ifndef FILTERDRUMKIT_H_
#define FILTERDRUMKIT_H_

#include "FilterMixer.h"
#include "FilterSample.h"

class FilterDrumKit
	: public FilterMixer
{
public:
	FilterDrumKit();
	virtual ~FilterDrumKit();

	enum DRUM {
		kick,
		snare,
		hatClosed,
		hatOpen,
		crash,
		tomHigh,
		tomMed,
		tomLow,
		ride,
		rim,
		clap,

		kNumDrums
	};

	bool load(const char *path);

	virtual void midi(MidiMessage msg);

//	void trigger(DRUM drum);

private:
	FilterSample _drum[kNumDrums];
};

#endif /* FILTERDRUMKIT_H_ */
