/*
 * UTimer.cpp
 *
 *  Created on: 19 Jul 2020
 *      Author: adam
 */

#include "UTimer.h"
#include "stm32f7xx_hal.h"
#include "stm32f7xx_hal_rcc_ex.h"

#define UTIMER TIM4
#define UTIMER_FREQ 1000000

static TIM_HandleTypeDef hUTimer;

void uTimerInit()
{
	// Activate the timer.
	__HAL_RCC_TIM4_CLK_ENABLE();
	__HAL_DBGMCU_FREEZE_TIM4();

	// Compute the prescaler value
	uint32_t uwPrescalerValue = (uint32_t) ((SystemCoreClock / 2) / UTIMER_FREQ) - 1;

	// Set TIMx instance
	hUTimer.Instance = UTIMER;
	hUTimer.Init.Period = 0xFFFFFFFF;
	hUTimer.Init.Prescaler = uwPrescalerValue;
	hUTimer.Init.ClockDivision = 0;
	hUTimer.Init.CounterMode = TIM_COUNTERMODE_UP;
	hUTimer.Init.RepetitionCounter = 0;
	hUTimer.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;
	if(HAL_TIM_Base_Init(&hUTimer) != HAL_OK) {
		while(1)
			; // Initialization Error
	}

	// Set up timer overflow interrupt (occurs evert 65536us).
	UTIMER->DIER |= TIM_DIER_UIE;
	NVIC_EnableIRQ(TIM4_IRQn);

	// Start the timer.
	if(HAL_TIM_Base_Start(&hUTimer) != HAL_OK) {
		while(1)
			; // Starting Error
	}

	// Configure the 1st channel as output compare
	if(HAL_OK != HAL_TIM_OC_Start(&hUTimer, TIM_CHANNEL_1)) {
		while(1)
			; // Failed to set up output compare.
	}
}

int usleep(useconds_t us)
{
	UTIMER->CCR1 = UTIMER->CNT + us;
	UTIMER->SR &= ~TIM_SR_CC1IF_Msk;
	while(0 == (UTIMER->SR | TIM_SR_CC1IF_Msk))
		;

	return 0;
}

static unsigned m_uTimerOverflow = 0;

void TIM4_IRQHandler()
{
	if(0 != (UTIMER->SR & TIM_SR_UIF)) {
		UTIMER->SR &= ~(TIM_SR_UIF);
		m_uTimerOverflow++;
	}
}


// Return the system uptime in milliseconds.
unsigned int fluid_curtime()
{
  return HAL_GetTick();
}

// Return the system uptime in microseconds.
double fluid_utime(void)
{
	double result = m_uTimerOverflow;
	result *= 65536;
	result += UTIMER->CNT;
	return result;
}
