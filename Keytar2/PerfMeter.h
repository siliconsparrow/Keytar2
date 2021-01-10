/*
 * PerfMeter.h - Visual display of performance metrics
 *
 *  Created on: 6 Apr 2020
 *      Author: adam
 */

#ifndef PERFMETER_H_
#define PERFMETER_H_

#include "Gui.h"
#include "Label.h"
#include "PerfMon.h"
#include "Systick.h"

class PerfMeter
	: public Systick::Delegate
{
public:
	enum {
		kWidth = 80,
		kLineHeight = 12
	};

	PerfMeter(Gui::Gui *gui, int x, int y);

protected:
	virtual void evtTick();

private:
	enum {
		kCacheSize = nPids + 2
	};
    Gui::Label *_lblPerf[kCacheSize];
    unsigned    _cache[kCacheSize];
};

#endif /* PERFMETER_H_ */
