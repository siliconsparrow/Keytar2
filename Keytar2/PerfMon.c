/*
 * PerfMon.c
 *
 *  Created on: 3 Apr 2020
 *      Author: adam
 */

#include "PerfMon.h"
#include "stm32f7xx_hal.h"
#include "stm32f7xx_hal_rcc_ex.h"
#include <string.h>

#ifdef USE_PERFMON

	#define PERF_TIMER TIM3
	#define PERF_TIMER_FREQ 1000000

	#define PERF_MAX_STACK (nPids + 4)

	static TIM_HandleTypeDef hPerfTimer;

	static unsigned _perfCurrentPid;
	static unsigned _perfSp;
	static unsigned _perfStat[nPids];
	static unsigned _perfStack[PERF_MAX_STACK];

	static void perfResetStats();

	const char *perfPidName[] = {
		"Idle",
		"Audio",
		"GUI",
		"USB",
		"Disk"
	};

	// Set up a timer with microsecond or better resolution.
	void perfInit()
	{
		_perfCurrentPid = pidIdle;
		_perfSp = 0;
		perfResetStats();

		// Activate the timer.
		__HAL_RCC_TIM3_CLK_ENABLE();

		// Compute the prescaler value
		uint32_t uwPrescalerValue = (uint32_t) ((SystemCoreClock / 2) / PERF_TIMER_FREQ) - 1;

		// Set TIMx instance
		hPerfTimer.Instance = PERF_TIMER;
		hPerfTimer.Init.Period = 0xFFFFFFFF;
		hPerfTimer.Init.Prescaler = uwPrescalerValue;
		hPerfTimer.Init.ClockDivision = 0;
		hPerfTimer.Init.CounterMode = TIM_COUNTERMODE_UP;
		hPerfTimer.Init.RepetitionCounter = 0;
		hPerfTimer.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;
		if (HAL_TIM_Base_Init(&hPerfTimer) != HAL_OK) {
			// Initialization Error
			while (1)
				;
		}

		// Start the timer.
		if (HAL_TIM_Base_Start(&hPerfTimer) != HAL_OK) {
			// Starting Error
			while (1)
				;
		}
	}

	void perfEnter(unsigned pid)
	{
		while(_perfSp >= (PERF_MAX_STACK - 1))
			; // Freeze on stack overflow.

		_perfStat[_perfCurrentPid] += PERF_TIMER->CNT;
		_perfStack[_perfSp++] = _perfCurrentPid;
		_perfCurrentPid = pid;
		PERF_TIMER->CNT = 0;
	}

	void perfLeave()
	{
		while(_perfSp <= 0)
			; // Freeze on stack underflow.

		_perfStat[_perfCurrentPid] += PERF_TIMER->CNT;
		_perfCurrentPid = _perfStack[--_perfSp];
		PERF_TIMER->CNT = 0;
	}

	unsigned perfReport(unsigned *result)
	{
		memcpy(result, _perfStat, sizeof(_perfStat));
		perfResetStats();
		return nPids;
	}

	void perfResetStats()
	{
		memset(&_perfStat[0], 0, sizeof(_perfStat));
	}

#else

	unsigned perfReport(unsigned *result)
	{
		return 0;
	}

#endif // USE_PERFMON
