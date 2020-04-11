/*
 * Systick.cpp
 *
 *  Created on: 8 Apr 2020
 *      Author: adam
 */

#include "Systick.h"
#include "stm32f7xx_hal.h"

static Systick g_systick;

Systick *Systick::instance()
{
	return &g_systick;
}

Systick::Systick()
	: _nItem(0)
{
}

void Systick::add(Delegate *delegate, uint32_t period)
{
	if(_nItem < kMaxDelegates) {
		_item[_nItem++].init(delegate, period);
	}
}

void Systick::Item::init(Delegate *delegate, uint32_t period)
{
	_period = period;
	_counter = _period;
	_delegate = delegate;
}

bool Systick::Item::isTime()
{
	if(_counter == 0)
		return true;
	--_counter;
	return false;
}

void Systick::Item::exec()
{
	if(isTime()) {
		_delegate->evtTick();
		_counter = _period;
	}
}


void Systick::tick()
{
	for(int i = 0; i < _nItem; i++) {
		_item[i].exec();
	}
}

extern "C" void HAL_IncTick()
{
  uwTick += uwTickFreq;
  g_systick.tick();
}
