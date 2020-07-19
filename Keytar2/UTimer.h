/*
 * UTimer.h
 *
 *  Created on: 19 Jul 2020
 *      Author: adam
 */

#ifndef UTIMER_H_
#define UTIMER_H_

#include <sys/types.h>

#ifdef __cplusplus
extern "C" {
#endif

void uTimerInit();
int  usleep(useconds_t us);

#ifdef __cplusplus
}
#endif

#endif /* UTIMER_H_ */
