/*
 * PerMeter.cpp
 *
 *  Created on: 6 Apr 2020
 *      Author: adam
 */

#include "PerfMeter.h"
#include <stdio.h>

PerfMeter::PerfMeter(Gui::Gui *gui, int x, int y)
{
    for(int i = 0; i < nPids; i++) {
    	_lblPerf[i] = new Gui::Label(Gui::Rect(x, y + (i * kLineHeight), kWidth, kLineHeight), perfPidName[i]);
    	_cache[i] = 256;
    	gui->add(_lblPerf[i]);
    }

    Systick::instance()->add(this, 50);
}

void PerfMeter::evtTick()
{
	// Update CPU usage.
	char buf[32];
	unsigned perf[nPids];
	unsigned n = perfReport(perf);
	int tot = 0;
	for(int i = 0; i < nPids; i++)
		tot += perf[i];

	for(int i = 0; i < nPids; i++) {
		int p = (100 * perf[i]) / tot;
		if(p != _cache[i]) {
			sprintf(buf, "%5s:%3d%%", perfPidName[i], p);
			_lblPerf[i]->setText(buf);
			_cache[i] = p;
		}
	}
}
