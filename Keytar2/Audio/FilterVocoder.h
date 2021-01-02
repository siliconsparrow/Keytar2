/*
 * FilterVocoder.h
 *
 *  Created on: 8 Apr 2020
 *      Author: adam
 */

#ifndef FILTERVOCODER_H_
#define FILTERVOCODER_H_

#include "AudioFilter.h"

class FilterVocoder: public AudioFilter {
public:
	FilterVocoder();
	virtual ~FilterVocoder() { }

	void setSource(AudioFilter *filter) { _src = filter; }

	virtual void fillFrame(StereoSample *frame);

private:
	AudioFilter *_src;
};

#endif /* FILTERVOCODER_H_ */
