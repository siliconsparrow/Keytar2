/*
 * PerfMon.cpp
 *
 *  Created on: 3 Apr 2020
 *      Author: adam
 */

#include "PerfMon.h"
#include "stm32f7xx_hal.h"
#include "stm32f7xx_hal_rcc_ex.h"
#include <string.h>

#define PERF_TIMER TIM3
#define PERF_TIMER_FREQ 1000000

static TIM_HandleTypeDef    TimHandle;

// Singleton instance.
static PerfMon g_perfMon;

PerfMon::PerfMon()
	: _currentPid(Idle)
	, _sp(0)
{
	resetStats();
}

void PerfMon::enter(ProcessId pid)
{
	unsigned t = g_perfMon.timerGet();
	g_perfMon._stat[g_perfMon._currentPid] += t;
	g_perfMon._stack[g_perfMon._sp++] = g_perfMon._currentPid;
	g_perfMon._currentPid = pid;
	g_perfMon.timerReset();
}

void PerfMon::leave()
{
	g_perfMon._stat[g_perfMon._currentPid] += g_perfMon.timerGet();
	g_perfMon._currentPid = g_perfMon._stack[--g_perfMon._sp];
	g_perfMon.timerReset();
}

unsigned PerfMon::report(unsigned *result)
{
	memcpy(result, g_perfMon._stat, sizeof(g_perfMon._stat));
	g_perfMon.resetStats();
	return nPids;
}

void PerfMon::resetStats()
{
	memset(&_stat[0], 0, sizeof(_stat));
}


// Set up a timer with microsecond or better resolution.
void PerfMon::init()
{

	__HAL_RCC_TIM3_CLK_ENABLE();

	  // Compute the prescaler value
	  uint32_t uwPrescalerValue = (uint32_t)((SystemCoreClock / 2) / PERF_TIMER_FREQ) - 1;

	  /* Set TIMx instance */
	  TimHandle.Instance               = PERF_TIMER;
	  TimHandle.Init.Period            = 0xFFFFFFFF;
	  TimHandle.Init.Prescaler         = uwPrescalerValue;
	  TimHandle.Init.ClockDivision     = 0;
	  TimHandle.Init.CounterMode       = TIM_COUNTERMODE_UP;
	  TimHandle.Init.RepetitionCounter = 0;
	  TimHandle.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;
	  if (HAL_TIM_Base_Init(&TimHandle) != HAL_OK)
	  {
	    /* Initialization Error */
	    while(1);
	  }

	  /*##-2- Start the TIM Base generation in interrupt mode ####################*/
	  /* Start Channel1 */
	  if (HAL_TIM_Base_Start(&TimHandle) != HAL_OK)
	  {
	    /* Starting Error */
	    while(1);
	  }
}

void PerfMon::timerReset()
{
	PERF_TIMER->CNT = 0;
}

unsigned PerfMon::timerGet()
{
	return PERF_TIMER->CNT;
}

