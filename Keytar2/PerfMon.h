/*
 * PerfMon.h - Monitor how much CPU time is used for audio processing.
 *
 *  Created on: 3 Apr 2020
 *      Author: adam
 */

#ifndef PERFMON_H_
#define PERFMON_H_

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

enum ProcessId {
	pidIdle,
	pidAudio,
	pidGui,
	pidUsb,
	pidDisk,

	nPids
};

extern const char *perfPidName[];

void     perfInit();
void     perfEnter(unsigned pid);
void     perfLeave();
unsigned perfReport(unsigned *result);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif /* PERFMON_H_ */
