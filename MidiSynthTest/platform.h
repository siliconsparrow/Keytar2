/*
 * platform.h
 *
 *  Created on: 25 Mar 2020
 *      Author: adam
 */

#ifndef PLATFORM_H_
#define PLATFORM_H_

#include "stm32f7xx_hal.h"

void SystemClock_Config();
void Error_Handler();
void MPU_Config();
void CPU_CACHE_Enable();

#endif /* PLATFORM_H_ */
