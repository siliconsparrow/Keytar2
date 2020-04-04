/*
 * PerfMon.h - Monitor how much CPU time is used for audio processing.
 *
 *  Created on: 3 Apr 2020
 *      Author: adam
 */

#ifndef PERFMON_H_
#define PERFMON_H_

class PerfMon {
public:

	PerfMon();

	enum ProcessId {
		Idle,
		Audio,
		Gui,
		Disk,

		nPids
	};

	static void init();

	static void     enter(ProcessId pid);
	static void     leave();
	static unsigned report(unsigned *result);

private:
	ProcessId _currentPid;
	unsigned  _sp;
	unsigned  _stat[nPids];
	ProcessId _stack[nPids * 2];

	void resetStats();
	void timerReset();
	unsigned timerGet();
};

#endif /* PERFMON_H_ */
